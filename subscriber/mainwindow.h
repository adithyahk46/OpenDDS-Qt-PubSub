#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <../OpenDDSHelper.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Subscriber GUI that connects to DDS and displays received messages.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Creates the subscriber main window.
     * @param parent Parent widget.
     */
    MainWindow(QWidget *parent = nullptr);
    /**
     * @brief Destroys the subscriber main window.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Updates transport defaults when the transport combo changes.
     * @param index Selected transport index.
     */
    void on_comboTransport_currentIndexChanged(int index);

    /**
     * @brief Handles subscriber disconnection action from the UI.
     */
    void on_btnDisconnect_clicked();

private:


private:
    Ui::MainWindow *ui;

    OpenDDSHelper* subscriber= nullptr;

};

#endif // MAINWINDOW_H
