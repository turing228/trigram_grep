//
// Created by jesus on 12.01.19.
//

#include <back/headers/Trigram.h>
#include "back/headers/SearchingWorker.h"
#include <unordered_map>
#include <algorithm>
#include <assert.h>
#include <back/headers/SearchingWorker.h>
#include <fstream>
#include <QtCore/QSet>
#include <QtCore/QMap>
#include <QtCore/QTextStream>
#include <algorithm>
#include <functional>

size_t lastSearchingFile;

SearchingWorker::SearchingWorker(QObject *parent) : QObject(parent), stopped(false), running(false) {
    lastSearchingFile = 0;
    Stop = false;
    isNew = true;

}

SearchingWorker::SearchingWorker(QString const &searched_string) : stopped(false), running(false),
                                                                   searched_string(searched_string) {
    trigram_string();
    mtx.lock();
    trigram.files_with_string.clear();
    trigram.cur_id_files = -1;
    mtx.unlock();
    lastSearchingFile = 0;
    Stop = false;
    isNew = true;
}


void SearchingWorker::StopWork() {
    stopped = true;
    running = false;
}

void SearchingWorker::StartWork() {
    stopped = false;
    running = true;
    process();
}

void SearchingWorker::process() {
    if (!running || stopped) {
        return;
    }

    if (isNew) {
        lastSearchingFile = 0;
        isNew = false;
    }

    size_t i = lastSearchingFile;
    if (i < trigram.files.size()) {
        search_file(trigram.files[i]);
        emit updateProgressSearching(lastSearchingFile);
        lastSearchingFile++;
    } else {
        //emit clusterEnded(-1);
        StopWork();
        emit finished();
    }

    QMetaObject::invokeMethod(this, "process", Qt::QueuedConnection);
}

void SearchingWorker::trigram_string() {
    QSet<uint32_t> result;
    std::string std_str = searched_string.toStdString();
    char buffer[3];
    if (std_str.length() >= 3) {
        buffer[1] = std_str[0];
        buffer[2] = std_str[1];
        for (size_t i = 2; i < std_str.length(); ++i) {
            buffer[0] = buffer[1];
            buffer[1] = buffer[2];
            buffer[2] = std_str[i];
            uint32_t number = (buffer[0] << 16) + (buffer[1] << 8) + buffer[2];
            result.insert(number);
        }
    }
    trigramed_string = result;
}

void SearchingWorker::search_file(QString const &file_path) {

    bool ok = true;
    for (auto it = trigramed_string.begin(); it != trigramed_string.end(); ++it) {
        if (trigram.trigrams[file_path].find(*it) == trigram.trigrams[file_path].end()) {
            ok = false;
            return; // TODO
        }
    }

    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly)) {
        return;     //TODO
    }

    QTextStream in(&file);

    QString temp;
    QString *qline = &temp;

    bool is_ok = false;
    while (!in.atEnd()) {
        if (in.readLineInto(qline, 100)) {
            std::string line = qline->toStdString();
            if (line.find(searched_string.toStdString()) < line.length()) {
                is_ok = true;
                break;
            }
        }
    }

    if (is_ok)
            emit searchInFileEnded(trigram.push(file_path));
}