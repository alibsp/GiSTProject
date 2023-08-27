#include "../headers/part_class.hpp"

Part::Part(QString dataPath, QObject *parent) : QObject(parent), m_dataPath(dataPath)
{
    // testInserts();
    //dropGists();
    //loadGists();
    ;
    //char path[10]="data/";
    struct stat st;
    if (stat(dataPath.toUtf8().data(), &st) == -1)
        mkdir(dataPath.toUtf8().data(), 0700);
}

QString Part::dataPath() const
{
    return m_dataPath;
}

void Part::setDataPath(QString dataPath)
{
    if (m_dataPath == dataPath)
        return;

    m_dataPath = dataPath;
    emit dataPathChanged(m_dataPath);
}
