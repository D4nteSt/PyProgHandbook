#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , showingBookmarks(false)

{
    ui->setupUi(this);
    ui->navigationList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    if (!loadDataFromFile(":/data.json"))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файл с данными");
        return;
    }

    connect(ui->navigationList, &QListWidget::currentRowChanged, this, &MainWindow::onNavigationItemSelected);

    if (ui->navigationList->count() > 0)
    {
        ui->navigationList->setCurrentRow(0);
    }
}
// Функция для загрузки данных из JSON файла
bool MainWindow::loadDataFromFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл:" << file.errorString();
        return false;
    }

    QByteArray fileData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    file.close();

    if (!jsonDoc.isArray())
    {
        qDebug() << "Файл не является JSON массивом";
        return false;

    }

    QJsonArray jsonArray = jsonDoc.array();

    for (int i = 0; i < jsonArray.size(); ++i) {
        QJsonObject obj = jsonArray[i].toObject();
        QString title = obj["title"].toString();
        QString filePath = obj["filePath"].toString();

        QListWidgetItem* item = new QListWidgetItem(title);
        item->setData(Qt::UserRole, filePath);
        ui->navigationList->addItem(item);
    }

    return true;
}

//Слот, вызываемый при выборе элемента из списка
void MainWindow::on_NavigationItem_Selected(int currentRow)
{
    QListWidgetItem* currentItem = ui->navigationList->item(currentRow);
    if (!currentItem)
    {
        return;
    }

    QString filePath = currentItem->data(Qt::UserRole).toString();
    QString fileContent = loadTextFromFile(filePath);

    if (!fileContent.isEmpty())
    {
        ui->textBrowser->setHtml(fileContent);
    }
}

//Функция для загрузки текста из файла
QString MainWindow::loadTextFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл текста:" << file.errorString();
        return "";
    }

    QString fileContent = file.readAll();
    file.close();

    return fileContent;
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_BookmarkButton_clicked()
{

}


void MainWindow::on_NextPageButton_clicked()
{

}


void MainWindow::on_PreviousPageButton_clicked()
{

}


void MainWindow::on_OpenBookmarks_clicked()
{

}

void MainWindow::on_menuExit_triggered()
{
    QMessageBox msgbox;
    msgbox.setText("Вы уверены, что хотите выйти?");
    msgbox.setWindowTitle("Выход");
    msgbox.setIcon(QMessageBox::Warning);
    msgbox.setStyleSheet("background-color: rgb(240, 240, 240);");
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    int answer = msgbox.exec();
        //QMessageBox::warning(this, ("Выход"), ("Вы уверены, что хотите выйти?"), QMessageBox::Yes | QMessageBox::Cancel);

    switch (answer) {
    case QMessageBox::Yes:
        QApplication::exit();
        break;

    case QMessageBox::Cancel:
        break;
    }

}


void MainWindow::on_menuAbout_triggered()
{
    QMessageBox msgbox;
    msgbox.setText("Этот программный продукт представляет из себя справочник по языку программирования Python для начинающих, сделанный в рамках учебной практики.");
    msgbox.setWindowTitle("О программе");
    msgbox.setIcon(QMessageBox::Information);
    msgbox.setStyleSheet("background-color: rgb(240, 240, 240);");
    msgbox.exec();
        //QMessageBox::information(this, ("О программе"), ("Эта программа представляет из себя справочник по языку программирования Python для начинающих, сделанный в рамках учебной практики."));

}



