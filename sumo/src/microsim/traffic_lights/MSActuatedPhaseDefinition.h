#ifndef MSActuatedPhaseDefinition_h
#define MSActuatedPhaseDefinition_h
//---------------------------------------------------------------------------//
//                        MSActuatedPhaseDefinition.h -
//  The definition of a single phase of an extended tls logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// $Log$
// Revision 1.2  2005/01/27 14:22:44  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSPhaseDefinition.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSActuatedPhaseDefinition
 * The definition of a single phase of an extended tls logic.
 * We moved it out of the main class to allow later template
 * parametrisation.
 */
class MSActuatedPhaseDefinition : public MSPhaseDefinition{
public:
    /// The minimum duration of the phase
    size_t minDuration;

    /// The maximum duration of the phase
    size_t maxDuration;

    /// stores the timestep of the last on-switched of the phase
    MSNet::Time _lastSwitch;

    /// constructor
    MSActuatedPhaseDefinition(size_t durationArg,
        const std::bitset<64> &driveMaskArg, const std::bitset<64> &breakMaskArg,
        const std::bitset<64> &yellowMaskArg,
        size_t minDurationArg, size_t maxDurationArg)
        : MSPhaseDefinition(durationArg, driveMaskArg,
            breakMaskArg, yellowMaskArg),
        minDuration(minDurationArg), maxDuration(maxDurationArg),
        _lastSwitch(0)
    {
        _lastSwitch = OptionsSubSys::getOptions().getInt("b");
        // defines minDuration
        size_t minDurationDefault = 10;
        if  (minDurationArg < 0) {
            if (durationArg < minDurationDefault) {
                minDuration = durationArg;
            } else {
                minDuration = minDurationDefault;
            }
        } else {
            minDuration = minDurationArg;
        }
        // defines maxDuration (maxDuration is only used in MSActuatedTraffifLight Logic)
        size_t maxDurationDefault = 30;
        if  (maxDurationArg < 0 ) {
            if (durationArg > maxDurationDefault) {
                maxDuration = durationArg;
            } else {
                maxDuration = maxDurationDefault;
            }
        } else {
            maxDuration = maxDurationArg;
        }
    }

    /// destructor
    ~MSActuatedPhaseDefinition() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
