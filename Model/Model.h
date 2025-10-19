#ifndef MODEL_H
#define MODEL_H
#define _USE_MATH_DEFINES

#include <QDebug>
#include <QDomAttr>
#include <QFile>
#include <QPointF>
#include <QPolygonF>
#include <QString>
#include <cmath>
#define EQUATORIAL_EARTH_RADIUS_METERS 6378137
class Model {
 private:
  QList<QPolygonF> polygons;

 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
  QList<QPolygonF> getPolygons();
};
#endif  // MODEL_H
