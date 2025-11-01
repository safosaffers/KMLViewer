#ifndef COORDINATECONVERTER_H
#define COORDINATECONVERTER_H

#include <QPointF>
#include <QPolygonF>
#include <QList>
#include <utility> // for std::pair

using PolygonPair = QPair<QPolygonF, QPolygonF>;

class CoordinateConverter {
public:
    static const double EQUATORIAL_EARTH_RADIUS_METERS;
    
    // Convert latitude/longitude to meters
    static PolygonPair convertLatLonToMeters(const PolygonPair& latLonMetPoly, 
                                           double minLon, double minLat);
    static QList<PolygonPair> convertLatLonToMetersList(const QList<PolygonPair>& latLonList, 
                                                      double minLon, double minLat);
    static QPointF getCornerInMeters(double minLon, double maxLon, double minLat, double maxLat);
    
    // Distance calculations
    static double degToRad(double deg);
    static double haversine(double theta);
    static double latDifferenceInMeters(double lat1, double lat2);
    static double lonDifferenceInMeters(double lat1, double lat2, double lon1, double lon2);
    static double distanceBetweenLatLonPoints(double lat1, double lon1, double lat2, double lon2);
};

#endif // COORDINATECONVERTER_H