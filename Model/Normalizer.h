#ifndef NORMALIZER_H
#define NORMALIZER_H

#include <QList>
#include <QPair>
#include <QPointF>
#include <QPolygonF>
#include <QtMath>

using PolygonPair = QPair<QPolygonF, QPolygonF>;

class Normalizer {
public:
  // Normalize a single polygon pair based on a maximum coordinate
  static void normalizePolygonPair(PolygonPair& polyPair, qreal normalizeFactor);

  // Normalize a list of polygon pairs based on a maximum coordinate
  static void normalizePolygonPairs(QList<PolygonPair>& polyPairs, qreal normalizeFactor);

  // Calculate the normalization factor based on max coordinate
  static qreal calculateNormalizeFactor(QPointF maxCoord);

  // Normalize to a specific range (e.g., [-1, 1])
  static void normalizeToRange(PolygonPair& polyPair, qreal minRange, qreal maxRange,
                               QPointF maxCoord);

  // Normalize a list of polygon pairs to a specific range
  static void normalizeListToRange(QList<PolygonPair>& polyPairs, qreal minRange, qreal maxRange,
                                   QPointF maxCoord);

private:
  // Private constructor to prevent instantiation (since this is a utility
  // class)
  Normalizer() = default;
};

#endif  // NORMALIZER_H