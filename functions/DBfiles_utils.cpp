#include "../headers/include_handler.hpp"
#include "../headers/part_class.hpp"

void Part::printAllKeys(QString treeName)
{
    QStringList results;
    gist *myGist=gists[treeName];

    bt_query_t q(bt_query_t::bt_nooper, &treeName);
    gist_cursor_t cursor;
    if(myGist->fetch_init(cursor, &q)!=RCOK)
    {
        cerr << "Can't initialize cursor." << endl;
        return ;
    }
    bool eof = false;
    smsize_t keysz=KEY_LEN, datasz=ID_LEN;

    char keyFound[KEY_LEN]={0};
    char id[ID_LEN]={0};
    while (!eof)
    {
        if(myGist->fetch(cursor, (void *)&keyFound, keysz, (void *)&id, datasz, eof)!=RCOK)
        {
            cerr << "Can't fetch from cursor." << endl;
            return ;
        }
        if (!eof)
        {
            cout<<"find:"<<(char*)&keyFound<<", id:"<<id<<endl;
            results.append(id);
        }
    }
    //cout<<"Execute Time: "<<timer.nsecsElapsed()<<" ns, record count:"<<results.count()<<endl;
}

void Part::importCSV(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        cerr <<  file.errorString().toStdString()<<endl;
        return ;
    }

    int i=0;
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        if(i && line!="\n" )
        {
            QList<QByteArray> columns = line.split(';');
            QString id=QString(columns[0]);
            QString keys=QString(columns[2]).replace("\"","");
            cout<<"Record "<<i<<endl;
            if(i==20674)
                i=20674;
            insertRecord(columns[0].data(), keys.toUtf8().data());
        }
        i++;
    }
    file.close();
}

void Part::loadGists()
{
    QDir dir(m_dataPath, "*.db");
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QString keyName = fileInfo.fileName().split(".")[0];

        gist *myGist  =gists[keyName];
        if(myGist==nullptr)
        {
            myGist = new gist();
            if(myGist->create(fileInfo.filePath().toLocal8Bit().data() , &bt_str_key_binary_data_ext)!=RCOK)
            {
                if(myGist->open(fileInfo.filePath().toLocal8Bit().data())==RCOK)
                    gists[keyName] = myGist;
            }
        }
    }
}

void Part::dropGists()
{
    QStringList list = findFiles(m_dataPath, QStringList() << "*.db" << "*.dat");
    for (int i = 0; i < list.size(); ++i)
    {
        QFile file (list[i]);
        file.remove();
    }
    QDir dir(m_dataPath);
    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList dirList = dir.entryList();
    for (int i=0; i<dirList.size(); ++i)
    {
        QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
        dir.rmdir(newPath);
    }
}
