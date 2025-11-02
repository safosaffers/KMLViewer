#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QtConcurrent>
#include <stdexcept>
#include <QElapsedTimer>
#include <QPair>

#include "../Model/Model.h"
#include "../View/view.h"
#include "../Model/PolygonInfoModel.h"

class Controller : public QObject {
  Q_OBJECT

 private:
  Model* model;
  View* view;
  QElapsedTimer timer;
  QFutureWatcher<QPair<PolygonPair, qint64>> watcher;  // Pair of simplified polygon and time
  PolygonInfoModel* polygonInfoModel;

 public:
  Controller(Model* model, View* view);
  ~Controller();

 public slots:
  void HandleModelLoading(QString fileName);
  void HandleModelSimplify(double epsilon);
  void finishModelSimplify();
  void HandleModelSimplifySave(QString fileName);
};

#endif  // CONTROLLER_H
