#ifndef POLYGONSIMPLIFIER_H
#define POLYGONSIMPLIFIER_H

#include <QLineF>
#include <QList>
#include <QPointF>
#include <QPolygonF>
#include <cmath>
#include <utility>  // for std::pair

#include "DeviationResult.h"
using PolygonPair = QPair<QPolygonF, QPolygonF>;

class PolygonSimplifier {
public:
  // Main simplification methods
  static PolygonPair simplifyPolygon(const PolygonPair& latLonMetPoly, double epsilon);
  static PolygonPair rammerDouglasPeucker(const PolygonPair& latLonMetPoly, double epsilon);
  static PolygonPair createFallbackSimplification(const PolygonPair& originalPoly);
  static QPointF QPointFProjectionOntoQLineF(const QPointF& p, const QLineF& line);
  static bool isProjectionPointOnLine(const QPointF& point, const QLineF& line);
  static DeviationResult calculateDeviationBetweenQPointFAndQLineF(const QPointF& point,
                                                                   const QLineF& edge);
  // For all lines in polygon calculate the shortest length and represent result as DeviationResult
  static DeviationResult calculateDeviationBetweenQPointFAndQPolygonF(const QPointF &point, const QPolygonF& polygon);
  // For all points in first check max perpendicular length to closest line in second
  static DeviationResult calculateMaxDeviationBetweenPolygons(const QPolygonF& first,
                                                        const QPolygonF& second);
  // Method to calculate max deviation between original and simplified polygons
  static DeviationResult calculateMaxDeviation(const PolygonPair& original,
                                                  const PolygonPair& simplified);


private:
  // Helper methods
  static qreal distanceBetweenLineAndPoint(const QLineF& line, const QPointF& p);
  static void findClosestToPointToLine(PolygonPair& latLonMetPoly, QLineF line,
                                       PolygonPair& result);
  static PolygonPair sortRectPointsClockwise(const PolygonPair& pair);
};

#endif  // POLYGONSIMPLIFIER_H
