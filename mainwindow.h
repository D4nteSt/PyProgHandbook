#ifndef MAINWINDOW_H
#define MA

/**
 * @file mainwindow.h
 * @brief Определение класса MainWindow и всех его компонентов.
 *
 * Этот файл является заголовочным файлом главного окна приложения, содержащим объявление класса MainWindow,
 * слотов, методов и переменных, необходимых для функционирования приложения. Основная задача класса MainWindow -
 * управление пользовательским интерфейсом приложения, реализация навигации по разделам справочника, добавления
 * закладок и работы с сохраненными закладками. Используются элементы управления Qt, такие как QListWidget,
 * QTextBrowser.
 */

#include <QMainWindow>
#include <QApplication>
#include <QListWidget>
#include <QTextBrowser>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDebug>
#include <QPair>
#include <QVector>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Класс, реализующий главное окно приложения.
 *
 * Этот класс управляет основными функциями приложения, такими как навигация по страницам справочника,
 * управление закладками, отображение информации о программе и выход. Включает слоты для обработки действий
 * пользователя и приватные методы для вспомогательных операций, связанных с интерфейсом.
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса MainWindow.
     * @param parent Родительский виджет, по умолчанию nullptr.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор класса MainWindow.
     */
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * @brief Слот для кнопки перехода на следующую страницу.
     * Устанавливает следующую страницу в списке активной и обновляет состояние навигационных кнопок.
     */
    void on_NextPageButton_clicked();

    /**
     * @brief Слот для кнопки перехода на предыдущую страницу.
     * Устанавливает предыдущую страницу в списке активной и обновляет состояние навигационных кнопок.
     */
    void on_PreviousPageButton_clicked();

    /**
     * @brief Слот для показа информации о приложении.
     * Отображает сообщение "О программе" с краткой информацией о проекте.
     */
    void on_menuAbout_triggered();

    /**
     * @brief Слот для отображения или скрытия списка закладок.
     * Показывает или скрывает список закладок в зависимости от текущего состояния.
     */
    void on_OpenBookmarksButton_clicked();

    /**
     * @brief Слот для добавления или удаления закладки.
     * Добавляет текущую страницу в закладки или удаляет её из закладок.
     */
    void on_BookmarkButton_clicked();

    /**
     * @brief Слот, срабатывающий при выборе элемента в списке навигации.
     * Позволяет перейти на выбранную страницу справочника.
     * @param currentRow Индекс текущего выбранного элемента в списке навигации.
     */
    void onNavigationItemSelected(int currentRow);

private:
    QListWidget* navigationList; ///< Список навигации для выбора разделов справочника.

    /**
     * @brief Загружает текст из файла.
     * @param filePath Путь к файлу.
     * @return Строка с содержимым файла.
     */
    QString loadTextFromFile(const QString& filePath);

    /**
     * @brief Загружает стиль из файла.
     * @param filePath Путь к файлу стилей.
     * @return Строка со стилем для применения.
     */
    QString loadStyleSheetFromFile(const QString& filePath);

    /**
     * @brief Загружает данные из файла JSON.
     * @param fileName Имя файла.
     * @return true, если данные успешно загружены, иначе false.
     */
    bool loadDataFromFile(const QString& fileName);

    /**
     * @brief Восстанавливает исходный список навигации.
     * Используется для отмены показа закладок и возврата к списку всех страниц.
     */
    void restoreNavigationList();

    /**
     * @brief Отображает список закладок.
     * Показывает только те элементы, которые сохранены в закладках.
     */
    void showBookmarkList();

    /**
     * @brief Сохраняет текущие закладки в файл.
     * Позволяет сохранить список закладок между сеансами работы с приложением.
     */
    void saveBookmarksToFile();

    /**
     * @brief Загружает закладки из файла.
     * Восстанавливает список закладок, сохраненный ранее.
     */
    void loadBookmarksFromFile();

    /**
     * @brief Обновляет состояние кнопок навигации.
     * Включает или отключает кнопки "Следующая" и "Предыдущая" в зависимости от текущей страницы.
     */
    void updateNavigationButtons();

    /**
     * @brief Обновляет состояние кнопки "Закладка".
     * Устанавливает текст и доступность кнопки закладки в зависимости от состояния текущей страницы.
     */
    void updateBookmarkButton();

    /**
     * @brief Обновляет состояние кнопки "Открыть закладки".
     * Меняет доступность кнопки "Открыть закладки" в зависимости от наличия закладок.
     */
    void updateOpenBookmarksButton();

    QVector<QPair<QString, QString>> bookmarks; ///< Вектор для хранения закладок (название и путь к странице).
    bool showingBookmarks; ///< Флаг, указывающий, отображаются ли в данный момент закладки.

    Ui::MainWindow *ui; ///< Указатель на графический интерфейс пользователя.
};
#endif // MAINWINDOW_H
