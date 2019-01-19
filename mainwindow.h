#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>


#include <QTimer>
#include <QTime>
#include <QtWidgets/QTreeWidgetItem>
#include <back/headers/IndexingWorker.h>
#include <back/headers/SearchingWorker.h>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QDir>

class TreeWidgetItem : public QTreeWidgetItem {
public:
    TreeWidgetItem() {}

    TreeWidgetItem(QTreeWidget *parent) : QTreeWidgetItem(parent) {}

private:
    bool operator<(const QTreeWidgetItem &other) const {
        int column = treeWidget()->sortColumn();
        if (column == 1) {
            column = 6;
        }
        if (column == 4) {
            column = 5;
        }
        bool isConvertAbleToInt;
        text(column).toInt(&isConvertAbleToInt);
        if (isConvertAbleToInt) {
            return text(column).toInt() < other.text(column).toInt();
        }

        return text(column).toLower() < other.text(column).toLower();
    }
};



namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = 0);
    ~main_window();

    QTimer *tmr;
private slots:

    void index_again();

    void updateTime();


    void select_directory();

    void index_directory(QString const &dir);
    void show_about_dialog();

    void onSearchInFileEnded(int);

    void onUpdateProgressSearching(int);

    void onIndexingFileEnded(int);

    void onUpdateProgressIndexing(int);

    void search_string();

    void show_file(QTreeWidgetItem *);


    void buttons_start();

    void buttons_indexing();

    void buttons_indexing_stopped();

    void buttons_indexing_ended();

    void buttons_searching();

    void buttons_searching_stopped();

    void buttons_searching_ended();



    void pushButton_stop_indexing_clicked();

    void pushButton_stop_searching_clicked();


    void pushButton_previous_clicked();

    void pushButton_next_clicked();


    void pushButton_open_path_clicked();

    void pushButton_open_file_clicked();

    void trigram_directory(QDir const &directory);

    QFileInfoList listFiles(QDir const &directory);

private:
    std::unique_ptr<Ui::MainWindow> ui;

    QThread *threadIndexing;
    IndexingWorker *indexingWorker;

    QThread *threadSearching;
    SearchingWorker *searchingWorker;

    QFileSystemWatcher watcher;

signals:

    void startIndexingWork();

    void stopIndexingWork();

    void startSearchingWork();

    void stopSearchingWork();

};

#endif // MAINWINDOW_H
