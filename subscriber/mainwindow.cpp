#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>


DDS::DomainId_t domain_id = 42;
const char* EXCHANGE_EVT_TOPIC_NAME = "ExchangeEventTopic";

OpenDDSHelper::CTYPE type ;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    subscriber = new OpenDDSHelper;

    connect(ui->btnConnect,&QPushButton::clicked,this,[this](){

    std::string addr = ui->ip->text().trimmed().toStdString() + ":" + ui->portNumber->text().trimmed().toStdString();

    if(!subscriber->initSubscriber(addr.c_str()
                                  ,type
                                  ,ui->domainId->text().toInt()
                                  ,ui->topicName->text().trimmed().toStdString().c_str() ))
    {
            qDebug()<<"Error:: Failed to initialize subscriber";
            // subscriber->cleanup();
    }
    });

    this->adjustSize();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboTransport_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->ip->setText("localhost");
        ui->portNumber->setText("12345");
        type = OpenDDSHelper::CTYPE::TCP;
        break;
    case 1:
        ui->ip->setText("127.0.0.1");
        ui->portNumber->setText("5000");
        type = OpenDDSHelper::CTYPE::UDP;
        break;
    case 2:
        ui->ip->setText("0.0.0.0");
        ui->portNumber->setText("5000");
        type = OpenDDSHelper::CTYPE::RTPS_UDP;
        break;

    default:
        break;
    }
}

void MainWindow::on_btnDisconnect_clicked()
{

}
