#include "Controller.h"

Controller::Controller(Model* m, View* v) : QObject(v), model(m), view(v) {
  connect(view, &View::fileNameChoosed, this, &Controller::HandleModelLoading);
  connect(view, &View::polygonSimplifyRequested, this,
          &Controller::HandleModelSimplify);
}
Controller::~Controller() {}

void Controller::HandleModelLoading(QString fileName) {
  model->initializeModel(fileName);
  view->getGLWidget()->setPolygons(model->getPolygons());
  view->getGLWidget()->updateViewport(model->getDownRightCornerForViewPort());
  view->getGLWidget()->update();
}

void Controller::HandleModelSimplify(double epsilon) {
  model->simplifyPolygons(epsilon);
  view->getGLWidget()->setSimplifiedPolygons(model->getSimplifiedPolygons());
  view->getGLWidget()->update();
}
