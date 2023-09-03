#include "../headers/part_class.hpp"

int compareBins(void* a, void* b)
{
    //char notEqual = 0;
    if(!a || !b)
        std::cout<<"IS NULL"<<std::endl;
    unsigned char* first = ((UUID*) a)->val;
    unsigned char* second = ((UUID*) b)->val;
    return 0;
    for (char i = 0; i < 16; i++)   //equal
    {
        if( (first[i] ^ second[i]) == 0 ) { continue;}
        else
        {
            if( (first[i] - second[i]) < 0 ) {return -1;}   //Second is bigger
            return 1;   //They weren't equal, nor second was bigger! So first must be bigger!
        }
    }
    return 0;   //They were equal
}

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
