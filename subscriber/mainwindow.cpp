#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMetaObject>


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

    if(subscriber->initSubscriber(addr.c_str()
                                  ,type
                                  ,ui->domainId->text().toInt()
                                  ,ui->topicName->text().trimmed().toStdString().c_str() ))
    {
            // subscriber->cleanup(); 
        qDebug()<<"Subscriber initialized successfully";
        MessageReaderListener* listener = subscriber->getMessageReaderListener();
        if (listener) {
            listener->setMessageCallback([this](const Messager::Message& msg) {
                const QString line = QString("Received: id=%1, content=%2, sender=%3")
                                         .arg(msg.id)
                                         .arg(msg.content.in())
                                         .arg(msg.sender.in());
                QMetaObject::invokeMethod(this, [this, line]() {
                    ui->textMessages->append(line);
                }, Qt::QueuedConnection);
            });
        }
    } else {
        qDebug()<<"Error:: Failed to initialize subscriber";
        subscriber->dissConnect();
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
        ui->ip->show();
        ui->portNumber->show();
        break;
    case 1:
        ui->ip->setText("127.0.0.1");
        ui->portNumber->setText("5000");
        type = OpenDDSHelper::CTYPE::UDP;
        ui->ip->show();
        ui->portNumber->show();
        break;
    case 2:
        ui->ip->setText("");
        ui->portNumber->setText("");
        ui->ip->hide();
        ui->portNumber->hide();
        type = OpenDDSHelper::CTYPE::RTPS_UDP;
        break;

    default:
        break;
    }
}

void MainWindow::on_btnDisconnect_clicked()
{
    if(subscriber) subscriber->dissConnect();
}
