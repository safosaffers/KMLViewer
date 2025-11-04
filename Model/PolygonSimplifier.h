#ifndef POLYGONSIMPLIFIER_H
#define POLYGONSIMPLIFIER_H

#include <QLineF>
#include <QList>
#include <QPointF>
#include <QPolygonF>
#include <cmath>
#include <limits>
#include <utility>  // for std::pair

using PolygonPair = QPair<QPolygonF, QPolygonF>;

class PolygonSimplifier {
public:
  // Main simplification methods
  static PolygonPair simplifyPolygon(const PolygonPair& latLonMetPoly, double epsilon);
  static PolygonPair rammerDouglasPeucker(const PolygonPair& latLonMetPoly, double epsilon);
  static PolygonPair createFallbackSimplification(const PolygonPair& originalPoly);

  // Method to calculate max deviation between original and simplified polygons
  static double calculateMaxDeviation(const PolygonPair& original, const PolygonPair& simplified);

private:
  // Helper methods
  static qreal distanceBetweenLineAndPoint(const QLineF& line, const QPointF& p);
  static void findClosestToPointToLine(PolygonPair& latLonMetPoly, QLineF line,
                                       PolygonPair& result);
  static PolygonPair sortRectPointsClockwise(const PolygonPair& pair);
};

#endif  // POLYGONSIMPLIFIER_H