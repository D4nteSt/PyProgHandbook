#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    ui->navigationList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    // Инициализируем интерфейс с пустыми элементами
    initializeEmptyItems(3);  // Предполагаем 3 элемента, можно динамически задавать

    // Загружаем данные из JSON файла
    if (!loadDataFromFile(":/data.json")) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить файл с данными.");
        return;
    }

    // Связываем выбор в списке с отображением контента
    connect(ui->navigationList, &QListWidget::currentRowChanged, ui->contentStack, &QStackedWidget::setCurrentIndex);

    // Устанавливаем начальное состояние
    if (ui->navigationList->count() > 0) {
        ui->navigationList->setCurrentRow(0);  // Отображаем первый элемент по умолчанию
    }
}

// Функция для инициализации пустых элементов
void MainWindow::initializeEmptyItems(int count) {
    for (int i = 0; i < count; ++i) {
        ui->navigationList->addItem("");  // Пустой элемент списка
        QLabel* label = new QLabel(" ");  // Пустой виджет контента
        ui->contentStack->addWidget(label);
    }
}

// Функция для загрузки данных из JSON файла
bool MainWindow::loadDataFromFile(const QString& fileName) {
    QFile file(fileName);  // Проверим файл по указанному пути
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл:" << file.errorString();  // Отладка ошибки
        return false;
    }

    QByteArray fileData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);

    if (!jsonDoc.isArray()) {
        qDebug() << "Файл не является JSON массивом.";
        return false;
    }

    QJsonArray jsonArray = jsonDoc.array();
    int jsonItemCount = jsonArray.size();

    // Убедимся, что список и стеки пусты перед добавлением новых данных
    ui->navigationList->clear();
    while (ui->contentStack->count() > 0) {
        QWidget* widget = ui->contentStack->widget(0);
        ui->contentStack->removeWidget(widget);
        delete widget;
    }

    // Добавляем элементы из JSON
    for (int i = 0; i < jsonItemCount; ++i) {
        QJsonObject obj = jsonArray[i].toObject();
        QString title = obj["title"].toString();
        QString content = obj["content"].toString();

        qDebug() << "Заголовок:" << title;
        qDebug() << "Контент:" << content;

        // Добавляем элемент в список навигации
        ui->navigationList->addItem(title);

        // Создаем новый QLabel для контента и добавляем в QStackedWidget
        QLabel *label = new QLabel(content, this);
        label->setWordWrap(true);
        ui->contentStack->addWidget(label);
    }

    file.close();
    return true;
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

