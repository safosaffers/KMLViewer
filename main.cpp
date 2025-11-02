#include <QApplication>

#include "Controller/Controller.h"
#include "Model/Model.h"
#include "View/view.h"

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  new Controller(new Model(), new View());
  return a.exec();
}
