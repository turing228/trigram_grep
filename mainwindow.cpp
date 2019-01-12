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


bool is_indexing = false;
bool is_searching = false;

main_window::main_window(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

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


    connect(ui->lineEdit_string, &QLineEdit::textChanged, this, &main_window::search_string);

    ui->progressBar_indexing->setValue(0);
    ui->progressBar_searching->setValue(0);

    buttons_start();

    //index_directory(QDir::homePath());
}

main_window::~main_window() {}

void main_window::select_directory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Indexing",
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    index_directory(dir);
}

void main_window::index_directory(QString const &dir) {
    ui->treeWidget->clear();
    setWindowTitle(QString("Directory Content - %1").arg(dir));
    QDir d(dir);

    trigram = Trigram(d);

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
    //connect(ui->actionScan_Directory, SIGNAL(clicked()), threadIndexing, SLOT(quit()), Qt::DirectConnection);
    //connect(ui->actionScan_Directory, SIGNAL(clicked()), indexingWorker, SLOT(deleteLater()), Qt::DirectConnection);

    // Connect worker to runstop-button
    connect(this, SIGNAL(startIndexingWork()), indexingWorker, SLOT(StartWork()));
    connect(this, SIGNAL(stopIndexingWork()), indexingWorker, SLOT(StopWork()));

    // Make sure the threadIndexing object is deleted after execution has finished.
    connect(threadIndexing, SIGNAL(finished()), threadIndexing, SLOT(deleteLater()), Qt::DirectConnection);

    threadIndexing->start();


    ui->progressBar_indexing->setValue(100);
}

void main_window::search_string() {
    ui->treeWidget->clear();

    QString text = ui->lineEdit_string->text();

    if (text.length() >= 3) {

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

        threadSearching->start();

        ui->progressBar_searching->setValue(100);

    }
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
        ui->progressBar_searching->setValue(num / trigram.file_vector.size());
    } else {
        ui->progressBar_searching->setValue(100);
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
        item->setText(2, file_path);

        mtxx.lock();
        ui->treeWidget->addTopLevelItem(item);
//        ui->progressBar_searching->setValue();
        mtxx.unlock();
    }
}

void main_window::onUpdateProgressSearching(int num) {
    if (trigram.files.size() > 0) {
        ui->progressBar_searching->setValue(num / trigram.files.size());
    } else {
        ui->progressBar_searching->setValue(100);
    }
}

void main_window::buttons_start() {
    ui->actionScan_Directory->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_indexing->setText("Run/Stop searching");
    ui->pushButton_stop_indexing->setDisabled(true);
}

void main_window::buttons_indexing() {
    ui->actionScan_Directory->setDisabled(true);

    ui->pushButton_stop_indexing->setText("Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run/Stop searching");
    ui->pushButton_stop_indexing->setDisabled(true);
}

void main_window::buttons_indexing_stopped() {
    ui->actionScan_Directory->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run indexing");
    ui->pushButton_stop_indexing->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run/Stop searching");
    ui->pushButton_stop_indexing->setDisabled(true);
}

void main_window::buttons_wait_search() {
    ui->actionScan_Directory->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_indexing->setText("Run/Stop searching");
    ui->pushButton_stop_indexing->setDisabled(true);
}

void main_window::buttons_searching() {
    ui->actionScan_Directory->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_indexing->setText("Stop searching");
    ui->pushButton_stop_indexing->setDisabled(false);
}

void main_window::buttons_searching_stopped() {
    ui->actionScan_Directory->setDisabled(false);

    ui->pushButton_stop_indexing->setText("Run/Stop indexing");
    ui->pushButton_stop_indexing->setDisabled(true);

    ui->pushButton_stop_indexing->setText("Run searching");
    ui->pushButton_stop_indexing->setDisabled(false);
}


void main_window::pushButton_stop_indexing_clicked() {
    if (is_indexing) {
        buttons_indexing_stopped();
        emit stopIndexingWork();
    } else {
        buttons_indexing();
        emit startIndexingWork();
    }
}

void main_window::pushButton_stop_searching_clicked() {
    if (is_searching) {
        buttons_searching_stopped();
        emit stopSearchingWork();
    } else {
        buttons_searching();
        emit startSearchingWork();
    }
}
