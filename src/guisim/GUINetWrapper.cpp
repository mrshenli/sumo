//---------------------------------------------------------------------------//
//                        GUINetWrapper.cpp -
//  No geometrical information is hold, here. Still, some methods for
//      displaying network-information are stored in here
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                :
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

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
// Revision 1.7  2004/04/02 11:18:37  dkrajzew
// recenter view - icon added to the popup menu
//
// Revision 1.6  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.5  2003/12/11 06:24:55  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.4  2003/11/12 13:59:04  dkrajzew
// redesigned some classes by changing them to templates
//
// Revision 1.3  2003/11/11 08:11:05  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.2  2003/08/14 13:47:44  dkrajzew
// false usage of function-pointers patched; false inclusion of .moc-files removed
//
// Revision 1.1  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream> // !!!
#include <utility>
#include <guisim/GUINet.h>
#include <gui/GUISUMOAbstractView.h>
#include "GUINetWrapper.h"
#include <gui/popup/GUIGLObjectPopupMenu.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <gui/GUIAppEnum.h>
#include <gui/icons/GUIIconSubSys.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUINetWrapper::GUINetWrapper( GUIGlObjectStorage &idStorage, GUINet &net)
    : GUIGlObject(idStorage, string("network")),
    myNet(net)
{
}


GUINetWrapper::~GUINetWrapper()
{
}


GUIGLObjectPopupMenu *
GUINetWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                 GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    new FXMenuCommand(ret, getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    new FXMenuCommand(ret, "Show Parameter", 0, ret, MID_SHOWPARS);
    return ret;
}


GUIParameterTableWindow *
GUINetWrapper::getParameterWindow(GUIApplicationWindow &app,
                                       GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 7);
    // add items
    ret->mkItem("vehicles running [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getRunningVehicleNo));
    ret->mkItem("vehicles ended [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getEndedVehicleNo));
    ret->mkItem("vehicles emitted [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getEmittedVehicleNo));
    ret->mkItem("vehicles loaded [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getLoadedVehicleNo));
    ret->mkItem("end time [s]", false,
        OptionsSubSys::getOptions().getInt("e"));
    ret->mkItem("begin time [s]", false,
        OptionsSubSys::getOptions().getInt("b"));
    ret->mkItem("time step [s]", true,
        new CastingFunctionBinding<GUINet, double, size_t>(
            &(getNet()), &GUINet::getCurrentTimeStep));
    // close building
    ret->closeBuilding();
    return ret;
}



GUIGlObjectType
GUINetWrapper::getType() const
{
    return GLO_NETWORK;
}


std::string
GUINetWrapper::microsimID() const
{
    return "";
}


Boundery
GUINetWrapper::getBoundery() const
{
    return myNet.getBoundery();
}


GUINet &
GUINetWrapper::getNet() const
{
    return myNet;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINetWrapper.icc"
//#endif

// Local Variables:
// mode:C++
// End:


