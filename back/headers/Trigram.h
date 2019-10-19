//
// Created by jesus on 12.01.19.
//

#ifndef TRIGRAM_GREP_TRIGRAM_H
#define TRIGRAM_GREP_TRIGRAM_H


#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QFileInfoList>
#include <mutex>
#include <QtCore/QSet>
#include <QtCore/QVector>

class Trigram {
public:
    QMap<QString, QSet<quint32>> trigrams;
    QVector<QString> files;
    QFileInfoList file_list;
    std::vector<QFileInfo> file_vector;

    std::vector<QString> files_with_string;

    Trigram() = default;

    explicit Trigram(QDir const &directory);

    QFileInfoList listFiles(QDir const &directory);

    int push(QString);

    int cur_id_files = -1;
private:

};

extern Trigram trigram;
extern std::mutex mtx;


#endif //TRIGRAM_GREP_TRIGRAM_H
