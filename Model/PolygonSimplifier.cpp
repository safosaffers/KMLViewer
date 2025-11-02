#include "PolygonSimplifier.h"

qreal PolygonSimplifier::distanceBetweenLineAndPoint(const QLineF& line, const QPointF& p) {
    // transform to local coordinates system (0,0) - (lx, ly)
    QPointF p1 = line.p1();
    QPointF p2 = line.p2();
    qreal x = p.x() - p1.x();
    qreal y = p.y() - p1.y();
    qreal x2 = p2.x() - p1.x();
    qreal y2 = p2.y() - p1.y();

    // if line is a point (nodes are the same) =>
    // just return distance between point and one line node
    qreal norm = sqrt(x2 * x2 + y2 * y2);
    if (norm <= std::numeric_limits<int>::epsilon()) return sqrt(x * x + y * y);

    // distance
    return fabs(x * y2 - y * x2) / norm;
}

PolygonPair PolygonSimplifier::rammerDouglasPeucker(const PolygonPair& latLonMetPoly, double epsilon) {
    if (latLonMetPoly.second.size() <= 2 || epsilon <= 0) return latLonMetPoly;
    PolygonPair simplifiedPolygon;
    QPointF A = latLonMetPoly.second.first();
    QPointF B = latLonMetPoly.second.last();
    QLineF AB(A, B);
    qreal d_max = 0.0;
    qsizetype C_idx = 0;
    for (qsizetype i = 1; i < latLonMetPoly.second.size() - 1; i++) {
        qreal d = distanceBetweenLineAndPoint(AB, latLonMetPoly.second.at(i));
        if (d > d_max) {
            d_max = d;
            C_idx = i;
        }
    }
    if (d_max <= epsilon) {
        simplifiedPolygon.first << latLonMetPoly.first.first()
                                << latLonMetPoly.first.last();
        simplifiedPolygon.second << latLonMetPoly.second.first()
                                 << latLonMetPoly.second.last();
    } else {
        PolygonPair A___C = {{latLonMetPoly.first.first(C_idx + 1)},
                             {latLonMetPoly.second.first(C_idx + 1)}};
        PolygonPair C___B = {
            {latLonMetPoly.first.last(latLonMetPoly.second.size() - C_idx)},
            {latLonMetPoly.second.last(latLonMetPoly.second.size() - C_idx)}};
        PolygonPair A___C_simplified = rammerDouglasPeucker(A___C, epsilon);
        PolygonPair C___B_simplified = rammerDouglasPeucker(C___B, epsilon);
        simplifiedPolygon = std::move(A___C_simplified);
        simplifiedPolygon.first.removeLast();
        simplifiedPolygon.second.removeLast();
        simplifiedPolygon.first << C___B_simplified.first;
        simplifiedPolygon.second << C___B_simplified.second;
    }
    return simplifiedPolygon;
}

void PolygonSimplifier::findClosestToPointToLine(PolygonPair& latLonMetPoly, QLineF line, PolygonPair& result) {
    if (latLonMetPoly.second.isEmpty()) return;

    qreal d_min = std::numeric_limits<qreal>::max();
    qsizetype closestPointIdx = 0;

    for (qsizetype i = 0; i < latLonMetPoly.second.size(); ++i) {
        qreal d = distanceBetweenLineAndPoint(line, latLonMetPoly.second.at(i));
        if (d < d_min) {
            d_min = d;
            closestPointIdx = i;
        }
    }

    if (closestPointIdx < latLonMetPoly.first.size() &&
        closestPointIdx < latLonMetPoly.second.size()) {
        result.first.append(latLonMetPoly.first.at(closestPointIdx));
        result.second.append(latLonMetPoly.second.at(closestPointIdx));
        latLonMetPoly.first.removeAt(closestPointIdx);
        latLonMetPoly.second.removeAt(closestPointIdx);
    }
}

PolygonPair PolygonSimplifier::sortRectPointsClockwise(const PolygonPair& pair) {
    if (pair.second.size() != 4 || pair.first.size() != 4) return pair;

    qreal cx = (pair.second[0].x() + pair.second[1].x() + pair.second[2].x() +
                pair.second[3].x()) /
               4.0;
    qreal cy = (pair.second[0].y() + pair.second[1].y() + pair.second[2].y() +
                pair.second[3].y()) /
               4.0;
    QPointF center(cx, cy);

    QVector<QPair<QPointF, QPointF>> points = {{pair.first[0], pair.second[0]},
                                               {pair.first[1], pair.second[1]},
                                               {pair.first[2], pair.second[2]},
                                               {pair.first[3], pair.second[3]}};

    std::sort(
        points.begin(), points.end(), [center](const auto& a, const auto& b) {
            qreal angleA =
                std::atan2(a.second.y() - center.y(), a.second.x() - center.x());
            qreal angleB =
                std::atan2(b.second.y() - center.y(), b.second.x() - center.x());
            return angleA > angleB;
        });

    QPolygonF sortedLatLon, sortedMeters;
    for (const auto& p : points) {
        sortedLatLon.append(p.first);
        sortedMeters.append(p.second);
    }

    return qMakePair(sortedLatLon, sortedMeters);
}

PolygonPair PolygonSimplifier::createFallbackSimplification(const PolygonPair& originalPoly) {
    QRectF rect = originalPoly.second.boundingRect();
    QLineF top(rect.topLeft(), rect.topRight());
    QLineF right(rect.topRight(), rect.bottomRight());
    QLineF bottom(rect.bottomRight(), rect.bottomLeft());
    QLineF left(rect.bottomLeft(), rect.topLeft());

    PolygonPair originalCopy = originalPoly;
    PolygonPair fallback;

    if (!originalCopy.second.isEmpty()) {
        findClosestToPointToLine(originalCopy, top, fallback);
        findClosestToPointToLine(originalCopy, right, fallback);
        findClosestToPointToLine(originalCopy, bottom, fallback);
        findClosestToPointToLine(originalCopy, left, fallback);
    }

    if (fallback.first.size() == 4 && fallback.second.size() == 4) {
        return sortRectPointsClockwise(fallback);
    } else {
        return originalPoly;
    }
}

PolygonPair PolygonSimplifier::simplifyPolygon(const PolygonPair& latLonMetPoly, double epsilon) {
    if (latLonMetPoly.first.isEmpty() || latLonMetPoly.second.isEmpty())
        return latLonMetPoly;

    // Remove duplicate closing point if present
    PolygonPair workingPoly = latLonMetPoly; // Make a copy to work with
    if (workingPoly.second.size() >= 2 &&
        qFuzzyCompare(workingPoly.second.first(),
                      workingPoly.second.last())) {
        workingPoly.first.removeLast();
        workingPoly.second.removeLast();
    }

    PolygonPair result = rammerDouglasPeucker(workingPoly, epsilon);

    // Apply fallback for small polygons that were over-simplified
    if (result.first.size() <= 4 && latLonMetPoly.first.size() >= 4) {
        result = createFallbackSimplification(latLonMetPoly);
    }

    return result;
}

double PolygonSimplifier::calculateMaxDeviation(const PolygonPair& original, const PolygonPair& simplified) {
    if (original.second.isEmpty() || simplified.second.isEmpty()) {
        return 0.0;
    }

    double maxDeviation = 0.0;

    // For each point in the original polygon, find its distance to the simplified polygon
    for (const QPointF& origPoint : original.second) {
        double minDistanceToSimplified = std::numeric_limits<double>::max();

        // Calculate distance from original point to each segment of simplified polygon
        for (int i = 0; i < simplified.second.size(); ++i) {
            QPointF p1 = simplified.second[i];
            QPointF p2 = simplified.second[(i + 1) % simplified.second.size()]; // close the polygon
            QLineF segment(p1, p2);
            
            // Calculate distance from point to line segment
            qreal distance = distanceBetweenLineAndPoint(segment, origPoint);
            if (distance < minDistanceToSimplified) {
                minDistanceToSimplified = distance;
            }
        }

        if (minDistanceToSimplified > maxDeviation) {
            maxDeviation = minDistanceToSimplified;
        }
    }

    // Also check for points in simplified polygon that may have large deviations from original
    for (const QPointF& simpPoint : simplified.second) {
        double minDistanceToOriginal = std::numeric_limits<double>::max();

        // Calculate distance from simplified point to each segment of original polygon
        for (int i = 0; i < original.second.size(); ++i) {
            QPointF p1 = original.second[i];
            QPointF p2 = original.second[(i + 1) % original.second.size()]; // close the polygon
            QLineF segment(p1, p2);
            
            // Calculate distance from point to line segment
            qreal distance = distanceBetweenLineAndPoint(segment, simpPoint);
            if (distance < minDistanceToOriginal) {
                minDistanceToOriginal = distance;
            }
        }

        if (minDistanceToOriginal > maxDeviation) {
            maxDeviation = minDistanceToOriginal;
        }
    }

    return maxDeviation;
}