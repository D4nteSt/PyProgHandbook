#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , showingBookmarks(false)

{
    ui->setupUi(this);
    //ui->navigationList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    if (!loadDataFromFile(":/data.json"))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файл с данными");
        return;
    }

   // connect(ui->OpenBookmarksButton, &QPushButton, this, &MainWindow::on_OpenBookmarks_clicked);
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
void MainWindow::onNavigationItemSelected(int currentRow)
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

void MainWindow::showBookmarkList()
{
    ui->navigationList->clear();

    for (const auto& bookmark : bookmarks)
    {
        QListWidgetItem* item = new QListWidgetItem(bookmark.first);
        item->setData(Qt::UserRole, bookmark.second);
        ui->navigationList->addItem(item);
    }
}

void MainWindow::restoreNavigationList()
{
    ui->navigationList->clear();

    loadDataFromFile(":/data.json");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OpenBookmarksButton_clicked()
{
    if (showingBookmarks)
    {
        restoreNavigationList();
        showingBookmarks = false;
        ui->OpenBookmarksButton->setText("Показать закладки");
    }
    else
    {
        showBookmarkList();
        showingBookmarks = true;
        ui->OpenBookmarksButton->setText("Скрыть закладки");
    }
}


void MainWindow::on_BookmarkButton_clicked()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow >= 0)
    {
        QListWidgetItem* currentItem = ui->navigationList->item(currentRow);
        if (currentItem)
        {
            QString title = currentItem->text();
            QString filePath = currentItem->data(Qt::UserRole).toString();

            bool alreadyBookmarked = false;
            for (const auto& bookmark : bookmarks)
            {
                if (bookmark.second == filePath)
                {
                    alreadyBookmarked = true;
                    break;
                }
            }

            if (!alreadyBookmarked)
            {
                bookmarks.append(qMakePair(title, filePath));
                qDebug() << "Закладка добавлена: " << title;
            }
        }
    }
}


void MainWindow::on_NextPageButton_clicked()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow < ui->navigationList->count() - 1)
    {
        ui->navigationList->setCurrentRow(currentRow + 1);
    }
}


void MainWindow::on_PreviousPageButton_clicked()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow > 0)
    {
        ui->navigationList->setCurrentRow(currentRow - 1);
    }
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
}


