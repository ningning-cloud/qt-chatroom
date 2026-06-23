#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QTcpSocket>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_connectButton_clicked();

    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;

    QString name;
};
#endif // MAINWINDOW_H
