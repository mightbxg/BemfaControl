#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
  Q_OBJECT

 public:
  Widget(QWidget* parent = nullptr);
  ~Widget();

 public slots:
  void show_message(const QString& title, const QString& msg = "");

 private slots:
  void load_settings();
  void save_settings();
  void on_btn_connect_clicked();

 private:
  Ui::Widget* ui;

  const QString config_filename{"config.ini"};
  QString uid;
  QString topic;
};
