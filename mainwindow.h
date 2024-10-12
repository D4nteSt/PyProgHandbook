#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QListWidget>
#include <QTextBrowser>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QDebug>

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

    void on_NextPageButton_clicked();
    void on_PreviousPageButton_clicked();

    void on_menuExit_triggered();
    void on_menuAbout_triggered();

    void on_OpenBookmarks_clicked();
    void on_BookmarkButton_clicked();

    void on_NavigationItem_Selected(int currentRow);
    void on_BookmarkItem_Selected(int currentRow);

private:
    QListWidget* navigationList;

    QString loadTextFromFile(const QString& filePath);
    bool loadDataFromFile(const QString& fileName);
    bool showingBookmarks = false;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
