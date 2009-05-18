/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author HUIBAN Vincent

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

#ifndef GdalImage_h
#define GdalImage_h 1

#include <map>
#include <string>
#include <vector>

#include <gdal_priv.h>

#include <Linear/Box.h>

namespace pm_trax {

class GdalImage 
{

public:

  GdalImage();

  GdalImage(const GdalImage& right);

  GdalImage(const std::string& file);

  GdalImage& operator =(const GdalImage& right);
  
  ~GdalImage();
  
  void setFilename(const std::string& file)
  {
    filename = file;
    open();
  }
  
  std::string getFilename() const
  {
    return filename;
  }

  Box2<double> getGeoref() const;

  Vec2<unsigned int> getSize() const;

  Vec2<double> getPixelSize() const;
 
  char* extract(Box2<double>& region, Vec2<int>& pixelSize, GDALDataType type = GDT_Byte, const Vec2d& zoomFactor = Vec2d(1.0, 1.0));

  void copy(const std::string& output, const Box2<double>& region, const Vec2<double>& zoomFactor = Vec2d(1, 1));

  void copy(const std::string& output, const Vec2<int>& offset, const Vec2<int>& size, const Vec2<double>& zoomFactor = Vec2d(1, 1));
 
  Box2<double> getRegion(const Vec2<int>& ul, const Vec2<int>& size);

  void getPixelRegion(const Box2<double>& region, Vec2<int>& ul, Vec2<int>& size);
  
  GDALDataset* getDataset() { return dataset; };
 
protected:

  static GDALDriver* getDriverByExtension(const std::string& fname, char**& options);

  static void init();

  void open();

  std::string filename;

  GDALDataset* dataset;

  static std::map<std::string, std::string> ext2drv;
  static std::map<std::string, char** > drvOptions;

}; // class GdalImage

}; // namespace pm_trax

#endif
