#include "view.h"
View::View(QWidget* parent) : QMainWindow(parent), ui(new Ui::View) {
  ui->setupUi(this);
  openglwidget = new OpenGLWidget();
  setCentralWidget(openglwidget);
}

View::~View() { delete ui; }
