#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../idl/MessagerTypeSupportImpl.h"

#include "../OpenDDSHelper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Publisher GUI that initializes transport and sends DDS messages.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Creates the publisher main window.
     * @param parent Parent widget.
     */
    MainWindow(QWidget *parent = nullptr);
    /**
     * @brief Destroys the main window and releases helper resources.
     */
    ~MainWindow();
private slots:

    /**
     * @brief Updates transport defaults when the transport combo changes.
     * @param index Selected transport index.
     */
    void on_comboTransport_currentIndexChanged(int index);

    /**
     * @brief Disconnects the publisher after user confirmation.
     */
    void on_btnDisconnect_clicked();

    /**
     * @brief Builds and publishes one message sample.
     */
    void on_sendMessage_clicked();

private:
    Ui::MainWindow *ui;
    OpenDDSHelper* publisher = nullptr;
};
#endif // MAINWINDOW_H
