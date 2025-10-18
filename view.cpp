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

void View::on_btnUploadaKMLFile_clicked()
{
  QString filePath = QFileDialog::getOpenFileName(
      nullptr, "Open File", "", "Text files (*.kml);;All files (*)");
  if (!filePath.isEmpty()) {
    // qInfo()<< "model choosed: " << filePath;
    emit fileNameChoosed(filePath);
  }
}

