#pragma once

#include <QWidget>
#include <QtNetwork>

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
  void show_connection_status(bool connected);
  void send_message(int cmd, const QString& msg = "");
  void send_heart_beat();

  void on_connected();
  void on_disconnected();
  void handle_connection_error(QAbstractSocket::SocketError e);
  void on_btn_connect_clicked();
  void on_btn_disconnect_clicked();

 private:
  Ui::Widget* ui;

  const QString config_filename{"config.ini"};
  QString uid;
  QString topic;

  QTcpSocket* socket;
  QTimer* heart_beat;

 protected:
  void keyPressEvent(QKeyEvent* event);
  void closeEvent(QCloseEvent* event);
};
