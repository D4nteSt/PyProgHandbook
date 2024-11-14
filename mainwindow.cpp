/**
 * @file mainwindow.cpp
 * @brief Реализация главного окна приложения.
 *
 * Этот файл содержит реализацию всех функций, относящихся к главному окну приложения, включая слоты для
 * обработки действий пользователя, такие как добавление/удаление закладок, навигация по страницам,
 * отображение информации о программе, выход из приложения и другие. Внутри используются механизмы Qt,
 * такие как списки, кнопки и окна сообщений, для реализации интерфейса и взаимодействия с пользователем.
 *
 * Программа представляет собой справочник по языку программирования Python и позволяет пользователю
 * легко переключаться между разделами, добавлять закладки на важные страницы.
 *
 * Основные функции:
 * - Обработка кнопок навигации (следующая и предыдущая страницы).
 * - Добавление и удаление закладок.
 * - Показ и скрытие списка закладок.
 * - Управление доступностью кнопок в зависимости от текущего состояния.
 *
 * Применяются стили для элементов управления, чтобы улучшить внешний вид приложения.
 */

#include "mainwindow.h"
#include "./ui_mainwindow.h"

/**
 * @brief Конструктор класса MainWindow.
 *
 * Инициализирует основное окно приложения, загружает данные и закладки, а также настраивает соединения для сигналов и слотов.
 *
 * @param parent Родительский объект для QMainWindow.
 */

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
        updateOpenBookmarksButton();
        updateNavigationButtons();
        updateBookmarkButton();
    }

}

/**
 * @brief Деконструктор класса MainWindow.
 *
 * Сохраняет закладки в файл перед завершением работы приложения
 */
MainWindow::~MainWindow()
{
    saveBookmarksToFile();
    delete ui;
}

/**
 * @brief Сохраняет закладки в файл.
 *
 * Закладки сохрняются в файл bookmark.json в формате JSON, который содержит значения "title" (название раздела) и "filePath" (путь до HTML файла).
 */
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

/**
 * @brief Загружает закладки из файла bookmarks.json.
 *
 * Закладки считываются из файла bookmarks.json и добавляются в вектор пар bookmarks.
 */
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

/**
 * @brief Загружает данные из JSON файла.
 *
 * Функция загружает данные из указанного JSON файла и добавляет элементы в список навигации.
 * @param fileName Имя JSON файла. В нашем случае data.json, который содержит значения "title" (название раздела) и "filePath" (путь до HTML файла).
 * @return Возвращает true при успешной загрузке файла, иначе false.
 */
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

/**
 * @brief Загружает содержимое CSS файла.
 *
 * Функция читает и возвращает содержимое CSS файла по указанному пути.
 *
 * @param filePath Путь к CSS файлу.
 * @return Содержимое CSS файла в виде строки.
 */
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

/**
 * @brief Обработчик выбора элемента в списке навигации.
 *
 * Загружает содержимое выбранного файла и применяет к нему CSS стили.
 *
 * @param currentRow Индекс выбранного элемента в списке.
 */
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

/**
 * @brief Загружает текст из файла.
 *
 * Функция читает и возвращает содержимое из файла, в нашем случае HTML, по указанному пути.
 *
 * @param filePath Путь к файлу.
 * @return Содержимое файла в виде строки.
 */
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

/**
 * @brief Отображает список закаладок в навигационном меню.
 *
 * Добавляет закладки в список навигации, заменяя все существующие разделы.
 */
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

    updateOpenBookmarksButton();
}

/**
 * @brief Восстанавливает элементы навигационного списка после скрытия закладок.
 *
 * Загружает данные из основного JSON файла (data.json) и восстанавливает навигациионный список со всеми разделами.
 */
void MainWindow::restoreNavigationList()
{
    ui->navigationList->clear();

    loadDataFromFile(":/data.json");

    if (ui->navigationList->count() > 0)
    {
        ui->navigationList->setCurrentRow(0);
    }
}

/**
 * @brief Слот для обработки показа или скрытия списка закладок.
 *
 * Меняет булевое значение, отвечающее за флаг открыты ли закладки или нет.
 *
 * Открывает список закладок/восстанавливает список всех разделов.
 * Меняет текст кнопки на "Показать закладки" если они скрыты, в ином случае "Скрыть закладки".
 * Вызываются фукнции обновления навигационных кнопок и добавления/скрытия закладок.
 */
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

/**
 * @brief Слот для обработки добавления или удаления закладки.
 *
 * Эта функция выполняет добавление текущего элемента в список закладок или удаление,
 * если элемент уже находится в закладках. Она проверяет, существует ли закладка для текущего
 * элемента, и если её нет, добавляет её. В противном случае, если закладка отображается
 * в списке закладок, она удаляется.
 *
 * Если закладка добавлена, появляется подтверждение и уведомление об успешном добавлении, если выбрано "Да", иначе программа закрывает диалоговое окно.
 * Если закладка удалена, также появляется подтверждение и сообщение об успешном удалении, если выбрано "Да", иначе программа так же закрывает окно с диалогом.
 *
 * После добавления или удаления закладки функция обновляет интерфейс, включая состояние
 * кнопок закладок и навигации, а также текст кнопки отображения списка закладок.
 */
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
                QMessageBox confirmBox;
                confirmBox.setWindowTitle("Добавить закладку");
                confirmBox.setText("Вы уверены, что хотите добавить эту страницу в закладки?");
                confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                confirmBox.setButtonText(QMessageBox::Yes, "Да");
                confirmBox.setButtonText(QMessageBox::No, "Нет");
                confirmBox.setIcon(QMessageBox::Question);

                if (confirmBox.exec() == QMessageBox::Yes)
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
            }

            else if (showingBookmarks)
            {
                    QMessageBox confirmBox;
                    confirmBox.setWindowTitle("Удалить закладку");
                    confirmBox.setText("Вы уверены, что хотите удалить эту страницу из закладок?");
                    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    confirmBox.setButtonText(QMessageBox::Yes, "Да");
                    confirmBox.setButtonText(QMessageBox::No, "Нет");
                    confirmBox.setIcon(QMessageBox::Question);

                    if (confirmBox.exec() == QMessageBox::Yes)
                    {
                        bookmarks.removeAt(bookmarkIndex);
                        delete ui->navigationList->takeItem(currentRow);
                        qDebug() << "Закладка удалена: " << title;

                        if (bookmarks.isEmpty())
                        {
                            restoreNavigationList();
                            showingBookmarks = false;
                            ui->OpenBookmarksButton->setText("Показать закладки");
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

            updateOpenBookmarksButton();
            updateNavigationButtons();
        }
    }
}

/**
 * @brief Обновляет состояние кнопки "Открыть/Скрыть закладки"
 *
 * Эта функция изменяет состояние кнопки "Открыть/Скрыть закладки" в зависимости от наличия закладок.
 * Если список закладок пуст, кнопка отключается и ей назначается стиль для неактивного состояния.
 * В противном случае кнопка становится активной и ей назначается стиль для активного состояния.
 *
 */
void MainWindow::updateOpenBookmarksButton()
{
    QString bookmarksButtonStyle = "background-color: rgb(72, 61, 139); border-radius: 10px; border: 1px solid transparent; color: #FFFFFF; font-family: \"Inter var\",ui-sans-serif,system-ui,-apple-system,system-ui,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,\"Noto Sans\",sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\",\"Noto Color Emoji\";";
    QString bookmarksButtonDisabledStyle = "background-color: #231b62; color: #a9a9a9; border-radius: 10px;";

    if (bookmarks.isEmpty())
    {
        ui->OpenBookmarksButton->setEnabled(false);
        ui->OpenBookmarksButton->setStyleSheet(bookmarksButtonDisabledStyle);
    }
    else
    {
        ui->OpenBookmarksButton->setEnabled(true);
        ui->OpenBookmarksButton->setStyleSheet(bookmarksButtonStyle);
    }
}

/**
 * @brief Обновляет внешний вид и состояние кнопки "Добавить/Удалить закладку".
 *
 * Функция обновляет текст и доступность кнопки "Добавить/Удалить закладку" в зависимости от
 * текущего элемента списка навигации и наличия этого элемента в списке закладок.
 *
 * Если текущий элемент уже добавлен в закладки и список закладок отображается,
 * то кнопка предлагает удалить этот элемент из закладок.
 * Если элемент уже добавлен в закладки, но список закладок не отображается,
 * кнопка становится неактивной.
 *
 * Если элемент не добавлен в закладки, кнопка становится активной и предлагает добавить его в закладки.
 *
 */
void MainWindow::updateBookmarkButton()
{
    QString bookmarksButtonStyle = "background-color: rgb(72, 61, 139); border-radius: 10px; border: 1px solid transparent; color: #FFFFFF; font-family: \"Inter var\",ui-sans-serif,system-ui,-apple-system,system-ui,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,\"Noto Sans\",sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\",\"Noto Color Emoji\";";
    QString bookmarksButtonDisabledStyle = "background-color: #231b62; color: #a9a9a9; border-radius: 10px;";

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
        ui->BookmarkButton->setStyleSheet(bookmarksButtonStyle);
    }
    else if (alreadyBookmarked && !showingBookmarks)
    {
        ui->BookmarkButton->setText("Добавить в закладки");
        ui->BookmarkButton->setEnabled(false);
        ui->BookmarkButton->setStyleSheet(bookmarksButtonDisabledStyle);
    }
    else if (!alreadyBookmarked)
    {
        ui->BookmarkButton->setText("Добавить в закладки");
        ui->BookmarkButton->setEnabled(true);
        ui->BookmarkButton->setStyleSheet(bookmarksButtonStyle);
    }
}

/**
 * @brief Обновляет внешний вид и состояние кнопок навигации ("Предыдущая страница" и "Следующая страница").
 *
 * Функция проверяет, есть ли предыдущие и последующие элементы в списке навигации (ui->navigationList),
 * и на основе этого активирует или деактивирует кнопки навигации.
 *
 * Если текущий элемент не первый в списке, кнопка "Предыдущая страница" становится активной и применяет
 * стиль для активной кнопки. Если элемент первый, кнопка деактивируется и применяет стиль для неактивной кнопки.
 *
 * Аналогично, если текущий элемент не последний в списке, кнопка "Следующая страница" активируется,
 * в противном случае она становится неактивной.
 *
 */
void MainWindow::updateNavigationButtons()
{
    QString navigationButtonsStyle = "background-color: rgb(125, 113, 216); border: 1px solid transparent; color: #FFFFFF; font-family: \"Inter var\",ui-sans-serif,system-ui,-apple-system,system-ui,\"Segoe UI\",Roboto,\"Helvetica Neue\",Arial,\"Noto Sans\",sans-serif,\"Apple Color Emoji\",\"Segoe UI Emoji\",\"Segoe UI Symbol\",\"Noto Color Emoji\";";
    QString navigationButtonsDisabledStyle = "background-color: #231e44; color: #a9a9a9; border-radius: 10px;";

    if (ui->navigationList->currentRow() > 0)
    {
        ui->PreviousPageButton->setEnabled(true);
        ui->PreviousPageButton->setStyleSheet(navigationButtonsStyle);
    }
    else
    {
        ui->PreviousPageButton->setEnabled(false);
        ui->PreviousPageButton->setStyleSheet(navigationButtonsDisabledStyle);
    }

    if (ui->navigationList->currentRow() < ui->navigationList->count() - 1)
    {
        ui->NextPageButton->setEnabled(true);
        ui->NextPageButton->setStyleSheet(navigationButtonsStyle);
    }
    else
    {
        ui->NextPageButton->setEnabled(false);
        ui->NextPageButton->setStyleSheet(navigationButtonsDisabledStyle);
    }
}

/**
 * @brief Слот, вызываемый при нажатии кнопки "Следующая страница".
 *
 * Функция перемещает выделение в списке (ui->navigationList) на следующий элемент,
 * если текущий элемент не является последним. После этого обновляется состояние
 * кнопок навигации для синхронизации их активности (вызов updateNavigationButtons()).
 */
void MainWindow::on_NextPageButton_clicked()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow < ui->navigationList->count() - 1)
    {
        ui->navigationList->setCurrentRow(currentRow + 1);
    }

    updateNavigationButtons();
}

/**
 * @brief Слот, вызываемый при нажатии кнопки "Предыдущая страница".
 *
 * Функция перемещает выделение в списке (ui->navigationList) на предыдущий элемент,
 * если текущий элемент не является первым. После этого обновляется состояние
 * кнопок навигации для синхронизации их активности (вызов updateNavigationButtons()).
 */
void MainWindow::on_PreviousPageButton_clicked()
{
    int currentRow = ui->navigationList->currentRow();
    if (currentRow > 0)
    {
        ui->navigationList->setCurrentRow(currentRow - 1);
    }

    updateNavigationButtons();
}

/**
 * @brief Слот подтверждения о выходе.
 *
 * Функция показывает диалоговое окно с подтверждением выхода из программы. Если пользователь
 * выбирает "Да", программа завершает работу. Если выбрано "Отмена",
 * окно закрывается без выхода.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Выход");
    msgBox.setText("Вы уверены, что хотите выйти?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setButtonText(QMessageBox::Yes, "Да");
    msgBox.setButtonText(QMessageBox::No, "Нет");

    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

/**
 * @brief Слот для пункта меню "О программе".
 *
 * Показывает диалоговое окно с информацией о программе.
 */
void MainWindow::on_menuAbout_triggered()
{
    QMessageBox msgbox;
    msgbox.setText("Этот программный продукт представляет из себя справочник по языку программирования Python для начинающих, сделанный в рамках учебной практики.");
    msgbox.setWindowTitle("О программе");
    msgbox.setIcon(QMessageBox::Information);
    msgbox.setStyleSheet("background-color: rgb(240, 240, 240);");
    msgbox.exec();       
}


