#pragma once

#include <QPixmap>
#include <QWidget>

class QPushButton;

class HomeScreenWidget : public QWidget {
  Q_OBJECT

 public:
  explicit HomeScreenWidget(QWidget* parent = nullptr);

 signals:
  void createNewProjectRequested();
  void openProjectRequested();
  void connectRemoteRequested();
  void helpRequested();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  QPushButton* createNewProjectBtn_ = nullptr;
  QPushButton* openProjectBtn_ = nullptr;
  QPushButton* connectRemoteBtn_ = nullptr;
  QPushButton* helpBtn_ = nullptr;
  QPixmap background_;
};