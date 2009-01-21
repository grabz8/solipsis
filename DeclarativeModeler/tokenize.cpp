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

#include <list>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

static void
replace(string & s, const string & s1, const string & s2, const char skip = 0);

void
tokenize(const string & s1, list<string> & lt)
{
	
	lt.clear();

	string s(s1);

	if (s[0] == '"') s.replace(0, 1, "`` ");
	replace(s, " \"", " \"");
	replace(s, "(\"", "( `` ");
	replace(s, "[\"", "[ `` ");
	replace(s, "{\"", "{ `` ");
	replace(s, "<\"", "< `` ");

	replace(s, "...", " ... ");

	replace(s, ",", " , ");
	replace(s, ";", " ; ");
	replace(s, ":", " : ");
	replace(s, "@", " @ ");
	replace(s, "#", " # ");
	replace(s, "$", " $ ");
//	replace(s, "%", " % ");
	replace(s, "&", " & ");

	replace(s, "s.", "s . ");
	int pos = s.size() - 1;
	while (pos > 0 && s[pos] == ' ') pos--;
	while (pos > 0) {
		char c = s[pos];
		if (c == '[' || c == ']' || c == ')' || c == '}' || c == '>' ||
				c == '"' || c == '\'') {
			pos--; continue;
		}
		break;
	}
	if (s[pos] == '.' && !(pos > 0 && s[pos-1] == '.')) s.replace(pos, 1, " .");

	replace(s, "?", " ? ");
	replace(s, "!", " ! ");
	replace(s, "[", " [ ");
	replace(s, "]", " ] ");
	replace(s, "(", " ( ");
	replace(s, ")", " ) ");
	replace(s, "{", " { ");
	replace(s, "}", " } ");
	replace(s, "<", " < ");
	replace(s, ">", " > ");

	replace(s, "--", " -- ");

	s.replace(string::size_type(0), 0, " ");
	s.replace(s.size(), 0, " ");

	//  replace(s, "\"", " '' ");

	replace(s, "' ", " ' ", '\'');
	replace(s, "'s ", " 's ");
	replace(s, "'S ", " 'S ");
	replace(s, "'m ", " 'm ");
	replace(s, "'M ", " 'M ");
	replace(s, "'d ", " 'd ");
	replace(s, "'D ", " 'D ");
	replace(s, "'ll ", " 'll ");
	replace(s, "'re ", " 're ");
	replace(s, "'ve ", " 've ");
	replace(s, "n't ", " n't ");
	replace(s, "'LL ", " 'LL ");
	replace(s, "'RE ", " 'RE ");
	replace(s, "'VE ", " 'VE ");
	replace(s, "N'T ", " N'T ");

	replace(s, " Cannot ", " Can not ");
	replace(s, " cannot ", " can not ");
	replace(s, " D'ye ", " D' ye ");
	replace(s, " d'ye ", " d' ye ");
	replace(s, " Gimme ", " Gim me ");
	replace(s, " gimme ", " gim me ");
	replace(s, " Gonna ", " Gon na ");
	replace(s, " gonna ", " gon na ");
	replace(s, " Gotta ", " Got ta ");
	replace(s, " gotta ", " got ta ");
	replace(s, " Lemme ", " Lem me ");
	replace(s, " lemme ", " lem me ");
	replace(s, " More'n ", " More 'n ");
	replace(s, " more'n ", " more 'n ");
	replace(s, "'Tis ", " 'T is ");
	replace(s, "'tis ", " 't is ");
	replace(s, "'Twas ", " 'T was ");
	replace(s, "'twas ", " 't was ");
	replace(s, " Wanna ", " Wan na ");
	replace(s, " wanna ", " wanna ");

	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	for(unsigned int i=0 ; i<s.size() ; i++)
		s[i] = tolower(s[i]);

	replace(s, ".", " . ");

	replace(s, "in front of", "in_front_of");
	replace(s, "to the right of", "to_the_right_of");
	replace(s, "to the left of", "to_the_left_of");

	replace(s, "on the right of", "on_the_right_of");
	replace(s, "on the left of", "on_the_left_of");

	replace(s, "on the top left of", "on_the_top_left_of");
	replace(s, "on the top right of", "on_the_top_right_of");
	replace(s, "on the top of", "on_the_top_of");

	replace(s, "at the top left of", "at_the_top_left_of");
	replace(s, "at the top right of", "at_the_top_right_of");
	replace(s, "at the top of", "at_the_top_of");

	replace(s, "at the bottom left of", "at_the_bottom_left_of");
	replace(s, "at the bottom right of", "at_the_bottom_right_of");
	replace(s, "at the bottom of", "at_the_bottom_of");

	replace(s, "next to", "next_to");

	replace(s, "facing left", "beautiful");
	replace(s, "facing right", "beautiful");
	replace(s, "facing up", "beautiful");
	replace(s, "facing down", "beautiful");
	replace(s, "facing front", "facing_front");
	replace(s, "facing back", "facing_back");
	// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	istringstream is(s);
	string t;
	while (is >> t) {
		lt.push_back(t);
	}
}

static void
replace(string & s, const string & s1, const string & s2, const char skip)
{
	string::size_type pos = 0;
	while (1) {
		string::size_type i = s.find(s1, pos);
		if (i == string::npos) break;
		if (i > 0 && s[i-1] == skip) {
			pos = i + 1;
			continue;
		}
		s.replace(i, s1.size(), s2);
		pos = i + s2.size();
	}
}

