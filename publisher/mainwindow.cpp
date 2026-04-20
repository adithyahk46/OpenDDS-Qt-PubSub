#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
OpenDDSHelper::CTYPE type ;


#include <orbsvcs/Time_Utilities.h>

DDS::DomainId_t domain_id = 42;
const char* EXCHANGE_EVT_TOPIC_NAME = "ExchangeEventTopic";

// Helper to get current timestamp
TimeBase::TimeT get_timestamp() {
    TimeBase::TimeT retval;
    ACE_hrtime_t t = ACE_OS::gethrtime();
    ORBSVCS_Time::hrtime_to_TimeT(retval, t);
    return retval;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    type = OpenDDSHelper::CTYPE::TCP;

    publisher = new OpenDDSHelper;

    connect(ui->btnConnect,&QPushButton::clicked,this,[this](){

    std::string addr = ui->ip->text().trimmed().toStdString() + ":" + ui->portNumber->text().trimmed().toStdString();

        if(!publisher->initPublisher(addr.c_str()
                      ,type
                      ,ui->domainId->value()
                      ,ui->topicName->text().trimmed().toStdString().c_str() ))
    {
            qDebug()<<"Error:: Failed to initialize publisher";
            return;
        }

        // if (!publisher->startPublisher()) {
        //     qDebug() << "Error:: Failed to start publisher";
    // }
    });

    this->adjustSize();

}

MainWindow::~MainWindow()
{
    if (publisher) {
        // publisher->finalShutdown();
        delete publisher;
        publisher = nullptr;
    }
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
        ui->ip->setText("0.0.0.0");
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
    //show message box to confirm disconnection with Yes/No options and config details
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Disconnection", "Are you sure you want to disconnect the publisher?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        qDebug() << "Publisher disconnected.";

        if(publisher){
            // publisher->disconnectPublisher();
            publisher->cleanup();
        }

    } else {
        qDebug() << "Disconnection cancelled.";
    }


}


void MainWindow::on_sendMessage_clicked()
{
    Messager::Message message;
        message.id =  1;
            message.content = "Hello, OpenDDS!";
            message.sender = "Publisher";
            message.timestamp = get_timestamp();

    if(publisher->publishMessage(message)){
                std::cout << "INFO: Message written successfully." << std::endl;
    } else {
                std::cerr << "ERROR: Failed to write message." << std::endl;
    }
}

