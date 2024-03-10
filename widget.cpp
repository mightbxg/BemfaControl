#include "widget.h"

#include <QDateTime>
#include <QSettings>
#include <QtNetwork>

#include "./ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget), socket(new QTcpSocket(this)) {
  ui->setupUi(this);
  connect(socket, &QTcpSocket::errorOccurred, [this](QAbstractSocket::SocketError e) {
    show_message(tr("Connection error"), QVariant::fromValue(e).toString());
  });
  connect(socket, &QTcpSocket::connected, [this] {
    show_message(tr("Connection success"));
    show_connection_status(true);
  });

  load_settings();
  if (uid.isEmpty() || topic.isEmpty()) {
    show_message(tr("Config error"), tr("Please set uid and topic in %1").arg(config_filename));
    save_settings();
  } else {
    show_message(tr("Config load"), tr("The topic is %1").arg(topic));
  }
  show_connection_status(false);
}

Widget::~Widget() { delete ui; }

void Widget::show_message(const QString &title, const QString &msg) {
  auto time = QDateTime::currentDateTime().toString("hh:mm:ss");
  ui->tbr_msg->append(">>> " + time + " " + title);
  if (!msg.isEmpty()) ui->tbr_msg->append(msg);
}

void Widget::load_settings() {
  QSettings settings(config_filename, QSettings::Format::IniFormat);
  uid = settings.value("bemfa/uid", "").toString();
  topic = settings.value("bemfa/topic", "").toString();
}

void Widget::save_settings() {
  QSettings settings(config_filename, QSettings::Format::IniFormat);
  settings.setValue("bemfa/uid", uid);
  settings.setValue("bemfa/topic", topic);
}

void Widget::show_connection_status(bool connected) {
  if (connected) {
    ui->lab_status->setStyleSheet("color:green");
    ui->lab_status->setText("connected");
  } else {
    ui->lab_status->setStyleSheet("color:red");
    ui->lab_status->setText("disconnected");
  }
}

void Widget::on_btn_connect_clicked() {
  socket->abort();
  socket->connectToHost("bemfa.com", 8344);
  ui->btn_connect->setEnabled(false);
  QTimer::singleShot(3000, [&] {
    if (!socket->waitForConnected(0)) {
      show_message(tr("Connection timeout"));
    }
    ui->btn_connect->setEnabled(true);
  });
}
