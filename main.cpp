#include <QApplication>

#include "Controller/Controller.h"
#include "Model/Model.h"
#include "View/view.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);

  Model* model = new Model();
  View* view = new View();
  new Controller(model, view);

  view->show();
  return a.exec();
}
