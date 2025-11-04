#ifndef POLYREPR_H
#define POLYREPR_H

#include <QList>
#include <QPair>
#include <QPolygonF>
#include <iterator>

// Represents a polygon in three different representations
using PolyLonLat = QList<QPolygonF>;
using PolyMeters = QList<QPolygonF>;
using PolyNormalize = QList<QPolygonF>;

// A structure to hold all three representations for a single polygon
struct PolygonTriple {
  QPolygonF lonLat;      // Longitude/Latitude representation
  QPolygonF meters;      // Meters representation
  QPolygonF normalized;  // Normalized representation
};

class PolyRepr {
private:
  QList<PolygonTriple> polygons;

public:
  PolyRepr();

  // Getters for each representation
  PolyLonLat getLonLatPolygons() const;
  PolyMeters getMetersPolygons() const;
  PolyNormalize getNormalizedPolygons() const;

  // Setters for each representation
  void setLonLatPolygons(const PolyLonLat& polygons);
  void setMetersPolygons(const PolyMeters& polygons);
  void setNormalizedPolygons(const PolyNormalize& polygons);

  // Direct access to PolygonTriple
  void addPolygon(const PolygonTriple& triple);
  void setPolygon(int index, const PolygonTriple& triple);
  PolygonTriple getPolygon(int index) const;

  // Container operations
  int size() const;
  bool empty() const;
  void clear();

  // Iterator access
  typedef QList<PolygonTriple>::iterator iterator;
  typedef QList<PolygonTriple>::const_iterator const_iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  // Access by representation type
  QPolygonF getLonLatAt(int index) const;
  QPolygonF getMetersAt(int index) const;
  QPolygonF getNormalizedAt(int index) const;

  void setLonLatAt(int index, const QPolygonF& poly);
  void setMetersAt(int index, const QPolygonF& poly);
  void setNormalizedAt(int index, const QPolygonF& poly);

  // Batch operations
  void updateMetersFromLonLat();      // Convert all lonlat to meters
  void updateNormalizedFromMeters();  // Normalize all meter polygons
};

#endif  // POLYREPR_H
