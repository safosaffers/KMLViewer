#ifndef MODEL_H
#define MODEL_H
#define _USE_MATH_DEFINES

#include <QDebug>
#include <QDomAttr>
#include <QFile>
#include <QLineF>
#include <QPointF>
#include <QPolygonF>
#include <QString>
#include <cmath>
#define EQUATORIAL_EARTH_RADIUS_METERS 6378137
class Model {
 private:
  QList<QPolygonF> polygons;
  QList<QPolygonF> simplifiedPolygons;
  QPointF downRightCornerForViewPort;

 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
  QList<QPolygonF> getPolygons();
  QPointF getDownRightCornerForViewPort();

  qreal distanceBetweenQLineFAndPoint(const QLineF& line,
                                      const QPointF& p) const;
  QPolygonF simplifyPolygon(QPolygonF polygon, double epsilon);
  void simplifyPolygons(double epsilon);
  QList<QPolygonF> getSimplifiedPolygons();
};
#endif  // MODEL_H
