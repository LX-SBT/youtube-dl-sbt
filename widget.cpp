#include "widget.h"
#include "ui_widget.h"
#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_submit_clicked()
{
    QProcess process;
    QStringList arguments;
    QString output;

    arguments.append(ui->lineEdit_arg->text());
    arguments.append(ui->lineEdit_url->text());
    process.start("youtube-dl",arguments);
    process.waitForFinished(-1);

    output = process.readAllStandardError();
    output += process.readAllStandardOutput();

    foreach(QString line, output.split("\n\r"))
    {
        qDebug()<< line;
        if(line.contains("Destination: "))
        {
            QString command;

            ui->lineEdit_url->clear();
            line = line.mid(line.indexOf("Destination:")+13);
            arguments.clear();
            arguments.append(line);
            process.start(ui->lineEdit_player->text(), arguments);
            process.waitForFinished(-1);
            if(ui->checkBox_delete_after->isChecked())
                QFile::remove(line);
        }
        if(line.contains("ERROR: requested format not available"))
        {
            //ui->lineEdit_arg->setText();
            arguments.clear();
            arguments.append("-F");
            arguments.append(ui->lineEdit_url->text());
            process.start("youtube-dl",arguments);
            process.waitForFinished(-1);
            QMessageBox msg;
            QString msg_text = process.readAllStandardOutput();
            msg_text.replace("audio only","<font color=\"#ba0000\">audio only</font>");
            msg_text.replace("video only","<font color=\"#ba0000\">video only</font>");
            msg_text.replace("(best)","<font color=\"#00ba00\">(best)</font>");
            msg_text = "<pre>" + msg_text + "</pre>";
            msg.setText(msg_text);
            msg.setStyleSheet("QLabel{min-width: 930px;}");
            msg.exec();
        }
    }
}

void Widget::on_lineEdit_url_returnPressed()
{
    on_pushButton_submit_clicked();
}

void Widget::on_lineEdit_arg_returnPressed()
{
    on_pushButton_submit_clicked();
}
