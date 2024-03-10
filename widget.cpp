#include "widget.h"

#include <QDateTime>
#include <QKeyEvent>
#include <QSettings>

#include "./ui_widget.h"

namespace {
QString uid_removed(const QString &str) {
  auto p0 = str.indexOf("uid=");
  if (p0 < 0) return str;
  auto p1 = str.indexOf('&', p0);
  if (p1 < 0) p1 = str.size();
  auto ret = str;
  ret.replace(p0, p1 - p0, "uid=***");
  ret.remove("\r\n");
  return ret;
}
}  // namespace

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget), socket(new QTcpSocket(this)), heart_beat(new QTimer(this)) {
  ui->setupUi(this);
  connect(socket, &QTcpSocket::connected, this, &Widget::on_connected);
  connect(socket, &QTcpSocket::disconnected, this, &Widget::on_disconnected);
  connect(socket, &QTcpSocket::errorOccurred, this, &Widget::handle_connection_error);
  connect(socket, &QTcpSocket::readyRead, this, &Widget::receive_message);

  heart_beat->setInterval(30000);
  connect(heart_beat, &QTimer::timeout, this, &Widget::send_heart_beat);
  heart_beat->start();

  load_settings();
  if (uid.isEmpty() || topic.isEmpty()) {
    show_message(tr("Config error"), tr("Please set uid and topic in %1").arg(config_filename));
    save_settings();
    ui->btn_connect->setEnabled(false);
  } else {
    show_message(tr("Config load"), tr("The topic is %1").arg(topic));
    ui->btn_connect->setEnabled(true);
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

void Widget::send_message(int cmd, const QString &msg) {
  if (socket->state() != QAbstractSocket::ConnectedState) return;
  QString str = QString("cmd=%1&uid=%2&topic=%3").arg(cmd).arg(uid).arg(topic);
  if (!msg.isEmpty()) str += "&" + msg;
  str += "\r\n";
  socket->write(str.toStdString().c_str());
  show_message(tr("Send message"), uid_removed(str));
}

void Widget::send_heart_beat() {
  if (socket->state() != QAbstractSocket::ConnectedState) return;
  socket->write("ping\r\n");
}

void Widget::receive_message() {
  QString msg = QString::fromUtf8(socket->readAll());
  msg = uid_removed(msg);
  if (msg.endsWith('\n')) msg.removeLast();
  if (msg.endsWith('\r')) msg.removeLast();
  show_message(tr("Received message"), msg);
}

void Widget::on_connected() {
  show_message(tr("Connection success"));
  show_connection_status(true);
  send_message(1);
  ui->btn_connect->setEnabled(true);
}

void Widget::on_disconnected() {
  show_message(tr("Disconnected"));
  show_connection_status(false);
}

void Widget::handle_connection_error(QAbstractSocket::SocketError e) {
  show_message(tr("Connection error"), QVariant::fromValue(e).toString());
  show_connection_status(false);
}

void Widget::on_btn_connect_clicked() {
  socket->abort();
  socket->connectToHost("bemfa.com", 8344);
  ui->btn_connect->setEnabled(false);
  QTimer::singleShot(3000, [&] {
    if (ui->btn_connect->isEnabled()) return;
    show_message(tr("Connection timeout"));
    ui->btn_connect->setEnabled(true);
  });
}

void Widget::on_btn_disconnect_clicked() { socket->disconnectFromHost(); }

void Widget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_D && event->modifiers() == (Qt::ControlModifier | Qt::AltModifier)) {
    show_message(tr("Debug"), tr("socket[%1]").arg(socket->state()));
  }
}

void Widget::closeEvent(QCloseEvent *event) { socket->disconnectFromHost(); }
