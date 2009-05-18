/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo / MERLET Jonathan

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

#ifndef _RoadWay_h_
#define _RoadWay_h_

#include <vector>
#include <string>
#include <iostream>

namespace roge {

	#define INTERSECTION_NULL 99999999
	
	class Pavement;
	
	/*
	 * Represents a roadway
	 */
	class RoadWay {
	public:
		/*
		 * Default constructor
		 */
		RoadWay();
	
		/*
		 * Copy constructor
		 */
		RoadWay(const RoadWay& right);
	
		/*
		 * Default destructor
		 */
		virtual ~RoadWay();
	
		/*
		 * Gets the name of this roadway
		 * \return name of the roadway
		 */
		std::string getName() const;
	
		/*
		 * sets the name of this roadway
		 * \param name Name of the roadway
		 */
		void setName(std::string name);
	
		/*
		 * Gets one of the two intersections of this roadway
		 * \param number can take value 0 or 1
		 */
		unsigned int getIntersection(unsigned int number) const;
	
		/*
		 * Sets one of the two intersections of the roadway
		 * \param intersection pointer to the intersection
		 * \param number number of the intersection (0 or 1)
		 */
		void setIntersection(unsigned int interIndex, unsigned int number);
	
		/*
		 * Assignment operator
		 */
		RoadWay& operator=(const RoadWay& right);
	
		bool operator==(const RoadWay& right);
	
		unsigned int getPavementNum() const;
	
		Pavement* getPavement(int number) const;
	
		/*
		 * Adds a new Pavement to this road
		 */
		void addPavement(Pavement* left);
	
		/*
		 * Removes a pavement from this road
		 */
		void remPavement(int number);
		
		/*
		 * Delete pavements used by this roadway
		 */
		void deletePavements();
	
		/*
		 * Gets nature of the road 
		 */
		std::string getNature() const;
	
		/*
		 * Sets nature ofthe roadway
		 */
		void setNature(std::string nature);
	
		void setNbLine(unsigned int nb) {
			_nbLine = nb;
		}
	
		unsigned int getNbLine() const {
			return _nbLine;
		}
	
		double getStartingWidth() const;
	
		double getEndingWidth() const;
	
		void setTexLength(double texLength) {
			_texLength = texLength;
		}
	
		double getTexLength() {
			return _texLength;
		}
	
		bool getDifferentWidth() {
			return _differentWidths;
		}
	
		void setDifferentWidth(bool diff) {
			_differentWidths = diff;
		}
	
	protected:
		std::string _name;
	
		std::string _nature;
	
		std::vector<Pavement*> _pavements;
	
		std::pair<unsigned int, unsigned int> _intersections; // index of intersection in road network
	
		unsigned int _nbLine;
	
		double _texLength; //he length of the texture associated to this roadway
	
		bool _differentWidths;
	};

} //namespace arSolipsisRoadGenerator
#endif /* ARROADWAY_H_HEADER_INCLUDED_BFA8860D */
