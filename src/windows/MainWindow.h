//
// Created by TheDaChicken on 6/23/2024.
//

#ifndef NRSC5_GUI_SRC_UI_WIDGETS_MAINWINDOW_H_
#define NRSC5_GUI_SRC_UI_WIDGETS_MAINWINDOW_H_

#include <QMainWindow>
#include <QTouchEvent>
#include <QButtonGroup>

#include "pages/RadioDashboard.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  void SwitchToRadioDashboard() const;
  void SwitchToSettingsPage() const;

  [[nodiscard]] RadioDashboard *Dashboard() const;
  [[nodiscard]] QWidget *CurrentPage() const;

 public slots:
  void OnSwitchPage() const;

 private slots:
  void OnPlayButton(bool target) const;

 protected:
  bool event(QEvent *event) override;

 private:
  Ui::MainWindow *ui;
  QButtonGroup button_group_;
};

#endif //NRSC5_GUI_SRC_UI_WIDGETS_MAINWINDOW_H_
