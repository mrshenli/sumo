//---------------------------------------------------------------------------//
//                        NISUMOHandlerNodes.cpp -
//  A handler for SUMO nodes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
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
// Revision 1.12  2006/11/16 10:50:46  dkrajzew
// warnings removed
//
// Revision 1.11  2006/04/05 05:32:26  dkrajzew
// code beautifying: embedding string in strings removed
//
// Revision 1.10  2005/10/07 11:39:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/23 06:02:34  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 12:24:36  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/08/02 12:44:12  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.5  2004/01/12 15:31:53  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.4  2003/07/07 08:27:53  dkrajzew
// adapted the importer to the new node type description
//
// Revision 1.3  2003/06/18 11:15:07  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/04/09 15:53:17  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.1  2003/02/07 11:13:27  dkrajzew
// names changed
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "NISUMOHandlerNodes.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

NISUMOHandlerNodes::NISUMOHandlerNodes(NBNodeCont &nc, LoadFilter what)
    : SUMOSAXHandler("sumo-network"),
    _loading(what),
    myNodeCont(nc)
{
}


NISUMOHandlerNodes::~NISUMOHandlerNodes()
{
}


void
NISUMOHandlerNodes::myStartElement(int element, const std::string &/*name*/,
                                   const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_JUNCTION:
        if(_loading==LOADFILTER_ALL) {
            addNode(attrs);
        }
        break;
    default:
        break;
    }
}


void
NISUMOHandlerNodes::addNode(const Attributes &attrs)
{
    string id;
    try {
        // retrieve the id of the node
        id = getString(attrs, SUMO_ATTR_ID);
/*        string name = id;
        // retrieve the name of the node
        try {
            name = getString(attrs, SUMO_ATTR_NAME);
        } catch (EmptyData) {
        }*/
        string typestr;
        // get the type of the node
        try {
            typestr = getString(attrs, SUMO_ATTR_TYPE);
        } catch (EmptyData) {
            addError("The type of the junction '" + id + "' is not given.");
            return;
        }
        // check whether the type string is valid by converting it to the known
        //  junction types
        NBNode::BasicNodeType type = NBNode::NODETYPE_UNKNOWN;
        if(typestr=="none") {
            type = NBNode::NODETYPE_NOJUNCTION;
        } else if(typestr=="priority") {
            type = NBNode::NODETYPE_PRIORITY_JUNCTION;
        } else if(typestr=="right_before_left") {
            type = NBNode::NODETYPE_RIGHT_BEFORE_LEFT;
        } else if(typestr=="DEAD_END") {
            type = NBNode::NODETYPE_DEAD_END;
        }
        if(type<0) {
            addError("The type '" + typestr + "' of junction '" + id + "is not known.");
            return;
        }
        // get the position of the node
        SUMOReal x, y;
        x = getFloatSecure(attrs, SUMO_ATTR_X, -1);
        y = getFloatSecure(attrs, SUMO_ATTR_Y, -1);
        if(x<0||y<0) {
            if(x<0) {
                addError("The x-position of the junction '" + id + "' is not valid.");
            }
            if(y<0) {
                addError("The y-position of the junction '" + id + "' is not valid.");
            }
            return;
        }
        // get the key
        string key;
        try {
            key = getString(attrs, SUMO_ATTR_KEY);
        } catch (EmptyData) {
            addError("The key is missing for junction '" + id + "'.");
        }
        // build the node
        throw 1; // !!! deprecated
        myNodeCont.insert(new NBNode(id, Position2D(x, y), type));
    } catch (EmptyData) {
        addError("A junction without an id occured.");
    }
}


void
NISUMOHandlerNodes::myCharacters(int element, const std::string &name,
                                 const std::string &chars)
{
    myCharactersDump(element, name, chars);
}

void
NISUMOHandlerNodes::myEndElement(int element, const std::string &name)
{
    myEndElementDump(element, name);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


