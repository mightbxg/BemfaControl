#include "widget.h"

#include <QDateTime>
#include <QSettings>

#include "./ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
  load_settings();
  if (uid.isEmpty() || topic.isEmpty()) {
    show_message(tr("Config error"), tr("Please set uid and topic in %1").arg(config_filename));
    save_settings();
  } else {
    show_message(tr("Config load"), tr("The topic is %1").arg(topic));
  }
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

void Widget::on_btn_connect_clicked() {}
