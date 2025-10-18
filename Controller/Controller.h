#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "../Model/Model.h"
#include "../View/view.h"

class Controller : public QObject {
  Q_OBJECT

 private:
  Model* model;
  View* view;

 public:
  Controller(Model* model, View* view);
  ~Controller();
 public slots:
  void HandleModelLoading(QString fileName);
};

#endif  // CONTROLLER_H
