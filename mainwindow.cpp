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
    connect(ui->navigationList, &QListWidget::currentRowChanged, this, &MainWindow::updateNavigationButtons);

    if (ui->navigationList->count() > 0)
    {
        ui->navigationList->setCurrentRow(0);
        updateNavigationButtons();
        updateBookmarkButton();
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

//Функция для загрузки содержимого css файла
QString MainWindow::loadStyleSheetFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть CSS файл: " << file.errorString();
        return "";
    }

    QString cssContent = file.readAll();
    file.close();

    return cssContent;
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
    QString cssContent = loadStyleSheetFromFile(":/style.css");

    if (!fileContent.isEmpty())
    {
        ui->textBrowser->setHtml(fileContent);
        ui->textBrowser->setStyleSheet(cssContent);
    }

    updateNavigationButtons();
    updateBookmarkButton();
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

    if (ui->navigationList->count() > 0)
    {
        ui->navigationList->setCurrentRow(0);
    }

    if (bookmarks.isEmpty())
    {
        ui->textBrowser->setText("Пока закладок нет...");
        return;
    }
}

//Функция для восстановления всех элементов в навигационном меню при скрытии закладок
void MainWindow::restoreNavigationList()
{
    ui->navigationList->clear();

    loadDataFromFile(":/data.json");

    if (ui->navigationList->count() > 0)
    {
        ui->navigationList->setCurrentRow(0);
    }
}

//Слот для кнопки открытия/скрытия списка закладок
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
    updateBookmarkButton();
    updateNavigationButtons();
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

                QMessageBox msgbox;
                msgbox.setText("Закладка успешно добавлена!");
                msgbox.setWindowTitle("Внимание!");
                msgbox.setIcon(QMessageBox::Information);
                msgbox.setStyleSheet("background-color: rgb(240, 240, 240);");
                msgbox.setStandardButtons(QMessageBox::Ok);
                msgbox.exec();

                updateBookmarkButton();
            }

            else if (showingBookmarks)
            {
                bookmarks.removeAt(bookmarkIndex);
                delete ui->navigationList->takeItem(currentRow);
                qDebug() << "Закладка удалена: " << title;

                if (bookmarks.isEmpty())
                {
                    ui->textBrowser->setText("Пока закладок нет...");
                }

                QMessageBox msgbox;
                msgbox.setText("Закладка успешно удалена!");
                msgbox.setWindowTitle("Внимание!");
                msgbox.setIcon(QMessageBox::Information);
                msgbox.setStyleSheet("background-color: rgb(240, 240, 240);");
                msgbox.setStandardButtons(QMessageBox::Ok);
                msgbox.exec();

                updateBookmarkButton();
            }
        }
    }
}

//Функция обновления внешнего вида кнопок добавления/удаления закладок
void MainWindow::updateBookmarkButton()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow < 0) {
        return;
    }

    QListWidgetItem* currentItem = ui->navigationList->item(currentRow);
    if (!currentItem) {
        return;
    }

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

    if (alreadyBookmarked && showingBookmarks)
    {
        ui->BookmarkButton->setText("Удалить из закладок");
        ui->BookmarkButton->setEnabled(true);
        ui->BookmarkButton->setStyleSheet("background-color: rgb(72, 61, 139); border-radius: 10px; border: 1px solid transparent; color: #FFFFFF; font-family: \"Inter var\",ui-sans-serif,system-ui,-apple-system,system-ui,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,\"Noto Sans\",sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\",\"Noto Color Emoji\";");
    }
    else if (alreadyBookmarked && !showingBookmarks)
    {
        ui->BookmarkButton->setText("Добавить в закладки");
        ui->BookmarkButton->setEnabled(false);
        ui->BookmarkButton->setStyleSheet("background-color: #231e44; color: #a9a9a9; border-radius: 10px;");
    }
    else if (!alreadyBookmarked)
    {
        ui->BookmarkButton->setText("Добавить в закладки");
        ui->BookmarkButton->setEnabled(true);
        ui->BookmarkButton->setStyleSheet("background-color: rgb(72, 61, 139); border-radius: 10px; border: 1px solid transparent; color: #FFFFFF; font-family: \"Inter var\",ui-sans-serif,system-ui,-apple-system,system-ui,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,\"Noto Sans\",sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\",\"Noto Color Emoji\";");
    }
}

//Функция обновления внешнего вида навигационных кнопок
void MainWindow::updateNavigationButtons()
{
    if (ui->navigationList->currentRow() > 0)
    {
        ui->PreviousPageButton->setEnabled(true);
        ui->PreviousPageButton->setStyleSheet("background-color: rgb(125, 113, 216); border: 1px solid transparent; color: #FFFFFF; font-family: \"Inter var\",ui-sans-serif,system-ui,-apple-system,system-ui,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,\"Noto Sans\",sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\",\"Noto Color Emoji\";");
    }
    else
    {
        ui->PreviousPageButton->setEnabled(false);
        ui->PreviousPageButton->setStyleSheet("background-color: #231b62; color: #a9a9a9;");
    }

    if (ui->navigationList->currentRow() < ui->navigationList->count() - 1)
    {
        ui->NextPageButton->setEnabled(true);
        ui->NextPageButton->setStyleSheet("background-color: rgb(125, 113, 216); border: 1px solid transparent; color: #FFFFFF; font-family: \"Inter var\",ui-sans-serif,system-ui,-apple-system,system-ui,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,\"Noto Sans\",sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\",\"Noto Color Emoji\";");
    }
    else
    {
        ui->NextPageButton->setEnabled(false);
        ui->NextPageButton->setStyleSheet("background-color: #231b62; color: #a9a9a9;");
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

    updateNavigationButtons();
}

//Слот для кнопки предыдущей страницы
void MainWindow::on_PreviousPageButton_clicked()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow > 0)
    {
        ui->navigationList->setCurrentRow(currentRow - 1);
    }

    updateNavigationButtons();
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


