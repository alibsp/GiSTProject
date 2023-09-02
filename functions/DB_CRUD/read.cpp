#include "../../headers/part_class.hpp"



QList<UUID> Part::findKey(const char * key_value)
{
    //QElapsedTimer timer;
    //timer.start();
    char key[KEY_LEN]={0};
    char value[KEY_LEN]={0};
    GeneralUtils::extractKeyValue(key_value, key, value);
    return findKey(key, value);
}


QList<UUID> Part::findKey(const char * key, const char * value)
{

    QList<UUID> results;
    gist *myGist=gists[key];

    bt_query_t q(bt_query_t::bt_eq, (void*)value);
    gist_cursor_t cursor;
    if(myGist->fetch_init(cursor, &q)!=RCOK)
    {
        cerr << "Can't initialize cursor." << endl;
        return results;
    }
    bool eof = false;
    smsize_t keysz=KEY_LEN, datasz=DATA_LEN;

    char keyFound[KEY_LEN]={0};
    char data[DATA_LEN]={0};
    unsigned int count=0;
    while (!eof)
    {
        if(myGist->fetch(cursor, (void *)&keyFound, keysz, (void *)&data, datasz, eof)!=RCOK)
        {
            cerr << "Can't fetch from cursor." << endl;
            return results;
        }
        if (!eof)
        {
            cout << "keyFound:" << key<<":"<<keyFound<<endl;

            memcpy(&count, data, 4);
            for(size_t i=0;i<count && i< 10;i++)
            {
                unsigned char bin_id[ID_LEN]={0};
                memcpy(bin_id, data+(4+i*ID_LEN), ID_LEN);
                //binToHexStr(uuid, id);
                UUID uuid(bin_id);
                results.append(uuid);
            }
            if(count==11)//go to postingFile
            {
                //---------------------------------------------|shahab|----------------------------------------------------
                char postingFilePath[255];
                unsigned char postingFileReadBuffer[PAGING_COUNT*RECORD_SIZE];
                char fileName[100];
                GeneralUtils::hashFileName(keyFound, fileName);
                snprintf(postingFilePath, 255, "%s%s/%s.dat", m_dataPath.toUtf8().data(), key, fileName);   //Shahab
                FILE *fp=getPostingFileHandleForRead(postingFilePath, key, fileName);
                if(fp)
                {
                    do
                    {
                        count=fread(postingFileReadBuffer, RECORD_SIZE, PAGING_COUNT, fp)*RECORD_SIZE;
                        for(uint i =0; i<count; i+=RECORD_SIZE)
                        {
                            UUID Hexuuid(postingFileReadBuffer+i);
                            results.append(Hexuuid);
                        }
                    }while(count);
                }

                /*
                int postingFileFd;
                struct stat postingFileStatBuffer;
                int readBytes = 0;
                unsigned char postingFileReadBuffer[PAGING_COUNT*RECORD_SIZE];
                unsigned char postingFileChunkBuffer[RECORD_SIZE];
                char postingFileIsEof = 0;

                if( (postingFileFd = openFileV2(postingFilePath, O_RDONLY | O_NOATIME)) > -1 )
                {
                    fstat(postingFileFd, &postingFileStatBuffer);
                    int currentPage=1;
                    //fprintf(stderr, "file size: %ld\n", postingFileStatBuffer.st_size/16);
                    while(!postingFileIsEof)
                    {
                        //fprintf(stderr, "page numebr: %d\n", currentPage);
                        readBytes =  readPostingFile(postingFileFd, postingFileStatBuffer.st_size/16, postingFileReadBuffer, &postingFileIsEof, currentPage);
                        if(readBytes<0)
                            break;
                        for(int i =0; i<PAGING_COUNT; i++)
                        {
                            memcpy(postingFileChunkBuffer, postingFileReadBuffer+(i*RECORD_SIZE), 16);
                            //binToHexStr(postingFileChunkBuffer, &binToHexBuffer);
                            //printf("[%d]: %s\n", i, binToHexBuffer);
                            UUID Hexuuid(postingFileChunkBuffer);
                            results.append(Hexuuid);
                        }
                    }
                }
                closePostingFd(postingFileFd);*/
            }
        }
    }
    //cout<<"Execute Time: "<<timer.nsecsElapsed()<<" ns, record count:"<<results.count()<<endl;
    return results;
}

QList<UUID> Part::query(const QString query)
{

}

//Start Mr. MahmoudiNik
//Continue by Aldaghi(Add unsigned char *data to reuslt)
bool Part::isKeyExist(const char *key_value, void *outpuData)
{
    char key[KEY_LEN]={0};
    char value[KEY_LEN]={0};
    GeneralUtils::extractKeyValue(key_value, key, value);
    return isKeyExist(key, value, outpuData);
}

bool Part::isKeyExist(const char *key, const char *value, void *outpuData)
{
    //This method checks if a "key-value" exists in tree or not

    gist *myGist=gists[key];
    bt_query_t q(bt_query_t::bt_eq, (void*)value);
    gist_cursor_t cursor;
    if(myGist->fetch_init(cursor, &q)!=RCOK)
    {
        cerr << "Can't initialize cursor." << endl;
        return false;
    }
    bool eof = false;
    smsize_t keysz=KEY_LEN, datasz=DATA_LEN;
    char keyFound[KEY_LEN]={0};
    bool find=false;
    while (!eof)
    {
        if(myGist->fetch(cursor, (void *)&keyFound, keysz, outpuData, datasz, eof)!=RCOK)
        {
            cerr << "Can't fetch from cursor." << endl;
            find=false;
        }
        if (!eof)
            find=true;
    }
    return find;
}

QStringList Part::getAllTreeNames()
{

#ifdef linux
    QDir dir("data/", "*.db");
#elif _WIN32
    QDir dir("data\\", "*.db");
#endif

    QStringList lstTrees;
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
//QString keyName = fileInfo.fileName().split(".")[0];
        lstTrees.append(fileInfo.fileName().split(".")[0]);
    }
    return lstTrees;
}

void Part::findKeyVectorDriver(const char * key_value, std::vector<UUID>& resOut)
{
    char key[KEY_LEN]={0};
    char value[KEY_LEN]={0};
    GeneralUtils::extractKeyValue(key_value, key, value);
    findKeyVector(key, value, resOut);
}

void Part::findKeyVector(const char * key, const char * value, std::vector<UUID>& results)
{

    // std::vector<UUID> results;
    gist *myGist=gists[key];

    bt_query_t q(bt_query_t::bt_eq, (void*)value);
    gist_cursor_t cursor;
    if(myGist->fetch_init(cursor, &q)!=RCOK)
    {
        cerr << "Can't initialize cursor." << endl;
        return; //results;
    }
    bool eof = false;
    smsize_t keysz=KEY_LEN, datasz=DATA_LEN;

    char keyFound[KEY_LEN]={0};
    char data[DATA_LEN]={0};
    unsigned int count=0;
    while (!eof)
    {
        if(myGist->fetch(cursor, (void *)&keyFound, keysz, (void *)&data, datasz, eof)!=RCOK)
        {
            cerr << "Can't fetch from cursor." << endl;
            return; //results;
        }
        if (!eof)
        {
            // cout << "keyFound:" << key<<":"<<keyFound<<endl;

            memcpy(&count, data, 4);
            for(size_t i=0;i<count && i< 10;i++)
            {
                unsigned char bin_id[ID_LEN]={0};
                memcpy(bin_id, data+(4+i*ID_LEN), ID_LEN);
                //binToHexStr(uuid, id);
                UUID uuid(bin_id);
                results.push_back(uuid);
            }
            if(count==11)//go to postingFile
            {
                //---------------------------------------------|shahab|----------------------------------------------------
                char postingFilePath[255];
                unsigned char postingFileReadBuffer[PAGING_COUNT*RECORD_SIZE];
                char fileName[100];
                GeneralUtils::hashFileName(keyFound, fileName);
                snprintf(postingFilePath, 255, "%s%s/%s.dat", m_dataPath.toUtf8().data(), key, fileName);   //Shahab
                FILE *fp=getPostingFileHandleForRead(postingFilePath, key, fileName);
                if(fp)
                {
                    do
                    {
                        count=fread(postingFileReadBuffer, RECORD_SIZE, PAGING_COUNT, fp)*RECORD_SIZE;
                        for(uint i =0; i<count; i+=RECORD_SIZE)
                        {
                            UUID Hexuuid(postingFileReadBuffer+i);
                            results.push_back(Hexuuid);
                        }
                    }while(count);
                }
            }
        }
    }
    //cout<<"Execute Time: "<<timer.nsecsElapsed()<<" ns, record count:"<<results.count()<<endl;
    return; //results;
}