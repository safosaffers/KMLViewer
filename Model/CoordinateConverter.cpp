#include "CoordinateConverter.h"
#include <cmath>
#include <QLineF>

const double CoordinateConverter::EQUATORIAL_EARTH_RADIUS_METERS = 6378137;

double CoordinateConverter::degToRad(double deg) { 
    return deg * M_PI / 180.0; 
}

double CoordinateConverter::haversine(double theta) {
    double radiansTheta = degToRad(theta);
    return pow(sin(radiansTheta / 2.0), 2);
}

double CoordinateConverter::latDifferenceInMeters(double lat1, double lat2) {
    double R = EQUATORIAL_EARTH_RADIUS_METERS;
    double deltaLat = fabs(lat2 - lat1);
    double havTheta = haversine(deltaLat);
    double result = 2 * R * asin(sqrt(havTheta));
    return result;
}

double CoordinateConverter::lonDifferenceInMeters(double lat1, double lat2, double lon1, double lon2) {
    double R = EQUATORIAL_EARTH_RADIUS_METERS;
    double deltaLon = fabs(lon2 - lon1);
    double radLat1 = degToRad(lat1);
    double radLat2 = degToRad(lat2);
    double havTheta = cos(radLat1) * cos(radLat2) * haversine(deltaLon);
    double result = 2 * R * asin(sqrt(havTheta));
    return result;
}

PolygonPair CoordinateConverter::convertLatLonToMeters(const PolygonPair& latLonMetPoly, 
                                                     double minLon, double minLat) {
    QPolygonF metersPoly;
    for (const QPointF& lonlat : latLonMetPoly.first) {
        double xLen = lonDifferenceInMeters(minLat, lonlat.y(), minLon, lonlat.x());
        double yLen = latDifferenceInMeters(minLat, lonlat.y());
        metersPoly.append(QPointF(xLen, yLen));
    }
    return qMakePair(latLonMetPoly.first, metersPoly);
}

QList<PolygonPair> CoordinateConverter::convertLatLonToMetersList(const QList<PolygonPair>& latLonList, 
                                                                double minLon, double minLat) {
    QList<PolygonPair> result;
    for (const PolygonPair& latLonPoly : latLonList) {
        result.append(convertLatLonToMeters(latLonPoly, minLon, minLat));
    }
    return result;
}

QPointF CoordinateConverter::getCornerInMeters(double minLon, double maxLon, double minLat, double maxLat) {
    double xLen = lonDifferenceInMeters(minLat, maxLat, minLon, maxLon);
    double yLen = latDifferenceInMeters(minLat, maxLat);
    return QPointF(xLen, yLen);
}

double CoordinateConverter::distanceBetweenLatLonPoints(double lat1, double lon1, double lat2, double lon2) {
    double R = EQUATORIAL_EARTH_RADIUS_METERS;
    double dLat = degToRad(lat2 - lat1);
    double dLon = degToRad(lon2 - lon1);
    
    double a = sin(dLat/2) * sin(dLat/2) +
               cos(degToRad(lat1)) * cos(degToRad(lat2)) *
               sin(dLon/2) * sin(dLon/2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}