#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QTreeWidgetItem>
#include <back/headers/Trigram.h>
#include <QtWidgets/QLineEdit>
#include <QtCore/QTextStream>
#include <QtGui/QDesktopServices>
#include <QtCore/QQueue>


bool is_indexing = false;
bool is_searching = false;

int searching_time = 0;
QTime last_start_searching_time;
int indexing_time = 0;
QTime last_start_indexing_time;


main_window::main_window(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setMinimumWidth(20);
    ui->treeWidget->setColumnWidth(0, 60);
    ui->treeWidget->setColumnWidth(1, 150);
    ui->treeWidget->setColumnWidth(2, 150);
    ui->treeWidget->header()->setSectionHidden(3, true);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);


    connect(ui->pushButton_stop_indexing, &QPushButton::clicked, this, &main_window::pushButton_stop_indexing_clicked);
    connect(ui->pushButton_stop_searching, &QPushButton::clicked, this,
            &main_window::pushButton_stop_searching_clicked);

    connect(ui->pushButton_previous, &QPushButton::clicked, this, &main_window::pushButton_previous_clicked);
    connect(ui->pushButton_next, &QPushButton::clicked, this, &main_window::pushButton_next_clicked);

    connect(ui->pushButton_open_path, SIGNAL(clicked()), this, SLOT(pushButton_open_path_clicked()));
    connect(ui->pushButton_open_file, SIGNAL(clicked()), this, SLOT(pushButton_open_file_clicked()));

    connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem * , int)), this,
            SLOT(show_file(QTreeWidgetItem * )));

    connect(ui->lineEdit_string, &QLineEdit::textChanged, this, &main_window::search_string);

    ui->progressBar_indexing->setValue(0);
    ui->progressBar_searching->setValue(0);

    ui->textEdit->setReadOnly(true);

    buttons_start();


    last_start_indexing_time.start();
    last_start_searching_time.start();

    //index_directory(QDir::homePath());

    tmr = new QTimer(this); // Создаем объект класса QTimer и передаем адрес переменной
    tmr->setInterval(1); // Задаем интервал таймера
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime())); // Подключаем сигнал таймера к нашему слоту
    tmr->start(); // Запускаем таймер


    QList<int> list = ui->splitter->sizes();
    list.replace(0, this->width() / 2);
    list.replace(1, this->width() / 2);
    ui->splitter->setSizes(list);

    ui->lineEdit_string->setFixedWidth(250);
}

main_window::~main_window() {
    if (tmr) {
        delete tmr;
    }
}

QString cur_dir;

QQueue<QString> watched_directories;

void main_window::select_directory() {
    while (!watched_directories.isEmpty()) {
        auto t = watched_directories.front();
        watched_directories.pop_back();
        watcher.removePath(t);
    }

    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Indexing",
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    cur_dir = dir;


    index_directory(dir);
}

void main_window::index_again() {
    index_directory(cur_dir);
}

void main_window::index_directory(QString const &dir) {

    buttons_indexing();

    ui->treeWidget->clear();
    ui->textEdit->clear();
    setWindowTitle(QString("Directory Content - %1").arg(dir));
    QDir d(dir);

    indexing_time = 0;
    searching_time = 0;

    trigram = Trigram();
    trigram_directory(d);
    //trigram = Trigram(d);

    threadIndexing = new QThread;
    indexingWorker = new IndexingWorker;

    // Give QThread ownership of Worker Object
    indexingWorker->moveToThread(threadIndexing);

    // Connect worker error signal to this errorHandler SLOT.
    //connect(indexingWorker, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));

    // Connects the threadIndexing’s started() signal to the process() slot in the worker, causing it to start.
    connect(indexingWorker, SIGNAL(indexingFileEnded(int)), SLOT(onIndexingFileEnded(int)));
    connect(indexingWorker, SIGNAL(updateProgressIndexing(int)), SLOT(onUpdateProgressIndexing(int)));
    connect(threadIndexing, SIGNAL(started()), indexingWorker, SLOT(StartWork()));

    // Connect worker finished signal to trigger threadIndexing quit, then delete.
    connect(indexingWorker, SIGNAL(finished()), threadIndexing, SLOT(quit()), Qt::DirectConnection);
    connect(indexingWorker, SIGNAL(finished()), indexingWorker, SLOT(deleteLater()), Qt::DirectConnection);

    connect(ui->actionScan_Directory, &QAction::triggered, threadIndexing, &QThread::quit);
    connect(ui->actionScan_Directory, &QAction::triggered, indexingWorker, &QThread::deleteLater);

    // Connect worker to runstop-button
    connect(this, SIGNAL(startIndexingWork()), indexingWorker, SLOT(StartWork()));
    connect(this, SIGNAL(stopIndexingWork()), indexingWorker, SLOT(StopWork()));

    connect(indexingWorker, SIGNAL(indexingStopped()), this, SLOT(search_string()));

    // Make sure the threadIndexing object is deleted after execution has finished.
    connect(threadIndexing, SIGNAL(finished()), threadIndexing, SLOT(deleteLater()), Qt::DirectConnection);
    connect(indexingWorker, SIGNAL(finished_fully()), this, SLOT(buttons_indexing_ended()), Qt::DirectConnection);

    threadIndexing->start();

    is_indexing = true;
    buttons_indexing();

    //ui->progressBar_indexing->setValue(100);
}

void main_window::search_string() {
    searching_time = 0;

    ui->treeWidget->clear();

    QString text = ui->lineEdit_string->text();

    if (text.length() >= 3) {

        buttons_searching();

        threadSearching = new QThread;
        searchingWorker = new SearchingWorker(text);

        // Give QThread ownership of Worker Object
        searchingWorker->moveToThread(threadSearching);

        // Connect worker error signal to this errorHandler SLOT.
        //connect(searchingWorker, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));

        // Connects the threadSearching’s started() signal to the process() slot in the worker, causing it to start.
        connect(searchingWorker, SIGNAL(searchInFileEnded(int)), SLOT(onSearchInFileEnded(int)));
        connect(searchingWorker, SIGNAL(updateProgressSearching(int)), SLOT(onUpdateProgressSearching(int)));
        connect(threadSearching, SIGNAL(started()), searchingWorker, SLOT(StartWork()));

        // Connect worker finished signal to trigger threadSearching quit, then delete.
        connect(searchingWorker, SIGNAL(finished()), threadSearching, SLOT(quit()), Qt::DirectConnection);
        connect(searchingWorker, SIGNAL(finished()), searchingWorker, SLOT(deleteLater()), Qt::DirectConnection);

        connect(ui->lineEdit_string, &QLineEdit::textChanged, threadSearching, &QThread::quit, Qt::DirectConnection);
        connect(ui->lineEdit_string, &QLineEdit::textChanged, searchingWorker, &SearchingWorker::deleteLater,
                Qt::DirectConnection);
        //connect(ui->actionScan_Directory, SIGNAL(clicked()), threadSearching, SLOT(quit()), Qt::DirectConnection);
        //connect(ui->actionScan_Directory, SIGNAL(clicked()), searchingWorker, SLOT(deleteLater()), Qt::DirectConnection);

        // Connect worker to runstop-button
        connect(this, SIGNAL(startSearchingWork()), searchingWorker, SLOT(StartWork()));
        connect(this, SIGNAL(stopSearchingWork()), searchingWorker, SLOT(StopWork()));

        // Make sure the threadSearching object is deleted after execution has finished.
        connect(threadSearching, SIGNAL(finished()), threadSearching, SLOT(deleteLater()), Qt::DirectConnection);
        connect(threadSearching, SIGNAL(finished()), this, SLOT(buttons_searching_ended()), Qt::DirectConnection);

        threadSearching->start();
        is_searching = true;

        //ui->progressBar_searching->setValue(100);

    }
}

QVector<QTextCursor> cursors;
int num_cur_cursor;
int count;
QTreeWidgetItem *showedItemOpened;

void main_window::show_file(QTreeWidgetItem *itemOpened) {
    ui->textEdit->clear();
    cursors.clear();

    showedItemOpened = itemOpened;

    num_cur_cursor = 0;

    //int file_number = itemOpened->data(0, Qt::DisplayRole).toInt() - 1;
    //trigram.files_with_string[file_number];

    QString selected = itemOpened->data(2, Qt::DisplayRole).toString();

    count = 0;

    if (!selected.isEmpty()) {
        QFile file(selected);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            ui->textEdit->setText("Cannot open the selected file");
        } else {
            QTextStream in(&file);
            ui->textEdit->document()->setPlainText(in.readAll());

            QString searchString = ui->lineEdit_string->text();
            QTextDocument *document = ui->textEdit->document();

            QTextCursor highlightCursor(document);
            QTextCursor cursor(document);

            cursor.beginEditBlock();

            QTextCharFormat plainFormat(highlightCursor.charFormat());
            QTextCharFormat colorFormat = plainFormat;
            //colorFormat.setForeground(Qt::red);
            colorFormat.setBackground(Qt::green);

            while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
                highlightCursor = document->find(searchString, highlightCursor);

                if (!highlightCursor.isNull()) {
                    //found = true;
                    count++;
                    highlightCursor.movePosition(QTextCursor::NoMove,
                                                 QTextCursor::KeepAnchor);
                    highlightCursor.mergeCharFormat(colorFormat);
                    //ui->textEdit->setTextCursor(highlightCursor);
                    cursors.push_back(highlightCursor);
                }
            }

            cursor.endEditBlock();

            if (!cursors.isEmpty()) {
                ui->textEdit->setTextCursor(cursors[0]);
                ui->pushButton_previous->setDisabled(false);
                ui->pushButton_next->setDisabled(false);
            } else {
                ui->pushButton_previous->setDisabled(true);
                ui->pushButton_next->setDisabled(true);
            }
            /*QList<QTextEdit::ExtraSelection> found_string;
            ui->textEdit->moveCursor(QTextCursor::Start);
            QString text = ui->lineEdit_string->text();
            while (ui->textEdit->find(text)) {
                QTextEdit::ExtraSelection extraSelection;
                extraSelection.format.setBackground(QColor(Qt::blue).lighter(120));
                extraSelection.cursor = textCursor();
            }*/
            ui->pushButton_open_path->setDisabled(false);
            ui->pushButton_open_file->setDisabled(false);
        }
    } else {
        ui->textEdit->setText("Select file");
        ui->pushButton_previous->setDisabled(true);
        ui->pushButton_next->setDisabled(true);
    }

    ui->label_matches_count->setText(QString::number(1) + QString("/") + QString::number(count));
}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}

void main_window::onIndexingFileEnded(int id) {
//    if (trigram.files.size() > 0) {
//        ui->progressBar_searching->setValue(num / trigram.files.size());
//    } else {
//        ui->progressBar_searching->setValue(100);
//    }
}

void main_window::onUpdateProgressIndexing(int num) {
    if (trigram.file_vector.size() > 0) {
        ui->progressBar_indexing->setValue((100 * (num + 1)) / trigram.file_vector.size());
    } else {
        ui->progressBar_indexing->setValue(100);
    }
}

std::mutex mtxx;

void main_window::onSearchInFileEnded(int id) {
    if (id >= 0) {
        int i = id;
        mtx.lock();
        QString file_path = trigram.files_with_string[i];
        QFileInfo file_info = QFile(file_path);
        mtx.unlock();
        TreeWidgetItem *item = new TreeWidgetItem;
        item->setText(0, QString::number(i + 1));
        item->setText(1, file_info.fileName());
        item->setText(2, file_info.absoluteFilePath());
        item->setText(3, file_info.absolutePath());

        mtxx.lock();
        ui->treeWidget->addTopLevelItem(item);
//        ui->progressBar_searching->setValue();
        mtxx.unlock();
    }
}

void main_window::onUpdateProgressSearching(int num) {
    if (trigram.files.size() > 0) {
        ui->progressBar_searching->setValue((100 * (num + 1)) / trigram.files.size());
    } else {
        ui->progressBar_searching->setValue(100);
    }
}

void main_window::buttons_start() {
    ui->actionScan_Directory->setDisabled(false);
    ui->lineEdit_string->setDisabled(true);

    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_searching->setText("Run/Stop searching");
    ui->pushButton_stop_searching->setDisabled(true);

    ui->pushButton_previous->setDisabled(true);
    ui->pushButton_next->setDisabled(true);

    ui->pushButton_open_path->setDisabled(true);
    ui->pushButton_open_file->setDisabled(true);


    ui->label_indexing_time->setText(QTime(0, 0, 0, 0).toString("mm:ss.zzz"));
    ui->label_searching_time->setText(QTime(0, 0, 0, 0).toString("mm:ss.zzz"));

    ui->label_matches_count->setText("0/0");
}

void main_window::buttons_indexing() {
    is_indexing = true;
    is_searching = false;
    last_start_indexing_time.restart();

    ui->actionScan_Directory->setDisabled(true);
    ui->lineEdit_string->setDisabled(true);

    ui->pushButton_stop_indexing->setText("Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(false);

    ui->pushButton_stop_searching->setText("Run/Stop searching");
    ui->pushButton_stop_searching->setDisabled(true);

    ui->pushButton_previous->setDisabled(true);
    ui->pushButton_next->setDisabled(true);

    ui->pushButton_open_path->setDisabled(true);
    ui->pushButton_open_file->setDisabled(true);


    ui->progressBar_indexing->setValue(0);
    ui->progressBar_searching->setValue(0);

    ui->label_matches_count->setText("0/0");
}

void main_window::buttons_indexing_stopped() {
    is_indexing = false;
    is_searching = false;
    ui->actionScan_Directory->setDisabled(false);
    ui->lineEdit_string->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run indexing");
    ui->pushButton_stop_indexing->setDisabled(false);

    ui->pushButton_stop_searching->setText("Run/Stop searching");
    ui->pushButton_stop_searching->setDisabled(true);

    ui->pushButton_previous->setDisabled(true);
    ui->pushButton_next->setDisabled(true);

    ui->pushButton_open_path->setDisabled(true);
    ui->pushButton_open_file->setDisabled(true);
}

void main_window::buttons_indexing_ended() {
    is_indexing = false;
    is_searching = false;
    ui->actionScan_Directory->setDisabled(false);
    ui->lineEdit_string->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_searching->setText("Run/Stop searching");
    ui->pushButton_stop_searching->setDisabled(true);

    ui->pushButton_previous->setDisabled(true);
    ui->pushButton_next->setDisabled(true);

    ui->pushButton_open_path->setDisabled(true);
    ui->pushButton_open_file->setDisabled(true);
}

void main_window::buttons_searching() {
    is_indexing = false;
    is_searching = true;
    last_start_searching_time.restart();

    ui->actionScan_Directory->setDisabled(false);
    ui->lineEdit_string->setDisabled(false);

//    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
//    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_searching->setText("Stop searching");
    ui->pushButton_stop_searching->setDisabled(true);

    ui->pushButton_previous->setDisabled(true);
    ui->pushButton_next->setDisabled(true);

    ui->pushButton_open_path->setDisabled(true);
    ui->pushButton_open_file->setDisabled(true);
}

void main_window::buttons_searching_stopped() {
    is_indexing = false;
    is_searching = false;
    ui->actionScan_Directory->setDisabled(false);
    ui->lineEdit_string->setDisabled(false);

//    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
//    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_searching->setText("Run searching");
    ui->pushButton_stop_searching->setDisabled(true);

    ui->pushButton_previous->setDisabled(true);
    ui->pushButton_next->setDisabled(true);

    ui->pushButton_open_path->setDisabled(true);
    ui->pushButton_open_file->setDisabled(true);
}

void main_window::buttons_searching_ended() {
    is_indexing = false;
    is_searching = false;
    ui->actionScan_Directory->setDisabled(false);
    ui->lineEdit_string->setDisabled(false);

//    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
//    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_searching->setText("Run/Stop searching");
    ui->pushButton_stop_searching->setDisabled(true);

    ui->pushButton_previous->setDisabled(true);
    ui->pushButton_next->setDisabled(true);

    ui->pushButton_open_path->setDisabled(true);
    ui->pushButton_open_file->setDisabled(true);
}


void main_window::pushButton_stop_indexing_clicked() {
    if (is_indexing) {
        buttons_indexing_stopped();
        emit stopIndexingWork();
    } else {
        buttons_indexing();
        emit startIndexingWork();
    }
    //is_indexing = !is_indexing;
}

void main_window::pushButton_stop_searching_clicked() {
    if (is_searching) {
        buttons_searching_stopped();
        emit stopSearchingWork();
    } else {
        buttons_searching();
        emit startSearchingWork();
    }
    is_searching = !is_searching;
}


void main_window::updateTime() {
    if (is_indexing) {
        indexing_time += last_start_indexing_time.elapsed();
        //ui->label_indexing_time->setText(QTime::currentTime().toString("mm:ss.zzz"));
        ui->label_indexing_time->setText(QTime(0, 0, 0, 0).addMSecs(indexing_time).toString("mm:ss.zzz"));
        last_start_indexing_time.restart();
    }
    if (is_searching) {
        searching_time += last_start_searching_time.elapsed();
        ui->label_searching_time->setText(QTime(0, 0, 0, 0).addMSecs(searching_time).toString("mm:ss.zzz"));
        last_start_searching_time.restart();
    }
}

void main_window::pushButton_previous_clicked() {
    if (cursors.size() > 0) {
        num_cur_cursor--;
        if (num_cur_cursor == -1) {
            num_cur_cursor = cursors.size() - 1;
        }
        ui->textEdit->setTextCursor(cursors[num_cur_cursor]);
        ui->label_matches_count->setText(QString::number(num_cur_cursor + 1) + QString("/") + QString::number(count));
    }
}

void main_window::pushButton_next_clicked() {
    if (cursors.size() > 0) {
        num_cur_cursor++;
        if (num_cur_cursor == cursors.size()) {
            num_cur_cursor = 0;
        }
        ui->textEdit->setTextCursor(cursors[num_cur_cursor]);
        ui->label_matches_count->setText(QString::number(num_cur_cursor + 1) + QString("/") + QString::number(count));
    }
}

void main_window::pushButton_open_path_clicked() {
    QString selected = showedItemOpened->data(3, Qt::DisplayRole).toString();
    if (!selected.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(selected + '/'));
}

void main_window::pushButton_open_file_clicked() {
    QString selected = showedItemOpened->data(2, Qt::DisplayRole).toString();
    if (!selected.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(selected));
}

void main_window::trigram_directory(QDir const &directory) {
    trigram.file_list = trigram.listFiles(directory);

    for (auto i : trigram.file_list) {
        trigram.file_vector.push_back(i);
    }
}

QFileInfoList main_window::listFiles(QDir const &directory) {
    QDir dir(directory);

    QFileInfoList list = dir.entryInfoList(
            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden); //| QDir::NoDotAndDotDot
    QFileInfoList result;

    for (QFileInfo file_info : list) {
        if (!file_info.isSymLink() && file_info.isDir()) {
            result.append(listFiles(file_info.absoluteFilePath()));
            watched_directories.push_back(file_info.absoluteFilePath());
            watcher.addPath(file_info.absoluteFilePath());
            connect(&watcher, &QFileSystemWatcher::fileChanged, this, &main_window::index_again);
            connect(&watcher, &QFileSystemWatcher::directoryChanged, this, &main_window::index_again);
        } else {
            if (file_info.isSymLink()) {
                file_info = file_info.symLinkTarget();
            }
            result.push_back(file_info);
        }
    }

    return result;
}