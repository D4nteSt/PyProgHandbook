#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QListWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QSplitter>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_BookmarkButton_clicked();

    void on_NextPageButton_clicked();

    void on_PreviousPageButton_clicked();

private:
    QListWidget* navigationList;
    QStackedWidget* contentStack;

    void initializeEmptyItems(int count);
    bool loadDataFromFile(const QString& fileName);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
