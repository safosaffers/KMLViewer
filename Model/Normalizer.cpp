#include "Normalizer.h"

qreal Normalizer::calculateNormalizeFactor(QPointF maxCoord) {
  return qMax(maxCoord.x(), maxCoord.y());
}

void Normalizer::normalizePolygonPair(PolygonPair& polyPair, qreal normalizeFactor) {
  if (normalizeFactor <= 0)
    return;  // Avoid division by zero

  for (QPointF& point : polyPair.second) {  // Only normalize the meters coordinates
    point /= normalizeFactor;
  }
}

void Normalizer::normalizePolygonPairs(QList<PolygonPair>& polyPairs, qreal normalizeFactor) {
  if (normalizeFactor <= 0)
    return;  // Avoid division by zero

  for (PolygonPair& polyPair : polyPairs) {
    normalizePolygonPair(polyPair, normalizeFactor);
  }
}

void Normalizer::normalizeToRange(PolygonPair& polyPair, qreal minRange, qreal maxRange,
                                  QPointF maxCoord) {
  qreal maxDimension = qMax(maxCoord.x(), maxCoord.y());
  if (maxDimension <= 0)
    return;  // Avoid division by zero

  qreal rangeSize = maxRange - minRange;

  for (QPointF& point : polyPair.second) {
    // Normalize to [0, 1] first, then scale to desired range
    point.setX((point.x() / maxDimension) * rangeSize + minRange);
    point.setY((point.y() / maxDimension) * rangeSize + minRange);
  }
}

void Normalizer::normalizeListToRange(QList<PolygonPair>& polyPairs, qreal minRange, qreal maxRange,
                                      QPointF maxCoord) {
  for (PolygonPair& polyPair : polyPairs) {
    normalizeToRange(polyPair, minRange, maxRange, maxCoord);
  }
}