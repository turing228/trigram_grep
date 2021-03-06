//
// Created by jesus on 12.01.19.
//

#ifndef TRIGRAM_GREP_INDEXINGWORKER_H
#define TRIGRAM_GREP_INDEXINGWORKER_H


#include <QThread>
#include <back/headers/IndexingWorker.h>
#include <mutex>
#include <QtCore/QObject>

class IndexingWorker : public QObject {
Q_OBJECT
public:
    explicit IndexingWorker(QObject *parent = nullptr);
    //ClusterWorker();

    //virtual ~ClusterWorker();
    bool isNew = true;
    bool Stop = false;

public slots:

    void process();

    void StopWork();

    void StartWork();

signals:

    void finished();

    void error(QString err);

    void indexingFileEnded(int);

    void updateProgressIndexing(int);

    void indexingStopped();

    void finished_fully();
    //void running();
    //void stopped();

private:

    void index_file(size_t file_number);

    size_t lastFile;
    volatile bool running, stopped;
};

extern int cur_id;


#endif //TRIGRAM_GREP_INDEXINGWORKER_H
