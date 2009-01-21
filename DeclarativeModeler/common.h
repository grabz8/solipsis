/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Yoshimasa Tsuruoka (http://www-tsujii.is.s.u-tokyo.ac.jp/~tsuruoka/postagger/), updated by Sebastien LAIGRE

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*
 * $Id: common.h,v 1.1 2004/12/21 13:54:45 tsuruoka Exp $
 */

#ifndef __POSTAGGER_COMMON_H_
#define __POSTAGGER_COMMON_H_

#include <string>
#include <vector>

struct Token
{
  std::string str;
  std::string pos;
  std::string prd;
  Token(std::string s, std::string p) : str(s), pos(p) {}
};

typedef std::vector<Token> Sentence;


#endif

/*
 * $Log: common.h,v $
 * Revision 1.1  2004/12/21 13:54:45  tsuruoka
 * add bidir.cpp
 *
 */
