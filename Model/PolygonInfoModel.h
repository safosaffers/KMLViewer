#ifndef POLYGONINFOMODEL_H
#define POLYGONINFOMODEL_H

#include <QAbstractTableModel>
#include <QElapsedTimer>
#include <QPair>
#include <QPolygonF>

struct PolygonInfo {
  int id;
  int pointsBefore;
  int pointsAfter;
  qint64 timeNs;        // Time in milliseconds
  double maxDeviation;  // Maximum deviation after simplification
  bool isSimplified;    // Whether simplification has been performed

  PolygonInfo(int _id = 0, int _pointsBefore = 0, int _pointsAfter = 0, qint64 _timeMs = 0,
              double _maxDeviation = 0.0)
      : id(_id),
        pointsBefore(_pointsBefore),
        pointsAfter(_pointsAfter),
        timeNs(_timeMs),
        maxDeviation(_maxDeviation),
        isSimplified(false) {}
};

class PolygonInfoModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit PolygonInfoModel(QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  // Add/modify polygon info
  void setPolygonCount(int count);
  void setPolygonInfo(int id, const PolygonInfo &info);
  PolygonInfo getPolygonInfo(int id) const;

  // Update specific fields after simplification
  void updatePolygonAfterSimplification(int id, int pointsAfter, qint64 timeNs,
                                        double maxDeviation);

private:
  QList<PolygonInfo> m_polygonInfos;
};

#endif  // POLYGONINFOMODEL_H