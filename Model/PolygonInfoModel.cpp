#include "PolygonInfoModel.h"

#include <QStringList>
#include <QVariant>

PolygonInfoModel::PolygonInfoModel(QObject* parent) : QAbstractTableModel(parent) {}

QVariant PolygonInfoModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    QStringList headers = {"ID", "Точек до", "Точек после", "Время (нс)", "Макс. отклонение"};
    if (section < headers.size()) {
      return headers[section];
    }
  }
  return QVariant();
}

int PolygonInfoModel::rowCount(const QModelIndex& parent) const {
  if (parent.isValid())
    return 0;

  return m_polygonInfos.size();
}

int PolygonInfoModel::columnCount(const QModelIndex& parent) const {
  if (parent.isValid())
    return 0;

  return 5;  // ID, pointsBefore, pointsAfter, timeNs, maxDeviation
}

QVariant PolygonInfoModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= m_polygonInfos.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    const PolygonInfo& info = m_polygonInfos[index.row()];

    switch (index.column()) {
      case 0:  // ID
        return info.id;
      case 1:  // Points before
        return info.pointsBefore > 0 ? QString::number(info.pointsBefore) : QString("—");
      case 2:  // Points after
        return info.isSimplified ? QString::number(info.pointsAfter) : QString("—");
      case 3:  // Time
        return info.isSimplified ? QString::number(info.timeNs) + " ns" : QString("—");
      case 4:  // Max deviation
        return info.isSimplified ? QString::number(info.maxDeviation.getValue(), 'f', 4) : QString("—");
      default:
        return QVariant();
    }
  }

  return QVariant();
}

void PolygonInfoModel::setPolygonCount(int count) {
  beginResetModel();
  m_polygonInfos.clear();

  for (int i = 0; i < count; i++) {
    m_polygonInfos.append(PolygonInfo());
  }

  endResetModel();
}

void PolygonInfoModel::setPolygonInfo(int id, const PolygonInfo& info) {
  if (id >= 0 && id < m_polygonInfos.size()) {
    m_polygonInfos[id] = info;
    QModelIndex topLeft = index(id, 0);
    QModelIndex bottomRight = index(id, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);
  }
}

PolygonInfo PolygonInfoModel::getPolygonInfo(int id) const {
  if (id >= 0 && id < m_polygonInfos.size()) {
    return m_polygonInfos[id];
  }
  return PolygonInfo();
}

void PolygonInfoModel::updatePolygonAfterSimplification(int id, int pointsAfter, qint64 timeNs,
                                                        DeviationResult maxDeviation) {
  if (id >= 0 && id < m_polygonInfos.size()) {
    m_polygonInfos[id].pointsAfter = pointsAfter;
    m_polygonInfos[id].timeNs = timeNs;
    m_polygonInfos[id].maxDeviation = maxDeviation;
    m_polygonInfos[id].isSimplified = true;

    QModelIndex topLeft = index(id, 0);
    QModelIndex bottomRight = index(id, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);
  }
}

QList<QLineF> PolygonInfoModel::getAllDerivationsLines(qreal normalize) {
  QList<QLineF> result;
  for (int i = 0; i < m_polygonInfos.size();i++){
    QPointF p1=  m_polygonInfos[i].maxDeviation.getLine().p1();
    QPointF p2=  m_polygonInfos[i].maxDeviation.getLine().p2();
    result.append(QLineF(p1/normalize, p2/normalize));
  }
  return result;
}
