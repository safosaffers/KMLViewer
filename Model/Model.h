#ifndef MODEL_H
#define MODEL_H
#define _USE_MATH_DEFINES

#include <QDebug>
#include <QDomAttr>
#include <QFile>
#include <QPointF>
#include <QString>
#include <cmath>

#include "../Common/Common.h"
#define EQUATORIAL_EARTH_RADIUS_METERS 6378137
class Model {
private:
  QList<Polygon> polygons;

 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
  QList<Polygon> getPolygons();
};
#endif  // MODEL_H
