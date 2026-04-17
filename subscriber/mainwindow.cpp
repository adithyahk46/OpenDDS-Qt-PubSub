#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    subscriber = new OpenDDSSubscriber;

    connect(ui->btnConnect,&QPushButton::clicked,this,[this](){

        std::string addr = ui->ip->text().trimmed().toStdString()
                + ":"
                + ui->portNumber->text().trimmed().toStdString();

    if(!subscriber->createSubscriber(addr.c_str()
                                  ,OpenDDSSubscriber::CTYPE::UDP
                                  ,ui->domainId->text().trimmed().toInt()
                                  ,ui->topicName->text().trimmed().toStdString().c_str() ))
    {
            qDebug()<<"Error:: Failed to initialize publisher";
            // publisher->cleanup();
    }
        qDebug()<<"Success::Publisher Initialized";
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
