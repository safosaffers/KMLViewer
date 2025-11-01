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
#include <limits>

#include "CoordinateConverter.h"
#include "KmlFileHandler.h"

using PolygonPair = QPair<QPolygonF, QPolygonF>;
class Model {
 private:
  QList<PolygonPair> latLonToMetersPolygons;
  QList<PolygonPair> simplifiedPolygons;
  QPointF downRightCornerForViewPort;
  QDomDocument* currentDocument;
  QString currentFilePath;

 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
  QList<PolygonPair> getPolygons();
  QPointF getDownRightCornerForViewPort();

  static PolygonPair RamerDouglasPeucker(PolygonPair latLonMetPoly,
                                         double epsilon);
  static PolygonPair createFallbackSimplification(
      const PolygonPair& originalPoly);
  static PolygonPair simplifyPolygon(PolygonPair latLonMetPoly, double epsilon);
  void simplifyPolygons(double epsilon);

  int getNumberOfPolygons();
  int getQListQPolygonFPointsCount(QList<PolygonPair> polygons);
  int getNumberOfPolygonsPoints();
  int getNumberOfSimplifiedPolygonsPoints();
  void setSimplifiedPolygons(const QList<PolygonPair>& polys);
  void saveSimplifiedModel(QString fileName);

 private:
  QList<PolygonPair> convertToMeters(QList<QPolygonF> LonLatQList,
                                     double& minLon, double& minLat);
  QPointF getCornerInMeters(double& minLon, double& maxLon, double& minLat,
                            double& maxLat);
};
#endif  // MODEL_H
