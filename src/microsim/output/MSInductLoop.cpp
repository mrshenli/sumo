/**
 * @file   MSInductLoop.cpp
 * @author Christian Roessel
 * @date   Mon Jul 21 16:12:01 2003
 * @version $Id$
 * @brief  Definition of class MSInductLoop.
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2005/02/01 10:10:43  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.4  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.5  2004/12/10 11:42:53  dksumo
// detectors usage reworked
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <helpers/SimpleCommand.h>
#include <utils/convert/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member variable definitions
 * ======================================================================= */
string MSInductLoop::xmlHeaderM(
"<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
"<!--\n"
"- nVehContrib is the number of vehicles that passed the detector during the\n"
"  current interval.\n"
"- flow [veh/h] denotes the same quantity in [veh/h]\n"
"- occupancy [%] is the time the detector was occupied by vehicles.\n"
"- speed [m/s] is the average speed of the nVehContib vehicles.\n"
"  If nVehContrib==0, speed is set to -1.\n"
//"- speedsquare [(m/s)^2]\n"
//"  If nVehContrib==0, speedsquare is set to -1.\n"
"- length [m] is the average vehicle length of the nVehContrib vehicles.\n"
"  If nVehContrib==0, length is set to -1.\n"
"-->\n\n");

string MSInductLoop::xmlDetectorInfoEndM( "</detector>\n" );


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSInductLoop::MSInductLoop( const string& id,
                            MSLane* lane,
                            double positionInMeters,
                            SUMOTime deleteDataAfterSeconds )
    : MSMoveReminder( lane, id ),
      posM( MSNet::getCells( positionInMeters ) ),
      deleteDataAfterStepsM( MSNet::getSteps( deleteDataAfterSeconds ) ),
      lastLeaveTimestepM( 0 ),
      vehiclesOnDetM(),
      vehicleDataContM()
{
    assert( posM >= 0 && posM <= laneM->length() );

    // insert object into dictionary
    if ( ! InductLoopDict::getInstance()->isInsertSuccess( idM,
                                                           this ) ) {
        MsgHandler::getErrorInstance()->inform(
            "induct loop '" + idM + "' could not be build;");
        MsgHandler::getErrorInstance()->inform(
            " (declared twice?)");
        throw ProcessError();
    }

    // start old-data removal through MSEventControl
    Command* deleteOldData = new SimpleCommand< MSInductLoop >(
        this, &MSInductLoop::deleteOldData );
    MSEventControl::getEndOfTimestepEvents()->addEvent(
        deleteOldData,
        deleteDataAfterStepsM,
        MSEventControl::ADAPT_AFTER_EXECUTION );
}

MSInductLoop::~MSInductLoop()
{
    vehiclesOnDetM.clear();
    vehicleDataContM.clear();
}


bool
MSInductLoop::isStillActive( MSVehicle& veh,
                             double oldPos,
                             double newPos,
                             double newSpeed )
{
    double entryTimestep = MSNet::getInstance()->timestep();
    double leaveTimestep = entryTimestep;

    if ( newPos < posM ) {
        // detector not reached yet
        return true;
    }
    double speed = newSpeed * MSNet::deltaT();
    if ( vehiclesOnDetM.find( &veh ) == vehiclesOnDetM.end() ) {
        // entered the detector by move
        entryTimestep -= 1 - ( posM - oldPos ) / speed;
        if ( newPos - veh.length() > posM ) {
            // entered and passed detector in a single timestep
            leaveTimestep -= ( newPos - veh.length() - posM ) / speed;
            enterDetectorByMove( veh, entryTimestep );
            leaveDetectorByMove( veh, leaveTimestep );
            return false;
        }
        // entered detector, but not passed
        enterDetectorByMove( veh, entryTimestep );
        return true;
    } else {
        // vehicle has been on the detector the previous timestep
        if ( newPos - veh.length() >= posM ) {
            // vehicle passed the detector
            leaveTimestep -= ( newPos - veh.length() - posM ) / speed;
            leaveDetectorByMove( veh, leaveTimestep );
            return false;
        }
        // vehicle stays on the detector
        return true;
    }
}


void
MSInductLoop::dismissByLaneChange( MSVehicle& veh )
{
    if ( veh.pos() > posM && veh.pos() - veh.length() <= posM ) {
        // vehicle is on detector during lane change
        leaveDetectorByLaneChange( veh );
    }
}


bool
MSInductLoop::isActivatedByEmitOrLaneChange( MSVehicle& veh )
{
    if ( veh.pos() > posM ) {
        // vehicle-front is beyond detector. Ignore
        return false;
    }
    // vehicle is in front of detector
    return true;
}


double
MSInductLoop::getFlow( SUMOTime lastNTimesteps ) const
{
    // return unit is [veh/h]
    assert( lastNTimesteps > 0 );
    MSInductLoop::DismissedCont::const_iterator mend = dismissedContM.end();
    return MSNet::getVehPerHour(
        ( getNVehContributed( lastNTimesteps ) + distance(
            getDismissedStartIterator( lastNTimesteps ), mend ) )
            / (double) lastNTimesteps);
}


double
MSInductLoop::getMeanSpeed( SUMOTime lastNTimesteps ) const
{
    // return unit is [m/s]
    assert( lastNTimesteps > 0 );
    double nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return -1;
    }
    double speedS = accumulate( getStartIterator( lastNTimesteps ),
                                vehicleDataContM.end(),
                                0.0,
                                speedSum );
    return MSNet::getMeterPerSecond( speedS / nVeh );
}

/*
double
MSInductLoop::getMeanSpeedSquare( SUMOTime lastNTimesteps ) const
{
    // unit is [(m/s)^2]
    assert( lastNTimesteps > 0 );
    double nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return -1;
    }
    double speedSquareS = accumulate( getStartIterator( lastNTimesteps ),
                                      vehicleDataContM.end(),
                                      0.0,
                                      speedSquareSum );
    return MSNet::getMeterPerSecond( MSNet::getMeterPerSecond(
                                         speedSquareS / nVeh ) );
}
*/

double
MSInductLoop::getOccupancy( SUMOTime lastNTimesteps ) const
{
    // unit is [%]
    assert( lastNTimesteps > 0 );
    double nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return 0;
    }
    return accumulate( getStartIterator( lastNTimesteps ),
                       vehicleDataContM.end(),
                       0.0,
                       occupancySum ) /
        static_cast<double>( lastNTimesteps );
}


double
MSInductLoop::getMeanVehicleLength( SUMOTime lastNTimesteps ) const
{
    assert( lastNTimesteps > 0 );
    double nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return -1;
    }
    double lengthS = accumulate( getStartIterator( lastNTimesteps ),
                                 vehicleDataContM.end(),
                                 0.0,
                                 lengthSum );
    return MSNet::getMeters( lengthS / nVeh );
}


double
MSInductLoop::getTimestepsSinceLastDetection() const
{
    // This method was formely called  getGap()
    if ( vehiclesOnDetM.size() != 0 ) {
        // detetctor is occupied
        return 0;
    }
    return MSNet::getInstance()->timestep() - lastLeaveTimestepM;
}


double
MSInductLoop::getNVehContributed( SUMOTime lastNTimesteps ) const
{
    return (double) distance( getStartIterator( lastNTimesteps ),
                     vehicleDataContM.end() );
}


string
MSInductLoop::getNamePrefix( void ) const
{
    return "MSInductLoop_" + idM;
}


void
MSInductLoop::writeXMLHeader( XMLDevice &dev ) const
{
    dev.writeString(xmlHeaderM);
}


void
MSInductLoop::writeXMLDetectorInfoStart( XMLDevice &dev ) const
{
    dev.writeString("<detector type=\"inductionloop\" id=\"" + idM +
        "\" lane=\"" + laneM->id() + "\" pos=\"" +
        toString( MSNet::getMeters( posM ) ) + "\" >");
}


void
MSInductLoop::writeXMLDetectorInfoEnd( XMLDevice &dev ) const
{
    dev.writeString(xmlDetectorInfoEndM);
}


void
MSInductLoop::writeXMLOutput(XMLDevice &dev,
                             SUMOTime startTime, SUMOTime stopTime )
{
    SUMOTime t( stopTime-startTime+1 );
    MSInductLoop::DismissedCont::const_iterator mend = dismissedContM.end();
    size_t nVehCrossed = ((size_t) getNVehContributed(t))
        + distance(
            getDismissedStartIterator( t ), mend );
    dev.writeString("<interval begin=\"").writeString(
        toString(startTime)).writeString("\" end=\"").writeString(
        toString(stopTime)).writeString("\" ");
    if(dev.needsDetectorName()) {
        dev.writeString("id=\"").writeString(idM).writeString("\" ");
    }
    dev.writeString("nVehContrib=\"").writeString(
        toString(getNVehContributed( t ))).writeString("\" flow=\"").writeString(
        toString(getFlow( t ))).writeString("\" occupancy=\"").writeString(
        toString(getOccupancy( t ))).writeString("\" speed=\"").writeString(
        toString(getMeanSpeed( t ))).writeString("\" length=\"").writeString(
        toString(getMeanVehicleLength( t ))).writeString("\" nVehCrossed=\"").writeString(
        toString(nVehCrossed)).writeString("\"/>");
}


SUMOTime
MSInductLoop::getDataCleanUpSteps( void ) const
{
    return deleteDataAfterStepsM;
}


GUIDetectorWrapper*
MSInductLoop::buildDetectorWrapper(GUIGlObjectStorage &,
                                   GUILaneWrapper &)
{
    throw "Only within the gui-version";
}


void
MSInductLoop::enterDetectorByMove( MSVehicle& veh,
                                   double entryTimestep )
{
    vehiclesOnDetM.insert( make_pair( &veh, entryTimestep ) );
}


void
MSInductLoop::leaveDetectorByMove( MSVehicle& veh,
                                   double leaveTimestep )
{
    VehicleMap::iterator it = vehiclesOnDetM.find( &veh );
    assert( it != vehiclesOnDetM.end() );
    double entryTimestep = it->second;
    vehiclesOnDetM.erase( it );
    assert( entryTimestep < leaveTimestep );
    vehicleDataContM.push_back( VehicleData( veh, entryTimestep,
                                             leaveTimestep ) );
    lastLeaveTimestepM = leaveTimestep;
}


void
MSInductLoop::leaveDetectorByLaneChange( MSVehicle& veh )
{
    // Discard entry data
    vehiclesOnDetM.erase( &veh );
    dismissedContM.push_back(MSNet::getInstance()->getCurrentTimeStep());
/*    double leaveTimestep = ( newPos - veh.length() - posM ) / speed;
    myDismissedByLaneChange++;*/
}


SUMOTime
MSInductLoop::deleteOldData( void )
{
    double deleteBeforeTimestep =
        MSNet::getInstance()->timestep() - deleteDataAfterStepsM;
    if ( deleteBeforeTimestep > 0 ) {
        vehicleDataContM.erase(
            vehicleDataContM.begin(),
            lower_bound( vehicleDataContM.begin(),
                         vehicleDataContM.end(),
                         deleteBeforeTimestep,
                         leaveTimeLesser() ) );
        dismissedContM.erase(
            dismissedContM.begin(),
            lower_bound( dismissedContM.begin(),
                         dismissedContM.end(),
                         deleteBeforeTimestep ) );
    }
    return deleteDataAfterStepsM;
}


MSInductLoop::VehicleDataCont::const_iterator
MSInductLoop::getStartIterator( SUMOTime lastNTimesteps ) const
{
    double startTime = 0;
    if ( lastNTimesteps < MSNet::getInstance()->timestep() ) {
        startTime = static_cast< double >(
            MSNet::getInstance()->timestep() - lastNTimesteps ) *
            MSNet::deltaT();
    }
    return lower_bound( vehicleDataContM.begin(),
                        vehicleDataContM.end(),
                        startTime,
                        leaveTimeLesser() );
}

MSInductLoop::DismissedCont::const_iterator
MSInductLoop::getDismissedStartIterator( SUMOTime lastNTimesteps ) const
{
    double startTime = 0;
    if ( lastNTimesteps < MSNet::getInstance()->timestep() ) {
        startTime = static_cast< double >(
            MSNet::getInstance()->timestep() - lastNTimesteps ) *
            MSNet::deltaT();
    }
    return lower_bound( dismissedContM.begin(),
                        dismissedContM.end(),
                        startTime );
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
