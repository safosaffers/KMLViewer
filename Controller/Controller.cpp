#include "Controller.h"

Controller::Controller(Model* m, View* v) : QObject(v), model(m), view(v) {
  connect(view, &View::fileNameChoosed, this, &Controller::HandleModelLoading);
}
Controller::~Controller() {}

void Controller::HandleModelLoading(QString fileName) {
  //...todo load model from fileName
}
