/****************************************************************************/
/// @file    StringTokenizer.h
/// @author  Daniel Krajzewicz
/// @date    ?
/// @version $Id: $
///
// A java-style StringTokenizer for c++ (stl)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef StringTokenizer_h
#define StringTokenizer_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>

/**
 * StringTokenizer
 * A class similar to the StringTokenizer from Java. It splits a string at
 * the given string or character (or one of the special cases NEWLINE or
 * WHITECHAR) and allows to iterate over the so generated substrings.
 *
 * The normal usage is like this:
 * <pre>
 * StringTokenizer st(CString("This is a line"), ' ');
 * while(st.hasNext())
 *    cout << st.next() << endl;
 * </pre>
 * This would generate the output:
 * <pre>
 * This
 * is
 * a
 * line
 * </pre>
 *
 * There is something to know about the behaviour:
 * When using WHITECHAR, a list of whitechars occuring in  the string to
 * split is regarded as a single divider. All other parameter will use
 * multiple occurences of operators as a list of single divider and the
 * string between them will have a length of zero.
 */
// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class StringTokenizer
{
public:
    /** identifier for splitting the given string at all newline characters */
    static const int NEWLINE;

    /** identifier for splitting the given string at all whitespace
        characters */
    static const int WHITECHARS;

public:
    /** default constructor */
    StringTokenizer()
    { }

    /** @brief constructor
        same as StringTokenizer(tosplit, StringTokenizer.WHITECHARS)
        tosplit is the string to split into substrings */
    StringTokenizer(std::string tosplit);

    /** @brief constructor
        the first string will be split at the second string's occurences */
    StringTokenizer(std::string tosplit, std::string token);

    /** @brief constructor
        The second parameter may be a character - the strig will then be
        split at all occurences of this character.
        When StringTokenizer.NEWLINE is used as second parameter, the string
        will be split at all occurences of a newline character (0x0d / 0x0a)
        When StringTokenizer.WHITECHARS is used as second parameter, the
        string will be split at all characters below 0x20
        The behaviour is undefined for other parameter */
    StringTokenizer(std::string tosplit, int special);

    /** destructor */
    ~StringTokenizer();

    /** reinitialises the internal iterator */
    void reinit();

    /** returns the information whether further substrings exist */
    bool hasNext();

    /** returns the next substring when it exists. Otherwise the behaviour is
        undefined */
    std::string next();

    /** returns the number of existing substrings */
    size_t size() const;

    /** returns the first substring without moving the iterator */
    std::string front();

    /** returns the item at the given position */
    std::string get(size_t pos) const;

    std::vector<std::string> getVector();

private:
    /** splits the first string at all occurences of the second */
    void prepare(const std::string &tosplit, const std::string &token);

    /** splits the first string at all occurences of the given char */
    void prepare(const std::string &tosplit, char token);

    /** unsused */
    void prepare(const std::string &tosplit, const std::string &token,
                 int dummy);

    /** splits the first string at all occurences of whitechars */
    void prepareWhitechar(const std::string &tosplit);

    /** splits the first string at all occurences of newlines */
    void prepareNewline(const std::string &tosplit);

private:
    /** a list of positions/lengths */
    typedef std::vector<size_t> SizeVector;

    /** the string to split */
    std::string   _tosplit;

    /** the current position in the list of substrings */
    size_t        _pos;

    /** the list of substring starts */
    SizeVector    _starts;

    /** the list of substring lengths */
    SizeVector    _lengths;

};


#endif

/****************************************************************************/

