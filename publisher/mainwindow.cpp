#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    publisher = new OpenDDSPublisher;

    connect(ui->btnConnect,&QPushButton::clicked,this,[this](){

        std::string addr = ui->ip->text().trimmed().toStdString() + ":" + ui->portNumber->text().trimmed().toStdString();


        qDebug()<<addr.c_str();
    if(!publisher->CreatePublisher(addr.c_str()
                                  ,OpenDDSPublisher::CTYPE::UDP
                                  ,ui->domainId->value()
                                  ,ui->topicName->text().trimmed().toStdString().c_str() ))
    {
            qDebug()<<"Error:: Failed to initialize publisher";
            publisher->cleanup();
    }
        qDebug()<<"Success::Publisher Initialized";
    });


}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::InitializePublisher()
{
 return false;
}


void MainWindow::on_sendMessage_clicked()
{

    Messager::Message message;
    message.id =  1;
        message.content = "Hello, OpenDDS!";
        message.sender = "Publisher";
        message.timestamp = get_timestamp();

    if(publisher){
            publisher->publishMessage(message);
        }
}


void MainWindow::on_pushButton_clicked()
{

}

