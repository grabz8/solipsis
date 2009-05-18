#include "arBSpline.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osgDB/WriteFile>

int main() {


  vector<arVec3<double> > controlPoints;
  controlPoints.push_back(arVec3<double>(0,0,0));
  controlPoints.push_back(arVec3<double>(0,1,0));
  controlPoints.push_back(arVec3<double>(5,5,0));
  controlPoints.push_back(arVec3<double>(10,5,2));
  controlPoints.push_back(arVec3<double>(10,5,10));
  controlPoints.push_back(arVec3<double>(15,10,10));
  controlPoints.push_back(arVec3<double>(15,15,15));
  controlPoints.push_back(arVec3<double>(20,20,20));
  controlPoints.push_back(arVec3<double>(20,15,30));
  controlPoints.push_back(arVec3<double>(20,15,40));
  controlPoints.push_back(arVec3<double>(15,20,40));


  arBSpline spline(controlPoints,0.05,false);

  vector<arVec3<double> >* splinePoints  = spline.compute();

  osg::ref_ptr<osg::Vec3Array> osgControlPoints = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec3Array> osgPoints = new osg::Vec3Array();

  for(int i=0; i<controlPoints.size(); i++) {
    osgControlPoints->push_back(osg::Vec3(controlPoints[i][0],controlPoints[i][1],controlPoints[i][1]));
  }

  for(int i=0; i<splinePoints->size(); i++) {
    osgPoints->push_back(osg::Vec3((*splinePoints)[i][0],(*splinePoints)[i][1],(*splinePoints)[i][1]));
  }

  osg::ref_ptr<osg::Geometry> controlGeom = new osg::Geometry();
  controlGeom->setVertexArray(osgControlPoints.get());
  controlGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,controlPoints.size()));

 
  osg::ref_ptr<osg::Geometry> splineGeom = new osg::Geometry();
  osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array();
  color->push_back(osg::Vec4(1,0,0,1));
  splineGeom->setColorArray(color.get());
  splineGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
  splineGeom->setVertexArray(osgPoints.get());
  splineGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,splinePoints->size()));

  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
  geode->addDrawable(controlGeom.get());
  geode->addDrawable(splineGeom.get());

  osgDB::writeNodeFile(*geode,"spline.osg");
}
