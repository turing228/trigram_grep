//
// Created by jesus on 12.01.19.
//

#include <back/headers/Trigram.h>
#include <QtCore/QDir>

#include "back/headers/Trigram.h"

Trigram trigram;
std::mutex mtx;

Trigram::Trigram(QDir const &directory) {
    file_list = listFiles(directory);

    for (auto i : file_list) {
        file_vector.push_back(i);
    }
}

QFileInfoList Trigram::listFiles(QDir const &directory) {
    QDir dir(directory);

    QFileInfoList list = dir.entryInfoList(
            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden); //| QDir::NoDotAndDotDot
    QFileInfoList result;

    for (QFileInfo file_info : list) {
        if (!file_info.isSymLink() && file_info.isDir()) {
            result.append(listFiles(file_info.absoluteFilePath()));
        } else {
            if (file_info.isSymLink()) {
                file_info = file_info.symLinkTarget();
            }
            result.push_back(file_info);
        }
    }

    return result;
}

int Trigram::push(QString file_path) {
    mtx.lock();
    files_with_string.push_back(file_path);
    cur_id_files++;
    int ans = cur_id_files;
    mtx.unlock();
    return ans;
}
