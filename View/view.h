#ifndef VIEW_H
#define VIEW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QFileDialog>
#include "openglwidget.h"
#include "ui_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

class OpenGLWidget;

class View : public QMainWindow {
  Q_OBJECT

 public:
  View(QWidget* parent = nullptr);
  ~View();

private slots:
  void on_btnUploadaKMLFile_clicked();

private:
  Ui::View* ui;
  OpenGLWidget* glwidget;

signals:
  /**
   * @brief Signal emitted when a file name is selected by the user
   * @param fileName Path to the selected model file
   */
  void fileNameChoosed(QString fileName);
};
#endif  // VIEW_H
