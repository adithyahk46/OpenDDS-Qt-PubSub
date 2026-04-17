#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <OpenDDSSubscriber.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
private:


private:
    Ui::MainWindow *ui;

    OpenDDSSubscriber* subscriber= nullptr;

};

#endif // MAINWINDOW_H
