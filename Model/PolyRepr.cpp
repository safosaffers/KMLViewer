#include "PolyRepr.h"
#include "CoordinateConverter.h"
#include <limits>

PolyRepr::PolyRepr() {}

// Getters
PolyLonLat PolyRepr::getLonLatPolygons() const {
    PolyLonLat result;
    for (const auto& triple : polygons) {
        result.append(triple.lonLat);
    }
    return result;
}

PolyMeters PolyRepr::getMetersPolygons() const {
    PolyMeters result;
    for (const auto& triple : polygons) {
        result.append(triple.meters);
    }
    return result;
}

PolyNormalize PolyRepr::getNormalizedPolygons() const {
    PolyNormalize result;
    for (const auto& triple : polygons) {
        result.append(triple.normalized);
    }
    return result;
}

// Setters
void PolyRepr::setLonLatPolygons(const PolyLonLat& polys) {
    for (int i = 0; i < polys.size() && i < polygons.size(); ++i) {
        polygons[i].lonLat = polys[i];
    }
    // If there are more new polygons than existing ones, add them
    for (int i = polygons.size(); i < polys.size(); ++i) {
        PolygonTriple triple;
        triple.lonLat = polys[i];
        polygons.append(triple);
    }
}

void PolyRepr::setMetersPolygons(const PolyMeters& polys) {
    for (int i = 0; i < polys.size() && i < polygons.size(); ++i) {
        polygons[i].meters = polys[i];
    }
    // If there are more new polygons than existing ones, add them
    for (int i = polygons.size(); i < polys.size(); ++i) {
        PolygonTriple triple;
        triple.meters = polys[i];
        if (i < polygons.size()) {
            polygons[i].meters = polys[i];
        } else {
            polygons.append(triple);
        }
    }
}

void PolyRepr::setNormalizedPolygons(const PolyNormalize& polys) {
    for (int i = 0; i < polys.size() && i < polygons.size(); ++i) {
        polygons[i].normalized = polys[i];
    }
    // If there are more new polygons than existing ones, add them
    for (int i = polygons.size(); i < polys.size(); ++i) {
        PolygonTriple triple;
        triple.normalized = polys[i];
        if (i < polygons.size()) {
            polygons[i].normalized = polys[i];
        } else {
            polygons.append(triple);
        }
    }
}

// Direct access to PolygonTriple
void PolyRepr::addPolygon(const PolygonTriple& triple) {
    polygons.append(triple);
}

void PolyRepr::setPolygon(int index, const PolygonTriple& triple) {
    if (index >= 0 && index < polygons.size()) {
        polygons[index] = triple;
    }
}

PolygonTriple PolyRepr::getPolygon(int index) const {
    if (index >= 0 && index < polygons.size()) {
        return polygons[index];
    }
    return PolygonTriple(); // Return empty triple if index is out of bounds
}

// Container operations
int PolyRepr::size() const {
    return polygons.size();
}

bool PolyRepr::empty() const {
    return polygons.isEmpty();
}

void PolyRepr::clear() {
    polygons.clear();
}

// Iterator access
PolyRepr::iterator PolyRepr::begin() {
    return polygons.begin();
}

PolyRepr::iterator PolyRepr::end() {
    return polygons.end();
}

PolyRepr::const_iterator PolyRepr::begin() const {
    return polygons.begin();
}

PolyRepr::const_iterator PolyRepr::end() const {
    return polygons.end();
}

PolyRepr::const_iterator PolyRepr::cbegin() const {
    return polygons.cbegin();
}

PolyRepr::const_iterator PolyRepr::cend() const {
    return polygons.cend();
}

// Access by representation type
QPolygonF PolyRepr::getLonLatAt(int index) const {
    if (index >= 0 && index < polygons.size()) {
        return polygons[index].lonLat;
    }
    return QPolygonF();
}

QPolygonF PolyRepr::getMetersAt(int index) const {
    if (index >= 0 && index < polygons.size()) {
        return polygons[index].meters;
    }
    return QPolygonF();
}

QPolygonF PolyRepr::getNormalizedAt(int index) const {
    if (index >= 0 && index < polygons.size()) {
        return polygons[index].normalized;
    }
    return QPolygonF();
}

void PolyRepr::setLonLatAt(int index, const QPolygonF& poly) {
    if (index >= 0 && index < polygons.size()) {
        polygons[index].lonLat = poly;
    }
}

void PolyRepr::setMetersAt(int index, const QPolygonF& poly) {
    if (index >= 0 && index < polygons.size()) {
        polygons[index].meters = poly;
    }
}

void PolyRepr::setNormalizedAt(int index, const QPolygonF& poly) {
    if (index >= 0 && index < polygons.size()) {
        polygons[index].normalized = poly;
    }
}

void PolyRepr::updateMetersFromLonLat() {
    // This method would need to calculate minLon and minLat values
    // For now, we'll skip the conversion if we don't have min values
    // In a real implementation, you'd calculate these values across all polygons first
    if (polygons.isEmpty()) return;
    
    // Find min values across all polygons first
    double minLon = std::numeric_limits<double>::max();
    double minLat = std::numeric_limits<double>::max();
    
    for (const auto& triple : polygons) {
        for (const auto& point : triple.lonLat) {
            minLon = std::min(minLon, point.x());
            minLat = std::min(minLat, point.y());
        }
    }
    
    // Now convert each polygon using the calculated min values
    for (auto& triple : polygons) {
        PolygonPair tempPair = qMakePair(triple.lonLat, QPolygonF());
        PolygonPair convertedPair = CoordinateConverter::convertLatLonToMeters(tempPair, minLon, minLat);
        triple.meters = convertedPair.second;
    }
}

void PolyRepr::updateNormalizedFromMeters() {
    // Find the maximum coordinate to determine normalization factor
    qreal maxCoord = 0;
    for (const auto& triple : polygons) {
        for (const auto& point : triple.meters) {
            maxCoord = qMax(maxCoord, qMax(qAbs(point.x()), qAbs(point.y())));
        }
    }
    
    if (maxCoord > 0) {
        for (auto& triple : polygons) {
            QPolygonF normalizedPoly;
            for (const auto& point : triple.meters) {
                normalizedPoly.append(QPointF(point.x() / maxCoord, point.y() / maxCoord));
            }
            triple.normalized = normalizedPoly;
        }
    }
}
