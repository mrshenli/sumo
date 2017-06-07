/****************************************************************************/
/// @file    NBPtStop.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// The representation of a single pt stop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMO_NBPTSTOP_H
#define SUMO_NBPTSTOP_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/geom/Position.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class NBPTStop
* @brief The representation of a single pt stop
*/
class NBPTStop {

public:
    /**@brief Constructor
    * @param[in] id The id of the pt stop
    * @param[in] position The position of the pt stop
    * @param[in] edgeId The edge id of the pt stop
    * @param[in] length The length of the pt stop
    */
    NBPTStop(std::string ptStopId, Position position, std::string edgeId, std::string origEdgeId, double length, std::string name, SVCPermissions svcPermissions);
    std::string getID() const;

    const std::string getEdgeId();
    const std::string getOrigEdgeId();
    const std::string getName();
    const Position& getPosition();
    SVCPermissions getPermissions();
    void computExtent(double center, double d);
    void setLaneID(const std::string& laneId);
    void write(OutputDevice& device);
    void reshiftPostion(const double offsetX, const double offsetY);

    void addPlatformPosCand(Position position);
    std::vector<Position>& getPlatformPosCands();
    bool getIsMultipleStopPositions();
    void setIsMultipleStopPositions(bool multipleStopPositions);
    double getLength();
    void setEdgeId(std::string edgeId);
private:
    const std::string myPTStopId;
    Position myPosition;
    std::string myEdgeId;
    const std::string myOrigEdgeId;
    const double myPTStopLength;
    const std::string myName;
    std::string myLaneId;
    const SVCPermissions myPermissions;

    double myStartPos;
    double myEndPos;


private:
    /// @brief Invalidated assignment operator.
    NBPTStop& operator=(const NBPTStop&);


    std::vector<Position> myPlatformPosCands;
    bool myIsMultipleStopPositions;
};

#endif //SUMO_NBPTSTOP_H
