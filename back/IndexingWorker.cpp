//
// Created by jesus on 12.01.19.
//

#include <back/headers/Trigram.h>
#include "back/headers/IndexingWorker.h"
#include <unordered_map>
#include <algorithm>
#include <assert.h>
#include <back/headers/IndexingWorker.h>
#include <fstream>
#include <QtCore/QSet>
#include <QtCore/QMap>
#include <QtCore/QTextStream>

size_t lastIndexingFile;

IndexingWorker::IndexingWorker(QObject *parent) : QObject(parent), stopped(false), running(false) {
    lastIndexingFile = 0;
    Stop = false;
    isNew = true;
}


void IndexingWorker::StopWork() {
    stopped = true;
    running = false;
    emit indexingStopped();
}

void IndexingWorker::StartWork() {
    stopped = false;
    running = true;
    process();
}

void IndexingWorker::process() {
    if (!running || stopped) {
        return;
    }

    if (isNew) {
        lastIndexingFile = 0;
        isNew = false;
    }

    size_t i = lastIndexingFile;
    if (i < trigram.file_vector.size()) {
        index_file(i);
        emit updateProgressIndexing(i);
        lastIndexingFile++;

    } else {
        //emit clusterEnded(-1);
        emit finished_fully();
        StopWork();
        emit finished();
    }

    QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection);
}

void IndexingWorker::index_file(size_t file_number) {
    QString file_path = trigram.file_vector[file_number].absoluteFilePath();

    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly)) {
        return;     //TODO
    }

    QSet<quint32> file_trigrams;

    QTextStream in(&file);

    char buffer[3];
    int cur_size = 0;
    int count = 0;

    QString temp;
    QString *qline = &temp;
    while (!in.atEnd()) {
        if (in.readLineInto(qline, 100)) {
            std::string line = qline->toStdString();
            int i = 0;
            if (cur_size < 2) {
                while (i < line.size() && cur_size < 2) {
                    buffer[1 + cur_size] = line.at(i);
                    i++;
                    cur_size++;
                }
            }
            while (i < line.size()) {
                buffer[0] = buffer[1];
                buffer[1] = buffer[2];
                buffer[2] = line[i];
                i++;
                uint32_t number = (buffer[0] << 16) + (buffer[1] << 8) + buffer[2];
                file_trigrams.insert(number);
                count++;
                if (count > 200000) {
                    return;     //TODO
                }
            }
        }
//        std::string line = in.readLine().toStdString();
//        int i = 0;
//        if (cur_size < 2) {
//            while (i < line.size() && cur_size < 2) {
//                buffer[1 + cur_size] = line.at(i);
//                i++;
//                cur_size++;
//            }
//        }
//        while (i < line.size()) {
//            buffer[0] = buffer[1];
//            buffer[1] = buffer[2];
//            buffer[2] = line[i];
//            i++;
//            uint32_t number = (buffer[0] << 16) + (buffer[1] << 8) + buffer[2];
//            file_trigrams.insert(number);
//            count++;
//            if (count > 200000) {
//                return;     //TODO
//            }
//        }
    }
/*
    while (!file.atEnd()) {
        file.readLine(buffer, sizeof(buffer));
        uint32_t number = (buffer[0] << 16) + (buffer[1] << 8) + buffer[2];
        file_trigrams.insert(number);
        count++;
        if (count > 200000) {
            return;     //TODO
        }
    }*/

//
//    std::ifstream cin(file_path.toStdString());
//    QSet<quint32> file_trigrams;
//
//    std::string line;
//    char buffer[3];
//
//    int count = 0;
//    while (std::getline(cin, line)) {
//        buffer[0] = ' ';
//        buffer[1] = ' ';
//        buffer[2] = ' ';
//        line = line + "  ";
//        for (int i=0; i<line.length(); ++i) {
//            buffer[0] = buffer[1];
//            buffer[1] = buffer[2];
//            buffer[2] = line[i];
//            uint32_t number = (buffer[0]<<16) + (buffer[1] << 8) + buffer[2];
//            file_trigrams.insert(number);
//        }
//    }

    //mtx.lock();
    trigram.files.push_back(file_path);
    trigram.trigrams.insert(file_path, file_trigrams);
    //emit indexingFileEnded(5);
    //trigram.files.insert(file_path);
    //mtx.unlock();
}
