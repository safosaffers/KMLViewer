#include "view.h"
View::View(QWidget* parent) : QMainWindow(parent), ui(new Ui::View) {
  ui->setupUi(this);

  QVBoxLayout* layout = new QVBoxLayout(ui->glwidget);
  layout->setContentsMargins(0, 0, 0, 0);
  ui->glwidget->setLayout(layout);
  glwidget = new OpenGLWidget(this);
  layout->addWidget(glwidget);

  // setCentralWidget(openglwidget);
}

View::~View() { delete ui; }
