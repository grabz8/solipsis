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

#include <ctype.h>

#include <algorithm>

#include <cpl_string.h>
#include <vrtdataset.h>

#include "GdalImage.h"

namespace pm_trax {

std::map<std::string, std::string> GdalImage::ext2drv;
std::map<std::string, char** > GdalImage::drvOptions;

GdalImage::GdalImage()
  : filename(""), dataset(NULL)
{
  init();
}

GdalImage::GdalImage(const GdalImage& right)
{
  *this = right;
}

GdalImage::GdalImage(const std::string& file)
  : filename(file), dataset(NULL)
{
  init();
  open();
}

GdalImage& GdalImage::operator =(const GdalImage& right)
{
  filename = right.getFilename();
  dataset = right.dataset;
  // Reference the dataset if not NULL.
  if (dataset!=NULL)
    dataset->Reference();
  return *this;
}

GdalImage::~GdalImage()
{
  // Use GDALClose to use ref/unref for shared datasets !!
  if (dataset!=NULL)
    GDALClose(dataset);
}

char* GdalImage::extract(Box2<double>& region, Vec2<int>& outsize, GDALDataType type, const Vec2d& zoomFactor)
{ 
  if (NULL==dataset)
    return NULL;
  
  // Find region to extract in pixels
  Vec2<int> ulp, size;
  getPixelRegion(region, ulp, size);
  
  //std::cout << "image size= " << dataset->GetRasterXSize() << "x" << dataset->GetRasterYSize() << std::endl;
  //std::cout << "Region= (" << region.getMin()[0] << "," << region.getMax()[1] << "," << region.getMax()[0] << "," << region.getMin()[1] << ")" << std::endl;  
  //std::cout << "ul    = (" << ulp[0] << "," << ulp[1] << ")" << std::endl;
  //std::cout << "size  = (" << size[0] << "," << size[1] << ")" << std::endl;
  
  std::string resamp;
  if(zoomFactor[0]!=1.0 || zoomFactor[1]!=1.0)
    resamp = "aver";//ATTENTION, PEUT PRODUIRE DES PIXELS A 0 SUR LES BORDS !!
  else
     resamp = "near";
  
  outsize = Vec2i(int(ceil(size[0]*zoomFactor[0])), int(ceil(size[1]*zoomFactor[1])));

  double currentGeoref[6];  
  dataset->GetGeoTransform(currentGeoref);
  
  Box2<double> newRegion = getRegion(ulp,size);
  //std::cout << "New Region= (" << newRegion.getMin()[0] << "," << newRegion.getMax()[1] << "," << newRegion.getMax()[0] << "," << newRegion.getMin()[1] << ")" << std::endl;  
  double georef[6] = { newRegion.getMin()[0], 
                       (size[0]*currentGeoref[1])/double(outsize[0]),
                       0,
                       newRegion.getMax()[1],
                       0,
                       (size[1]*currentGeoref[5])/double(outsize[1])
  };
  
  //MAJ REGION
  region = newRegion;  

  void* data = NULL;
  int pixelDataSize = 0;
  switch (type) {
  case GDT_Byte:
    data = (char*)calloc(outsize[0]*outsize[1], sizeof(char));
    pixelDataSize = sizeof(char);
    break;
  case GDT_UInt16:
    data = (unsigned short*)calloc(outsize[0]*outsize[1], sizeof(unsigned short));
    pixelDataSize = sizeof(unsigned short);
    break;
  case GDT_Int16:
    data = (short*)calloc(outsize[0]*outsize[1], sizeof(short));
    pixelDataSize = sizeof(short);
    break;
  case GDT_UInt32:
    data = (unsigned int*)calloc(outsize[0]*outsize[1], sizeof(unsigned int));
    pixelDataSize = sizeof(unsigned int);
    break;
  case GDT_Int32:
    data = (int*)calloc(outsize[0]*outsize[1], sizeof(int));
    pixelDataSize = sizeof(int);
    break;
  case GDT_Float32:
    data = (double*)calloc(outsize[0]*outsize[1], sizeof(double));
    pixelDataSize = sizeof(double);
    break;
  case GDT_Float64:
    data = (double*)calloc(outsize[0]*outsize[1], sizeof(double));
    pixelDataSize = sizeof(double);
    break;
  default:
    return NULL;
    break;
  }
  pixelDataSize *= dataset->GetRasterCount();
  
  // Create a virtual dataset with the new values.
  VRTDataset* virt = new VRTDataset(outsize[0], outsize[1]);
  virt->SetProjection(dataset->GetProjectionRef());
  Box2<double> geo = getGeoref();
  if (!geo.intersect(region))
    return NULL;
  virt->SetGeoTransform(georef);
  
  for (int i=1; i<=dataset->GetRasterCount(); i++) {
    virt->AddBand(dataset->GetRasterBand(i)->GetRasterDataType(), NULL);
    static_cast<VRTSourcedRasterBand*>(virt->GetRasterBand(i))->AddSimpleSource(dataset->GetRasterBand(i), ulp[0], ulp[1], size[0], size[1], 0, 0, outsize[0], outsize[1], resamp.c_str());
  }

  // See if dataset is grayscale or rgb
  int bands[3] = { 0, 0, 0 };
  // Find bands for R, G, B or grayscale
  for (int i=1; i<= dataset->GetRasterCount(); i++)
    switch (dataset->GetRasterBand(i)->GetColorInterpretation()) {
    case GCI_RedBand:
      bands[0] = i;
      break;
    case GCI_GreenBand:
      bands[1] = i;
      break;
    case GCI_BlueBand:
      bands[2] = i;
      break;
    case GCI_GrayIndex:
      bands[0] = bands[1] = bands[2] = i;
      break;
    default:
	  bands[i-1] = i;
      break;
    }

  if(dataset->GetRasterCount() == 1){
  	if(bands[0] == 0){
	    std::cerr << "Troubles with color interpretation" << std::endl;
    	std::cerr << "Please check the image 'ColorInterp' parameter" << std::endl;
    	return NULL;
  	}
  }
  else{
  	  if (bands[0]==0 || bands[1]==0 || bands[2]==0){
    	std::cerr << "Troubles with color interpretation" << std::endl;
    	std::cerr << "Please check the image 'ColorInterp' parameter" << std::endl;
    	return NULL;
	  }
  }
  
  // resizing and transferring data
  virt->RasterIO(GF_Read, 0, 0, outsize[0], outsize[1], data, outsize[0], outsize[1], type, dataset->GetRasterCount(), NULL, pixelDataSize, 0, 0);
  delete virt;
  
//  std::cout << "data:" << std::endl;
//  for(int q=0;q<outsize[1];q++)
//  {
//    for(int p=0;p<outsize[0];p++)
//      std::cout << ((double*)data)[p+outsize[0]*q] << "-" ;
//    std::cout << std::endl;
//  }
//  std::cout << std::endl;

  return (char *)data;

}

void GdalImage::copy(const std::string& output, const Box2<double>& region, const Vec2<double>& zoomFactor)
{
  if (NULL==dataset)
    return;

  // Get pixel region
  Vec2<int> ulp, size;
  getPixelRegion(region, ulp, size);

  // copy
  copy(output, ulp, size, zoomFactor);
}

void GdalImage::copy(const std::string& output, const Vec2<int>& offset, const Vec2<int>& size, const Vec2d& zoomFactor)
{
  if (NULL==dataset)
    return;

  std::string resamp = "aver";//ATTENTION, PEUT PRODUIRE DES PIXELS A 0 SUR LES BORDS !!
  Vec2<int> outsize(static_cast<int>(ceil(size[0]*zoomFactor[0])), static_cast<int>(ceil(size[1]*zoomFactor[1])));
  Box2<double> region = getRegion(offset, size);
  Box2<double> geo = getGeoref();

  double georef[6] = { region.getMin()[0], 
                       (region.getMax()[0]-region.getMin()[0])/double(outsize[0]),
                       0,
                       region.getMax()[1],
                       0,
                       (region.getMin()[1]-region.getMax()[1])/double(outsize[1])
  };
  
  std::cout << "georef : " << georef[0] << " " << georef[1] << " " << georef[2] << " " << georef[3] << " " << georef[4] << " " << georef[5] << std::endl;

  // Create a virtual dataset with the new values.
  VRTDataset* virt = new VRTDataset(outsize[0], outsize[1]);
  virt->SetProjection(dataset->GetProjectionRef());
  if (!geo.intersect(region))
    return;
  virt->SetGeoTransform(georef);
  
  for (int i=1; i<=dataset->GetRasterCount(); i++) {
    virt->AddBand(dataset->GetRasterBand(i)->GetRasterDataType(), NULL);
    static_cast<VRTSourcedRasterBand*>(virt->GetRasterBand(i))->AddSimpleSource(dataset->GetRasterBand(i), offset[0], offset[1], size[0], size[1], 0, 0, outsize[0], outsize[1], resamp.c_str());
  }

  // See if dataset is grayscale or rgb
  int bands[3] = { 0, 0, 0 };
  // Find bands for R, G, B or grayscale
  for (int i=1; i<= dataset->GetRasterCount(); i++)
    switch (dataset->GetRasterBand(i)->GetColorInterpretation()) {
    case GCI_RedBand:
      bands[0] = i;
      break;
    case GCI_GreenBand:
      bands[1] = i;
      break;
    case GCI_BlueBand:
      bands[2] = i;
      break;
    case GCI_GrayIndex:
      bands[0] = bands[1] = bands[2] = i;
      break;
    default:
      bands[i-1] = i;
      break;
    }

  if (bands[0]==0 || bands[1]==0 || bands[2]==0){
    std::cerr << "No color interpretation found, returning without reduction nor writing the image" << std::endl;
    return;
  }

  // Create destination dataset
  char** options = NULL;
  GDALDriver* drv = getDriverByExtension(output, options);
  if (NULL==drv)
    return;

  GDALDataset* out = drv->CreateCopy(output.c_str(), virt, false, options, GDALTermProgress, NULL);

  delete out;
  delete virt;
}

GDALDriver* GdalImage::getDriverByExtension(const std::string& fname, char**& options)
{
  // Can't find driver if there is no extension !!
  GdalImage::init();

  if (fname.length()<4)
    return NULL;
  std::string name = fname;
  std::transform(fname.begin(),fname.end(), name.begin(), tolower);
  std::string::size_type pos = name.rfind(".");
  if (pos==std::string::npos)
    return NULL;

  std::string drvname = ext2drv[name.substr(pos+1, name.length()-pos-1)];
  
  std::map<std::string, char**>::iterator it = drvOptions.find(drvname);
  if (it!=drvOptions.end())
    options = it->second;

  return GetGDALDriverManager()->GetDriverByName(drvname.c_str());
}

void GdalImage::init()
{
  if (ext2drv.empty()) {
    ext2drv["tif"]  = "GTiff";
    ext2drv["tiff"] = "GTiff";
    ext2drv["raw"]  = "ENVI";
    ext2drv["png"]  = "PNG";
    ext2drv["jpg"]  = "JPEG";
    ext2drv["jpeg"] = "JPEG";
    ext2drv["j2c"]  = "JP2KAK";
    ext2drv["jp2"]  = "JP2KAK";
    ext2drv["bmp"]  = "BMP";
    ext2drv["vrt"]  = "VRT";
  }

  if (drvOptions.empty()) {
    char** opttif = NULL;
    opttif = CSLAddString(opttif, "TFW=YES");
    drvOptions["GTiff"] = opttif;
    char** optjpeg = NULL;
    optjpeg = CSLAddString(optjpeg, "WORLDFILE=YES");
    drvOptions["JPEG"] = optjpeg;
    char** optpng = NULL;
    optpng = CSLAddString(optpng, "WORLDFILE=YES");
    drvOptions["PNG"] = optpng;
    char** optdds = NULL;
    optdds = CSLAddString(optdds, "WORLDFILE=YES");
    drvOptions["DDS"] = optdds;
  }
}

void GdalImage::getPixelRegion(const Box2<double>& region, Vec2<int>& ul, Vec2<int>& size)
{ 
  double georef[6];  
  dataset->GetGeoTransform(georef);

  ul[0] = floor((region.getMin()[0]-georef[0])/georef[1]+0.5);
  ul[1] = floor((region.getMax()[1]-georef[3])/georef[5]+0.5);  
  int lrpx = floor((region.getMax()[0]-georef[0])/georef[1]+0.5);
  int lrpy = floor((region.getMin()[1]-georef[3])/georef[5]+0.5);
  
  //std::cout << "ulp= " << ul[0] << "," << ul[1] << std::endl;
  //std::cout << "lrp= " << lrpx << "," << lrpy << std::endl;
  
  size[0] = lrpx - ul[0] + 1;
  size[1] = lrpy - ul[1] + 1;
}

Box2<double> GdalImage::getRegion(const Vec2<int>& ul, const Vec2<int>& size)
{
  double georef[6];  
  dataset->GetGeoTransform(georef);
  return Box2<double>(georef[0]+georef[1]*ul[0], georef[3]+georef[5]*(ul[1]+size[1]), georef[0]+georef[1]*(ul[0]+size[0]), georef[3]+georef[5]*ul[1]);
}

void GdalImage::open()
{
  if (filename=="")
    return;
  
  if (GetGDALDriverManager()->GetDriverCount()==0)
    GDALAllRegister();

  dataset = (GDALDataset*) GDALOpenShared(filename.c_str(), GA_ReadOnly);
}

Box2<double> GdalImage::getGeoref() const
{
  Box2<double> georef;
  double gt[6];
  if (NULL==dataset)
    return georef;

  dataset->GetGeoTransform(gt);
  georef.setBounds(gt[0], gt[3]+gt[5]*dataset->GetRasterYSize(), gt[0]+gt[1]*dataset->GetRasterXSize(), gt[3]);
  return georef;
}

Vec2<unsigned int> GdalImage::getSize() const
{
  if (NULL==dataset)
    return Vec2<unsigned int>(0, 0);
  return Vec2<unsigned int>(dataset->GetRasterXSize(), dataset->GetRasterYSize());
}

Vec2<double> GdalImage::getPixelSize() const
{
  if (NULL==dataset)
    return Vec2<double>(0.0, 0.0);
  double gt[6];
  dataset->GetGeoTransform(gt);
  return Vec2<double>(gt[1], gt[5]);
}

}; // namespace pm_trax