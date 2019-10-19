//
// Created by jesus on 12.01.19.
//

#ifndef TRIGRAM_GREP_SEARCHINGWORKER_H
#define TRIGRAM_GREP_SEARCHINGWORKER_H


#include "back/headers/SearchingWorker.h"

#include <QThread>
#include <mutex>
#include <QtCore/QObject>
#include <QtCore/QSet>

class SearchingWorker : public QObject {
Q_OBJECT
public:
    explicit SearchingWorker(QObject *parent = nullptr);

    explicit SearchingWorker(QString const &);
    //ClusterWorker();

    //virtual ~ClusterWorker();
    bool isNew = true;
    bool Stop = false;

    QString searched_string = "";

public slots:

    void process();

    void StopWork();

    void StartWork();

signals:

    void finished();

    void error(QString err);

    void searchInFileEnded(int);

    void updateProgressSearching(int);

    //void running();
    //void stopped();

private:
    void search_file(QString const &file_path);

    QSet<uint32_t> trigramed_string;

    void trigram_string();
    //void index_file(size_t file_number);

    //size_t lastCluster;
    volatile bool running, stopped;
};

//extern int cur_id;


#endif //TRIGRAM_GREP_SEARCHINGWORKER_H
