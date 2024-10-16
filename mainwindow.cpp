#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , showingBookmarks(false)

{
    ui->setupUi(this);

    loadBookmarksFromFile();

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

MainWindow::~MainWindow()
{
    saveBookmarksToFile();
    delete ui;
}

//Функция загрузки файла с сохраненными закладками
void MainWindow::saveBookmarksToFile()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/bookmarks.json";

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл для записи:" << file.errorString();
        return;
    }

    QJsonArray jsonArray;
    for (const auto& bookmark : bookmarks)
    {
        QJsonObject obj;
        obj["title"] = bookmark.first;
        obj["filePath"] = bookmark.second;
        jsonArray.append(obj);
    }

    QJsonDocument jsonDoc(jsonArray);
    file.write(jsonDoc.toJson());
    file.close();

    qDebug() << "Закладки успешно сохранены в" << filePath;
}

//Функция загрузки закладок из файла
void MainWindow::loadBookmarksFromFile()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/bookmarks.json";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл для чтения:" << file.errorString();
        return;
    }

    QByteArray fileData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    file.close();

    if (!jsonDoc.isArray())
    {
        qDebug() << "Файл закладок не является JSON массивом";
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();
    bookmarks.clear();

    for (int i = 0; i < jsonArray.size(); ++i)
    {
        QJsonObject obj = jsonArray[i].toObject();
        QString title = obj["title"].toString();
        QString filePath = obj["filePath"].toString();
        bookmarks.append(qMakePair(title, filePath));
    }

    qDebug() << "Закладки успешно загружены из" << filePath;
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

//Функция для вывода списка закладок в навигационное меню
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

//Функция для восстановления всех элементов в навигационном меню при скрытии закладок
void MainWindow::restoreNavigationList()
{
    ui->navigationList->clear();

    loadDataFromFile(":/data.json");
}

//Слот для кнопки открытия списка закладок
void MainWindow::on_OpenBookmarksButton_clicked()
{
    if (showingBookmarks)
    {
        restoreNavigationList();
        showingBookmarks = false;
        ui->OpenBookmarksButton->setText("Показать закладки");
        ui->BookmarkButton->setText("Добавить в закладки");
    }
    else
    {
        showBookmarkList();
        showingBookmarks = true;
        ui->OpenBookmarksButton->setText("Скрыть закладки");
        ui->BookmarkButton->setText("Удалить из закладок");
    }
}

//Слот для кнопки добавления/удаления закладки
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
            int bookmarkIndex = -1;

            for (int i = 0; i < bookmarks.size(); ++i)
            {
                if (bookmarks[i].second == filePath)
                {
                    alreadyBookmarked = true;
                    bookmarkIndex = i;
                    break;
                }
            }

            if (!alreadyBookmarked)
            {
                bookmarks.append(qMakePair(title, filePath));
                qDebug() << "Закладка добавлена: " << title;
            }

            else if (showingBookmarks)
            {
                bookmarks.removeAt(bookmarkIndex);
                delete ui->navigationList->takeItem(currentRow);
                qDebug() << "Закладка удалена: " << title;
            }
        }
    }
}

//Слот для кнопки следующей страницы
void MainWindow::on_NextPageButton_clicked()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow < ui->navigationList->count() - 1)
    {
        ui->navigationList->setCurrentRow(currentRow + 1);
    }
}

//Слот для кнопки предыдущей страницы
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


