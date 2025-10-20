#ifndef MODEL_H
#define MODEL_H
#define _USE_MATH_DEFINES

#include <QDebug>
#include <QDomAttr>
#include <QFile>
#include <QPointF>
#include <QLineF>
#include <QPolygonF>
#include <QString>
#include <cmath>
#define EQUATORIAL_EARTH_RADIUS_METERS 6378137
class Model {
 private:
  QList<QPolygonF> polygons;
   QPointF downRightCornerForViewPort;

 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
  QList<QPolygonF> getPolygons();
  QPointF getDownRightCornerForViewPort();

  QPolygonF simplifyPolygon(QPolygonF polygon, double epsilon);
};
#endif  // MODEL_H
