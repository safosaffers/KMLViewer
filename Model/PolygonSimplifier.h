#ifndef POLYGONSIMPLIFIER_H
#define POLYGONSIMPLIFIER_H

#include <QPointF>
#include <QPolygonF>
#include <QList>
#include <QLineF>
#include <utility> // for std::pair
#include <cmath>
#include <limits>

using PolygonPair = QPair<QPolygonF, QPolygonF>;

class PolygonSimplifier {
public:
    // Main simplification methods
    static PolygonPair simplifyPolygon(const PolygonPair& latLonMetPoly, double epsilon);
    static PolygonPair rammerDouglasPeucker(const PolygonPair& latLonMetPoly, double epsilon);
    static PolygonPair createFallbackSimplification(const PolygonPair& originalPoly);
    
private:
    // Helper methods
    static qreal distanceBetweenLineAndPoint(const QLineF& line, const QPointF& p);
    static void findClosestToPointToLine(PolygonPair& latLonMetPoly, QLineF line, PolygonPair& result);
    static PolygonPair sortRectPointsClockwise(const PolygonPair& pair);
};

#endif // POLYGONSIMPLIFIER_H