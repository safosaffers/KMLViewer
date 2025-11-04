#include "Model.h"

#include "CoordinateConverter.h"
#include "KmlFileLoader.h"
#include "KmlFileSaver.h"

Model::Model() { currentDocument = NULL; }
Model::~Model() {
  if (currentDocument)
    delete currentDocument;
}

QList<QPolygonF> Model::convertToMeters(QList<QPolygonF> LonLatQList, double& minLon,
                                        double& minLat) {
  QList<QPolygonF> metersPolygons;
  for (const QPolygonF& latLonPoly : LonLatQList) {
    // Create a temporary pair for conversion
    PolygonPair tempPair = qMakePair(latLonPoly, QPolygonF());
    PolygonPair convertedPair =
        CoordinateConverter::convertLatLonToMeters(tempPair, minLon, minLat);
    metersPolygons.append(convertedPair.second);
  }
  return metersPolygons;
}
QPointF Model::getCornerInMeters(double& minLon, double& maxLon, double& minLat, double& maxLat) {
  return CoordinateConverter::getCornerInMeters(minLon, maxLon, minLat, maxLat);
}
void Model::initializeModel(QString filePath) {
  try {
    double minLon = 90.0;
    double maxLon = -90.0;
    double minLat = 180.0;
    double maxLat = -180.0;
    if (currentDocument) {
      delete currentDocument;
      currentDocument = NULL;
    }
    currentDocument = KmlFileLoader::loadKmlFile(filePath);
    if (!currentDocument) {
      throw std::invalid_argument("Failed to open or parse KML file");
    }

    QList<QPolygonF> LonLatQList = KmlFileLoader::parseCoordinatesFromDocument(
        currentDocument, minLon, maxLon, minLat, maxLat);
    QList<QPolygonF> metersPolygons = convertToMeters(LonLatQList, minLon, minLat);

    // Set up the polygon representations
    polygonRepresentations.clear();
    polygonRepresentationsSimplified.clear();
    setLonLatPolygons(LonLatQList);
    setMetersPolygons(metersPolygons);

    // -> top-left corner is just 0,0
    downRightCornerForViewPort = getCornerInMeters(minLon, maxLon, minLat,
                                                   maxLat);  // -> down-right corner
    // Normalize the polygons to [-1, 1] range or similar
    normalizePolygons();
  } catch (const std::invalid_argument& e) {
    qDebug() << "Error: " << e.what();
    throw;
  }
}
PolyRepr Model::getPolygons() { return polygonRepresentations; }
QPointF Model::getNormalizedMaxCoord() { return this->normalizedMaxCoord; }

int Model::getNumberOfPolygons() { return polygonRepresentations.size(); }
int Model::getQListQPolygonFPointsCount(const QList<QPolygonF>& polygons) {
  int result = 0;
  for (const QPolygonF& poly : polygons) {
    result += poly.size();
  }
  return result;
}
int Model::getNumberOfPolygonsPoints() { return getQListQPolygonFPointsCount(getMetersPolygons()); }
int Model::getNumberOfSimplifiedPolygonsPoints() {
  return getQListQPolygonFPointsCount(getSimplifiedNormalizedPolygons());
}

PolygonPair Model::simplifyPolygon(const PolygonPair poly, double epsilon) {
  // Create a temporary pair for the simplification function (to maintain compatibility)
  PolygonPair simplifiedPair = PolygonSimplifier::simplifyPolygon(poly, epsilon);
  return simplifiedPair;  // Return only the simplified meters polygon
}

SimplificationResult Model::simplifyPolygonWithDeviation(const PolygonPair poly, double epsilon) {
  QElapsedTimer timer;
  timer.start();

  PolygonPair simplifiedPair = PolygonSimplifier::simplifyPolygon(poly, epsilon);
  qint64 elapsed = timer.nsecsElapsed();  // Time in nanoseconds

  // Calculate the maximum deviation between original and simplified polygons
  MaxDeviationResult maxDeviation =
      PolygonSimplifier::calculateMaxDeviation(poly, simplifiedPair);

  int originalPoints = poly.second.size();
  int simplifiedPoints = simplifiedPair.second.size();

  // Create and return the full simplification result
  return SimplificationResult(simplifiedPair, elapsed, maxDeviation, originalPoints,
                              simplifiedPoints);
}

QPointF Model::getMaxCoord() const { return normalizedMaxCoord; }

// Access to different polygon representations
QList<QPolygonF> Model::getLonLatPolygons() const {
  return polygonRepresentations.getLonLatPolygons();
}

QList<QPolygonF> Model::getMetersPolygons() const {
  return polygonRepresentations.getMetersPolygons();
}

QList<QPolygonF> Model::getNormalizedPolygons() const {
  return polygonRepresentations.getNormalizedPolygons();
}

void Model::setLonLatPolygons(const QList<QPolygonF>& polys) {
  polygonRepresentations.setLonLatPolygons(polys);
}

void Model::setMetersPolygons(const QList<QPolygonF>& polys) {
  polygonRepresentations.setMetersPolygons(polys);
}

void Model::setNormalizedPolygons(const QList<QPolygonF>& polys) {
  polygonRepresentations.setNormalizedPolygons(polys);
}
void Model::setSimplifiedLonLatPolygons(const QList<QPolygonF>& polys) {
  // This updates the normalized representation with simplified polygons
  polygonRepresentationsSimplified.setLonLatPolygons(polys);
}
void Model::setSimplifiedMetersPolygons(const QList<QPolygonF>& polys) {
  // This updates the normalized representation with simplified polygons
  polygonRepresentationsSimplified.setMetersPolygons(polys);
}
void Model::setSimplifiedNormalizedPolygons(const QList<QPolygonF>& polys) {
  polygonRepresentationsSimplified.setNormalizedPolygons(polys);
}

QList<QPolygonF> Model::getSimplifiedLonLatPolygons() const {
  return polygonRepresentationsSimplified.getLonLatPolygons();
}
QList<QPolygonF> Model::getSimplifiedMetersPolygons() const {
  return polygonRepresentationsSimplified.getMetersPolygons();
}
QList<QPolygonF> Model::getSimplifiedNormalizedPolygons() const {
  return polygonRepresentationsSimplified.getNormalizedPolygons();
}
void Model::normalizePolygons() {
  // Calculate max coordinate for normalization factor
  maxCoord = 0;
  QList<QPolygonF> metersPolygons = getMetersPolygons();
  for (const auto& poly : metersPolygons) {
    for (const auto& point : poly) {
      maxCoord = qMax(maxCoord, qMax(qAbs(point.x()), qAbs(point.y())));
    }
  }

  if (maxCoord > 0) {
    normalizeFactor = maxCoord;
    this->normalizedMaxCoord = QPointF(maxCoord, maxCoord) / normalizeFactor;

    // Normalize the meters polygons
    QList<QPolygonF> normalizedPolygons;
    for (const auto& poly : metersPolygons) {
      QPolygonF normalizedPoly;
      for (const auto& point : poly) {
        normalizedPoly.append(QPointF(point.x() / maxCoord, point.y() / maxCoord));
      }
      normalizedPolygons.append(normalizedPoly);
    }
    setNormalizedPolygons(normalizedPolygons);
  }
}

void Model::normalizeSimplifiedPolygons() {
  // Normalize the simplified polygons based on the same normalization factor
  QList<QPolygonF> simplifiedMetersPolygons = polygonRepresentationsSimplified.getMetersPolygons();
  if (maxCoord > 0) {
    // Normalize the meters polygons
    QList<QPolygonF> normalizedPolygons;
    for (const auto& poly : simplifiedMetersPolygons) {
      QPolygonF normalizedPoly;
      for (const auto& point : poly) {
        normalizedPoly.append(QPointF(point.x() / maxCoord, point.y() / maxCoord));
      }
      normalizedPolygons.append(normalizedPoly);
    }
    setSimplifiedNormalizedPolygons(normalizedPolygons);
  }
}

void Model::saveSimplifiedModel(QString fileName) {
  if (!currentDocument) {
    qWarning() << "No source KML document to preserve structure";
    return;
  }

  QDomDocument doc = currentDocument->cloneNode(true).toDocument();
  // For saving, we need to use the simplified normalized polygons
  QList<PolygonPair> simplifiedPairs;
  QList<QPolygonF> simplifiedLonLatPolygons = getSimplifiedLonLatPolygons();
  QList<QPolygonF> simplifiedNormalized = getSimplifiedNormalizedPolygons();

  // Create PolygonPair list for saving (using original lonLat as the first part and simplified as
  // second)
  for (int i = 0; i < qMin(simplifiedLonLatPolygons.size(), simplifiedNormalized.size()); ++i) {
    simplifiedPairs.append(qMakePair(simplifiedLonLatPolygons[i], simplifiedNormalized[i]));
  }

  KmlFileSaver::updateCoordinatesInDocument(doc, simplifiedPairs);
  KmlFileSaver::saveKmlFile(doc, fileName);
}
