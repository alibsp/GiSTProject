#include "../../headers/include_handler.hpp"
#include "../../headers/part_class.hpp"

void Part::insertRecord(const char *id, const char *keys)
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    char myId[37]={0};
    strncpy(myId, id, 36);
    if(insertId(myId))
    {
        int i=0;
        char term[KEY_LEN]={0};
        for (int j=0; ; i++)
        {
            if(keys[i]==',' || (keys[i]=='}' &&  keys[i+1]==0) || keys[i]==0 || j>=KEY_LEN)
            {
                term[j] = 0;
                if(j)
                    insertTerm(myId, term);
                j=0;
            }
            else if((i==0&&keys[i]!='{') || i)
                term[j++]=keys[i];
            if( keys[i]==0 || j>=KEY_LEN)
                break;
        }
    }
    cout<<"insertRecord finish at "<<elapsedTimer.nsecsElapsed()<<endl;
}

void Part::insertTerm(const char *id, const char *term)
{
    unsigned char binaryUUID[ID_LEN];  //shahab: I think we don't need to worry about allocating explict memory range for this, as it'll be assigned to another memory location later;
    GeneralUtils::hexStrToBin(id, binaryUUID);

    unsigned char data[DATA_LEN]={0};



    char treeName[KEY_LEN]={0};
    char treeKey[KEY_LEN]={0};
    GeneralUtils::extractKeyValue(term, treeName, treeKey);

    //strcat(value, ":"); //shahab:duplicate error fix
    //strcat(value, id);  //shahab:duplicate error fix

    if(treeKey[0]==0)
    {
        strncpy(treeKey , treeName, KEY_LEN);
        strncpy(treeName, "nonekey", KEY_LEN);
    }
    char path[257];
    char path_data_folder[257];

    strncpy(path, m_dataPath.toUtf8().data(), 256);
    strncpy(path_data_folder, m_dataPath.toUtf8().data(), 256);

    strcat(path, treeName);
    strcat(path_data_folder, treeName);

    strcat(path, ".db");
    //ایجاد پایگاه داده با افزونه BTree
    gist *myGist  =gists[treeName];
    if(myGist==nullptr)
    {
        myGist = new gist();
        gists[treeName] = myGist;

        if(myGist->create(path, &bt_str_key_binary_data_ext)!=RCOK)
            if(myGist->open(path)!=RCOK)
            {
                cerr << "Can't Open File." << endl;
                return ;
            }
    }
    //qDebug()<< id << key << value;
    //std::cout <<id <<" "<< key  <<" "<< value<<endl;
    //myGist
    //if -> bloom check (term)  -->0 --1>
    //TODO: Consider adding bloom filter
    unsigned int count=0;



    isKeyExist(treeName, treeKey, (void *)data);

    memcpy(&count, data, 4);
    count++;
    if(count<=10)
    {
        memcpy(data, &count, 4);
        memcpy(data+(4+(count-1)*ID_LEN), binaryUUID, ID_LEN);
        if(count>1)
        {
            bt_query_t q(bt_query_t::bt_eq, (void *)treeKey);
            myGist->remove(&q);
        }
        myGist->insert((void *) &treeKey, KEY_LEN, (void *) data, DATA_LEN);
        myGist->flush();
    }
    else //count>10; postingTree
    {
        //Directory name: treeName
        //File name     : treeKey
        //key in tree   : binaryUUID
        //type of tree  : bt_binary_key_ext
        //mahmoud
        //1.Check if directory not exists, then create directory
        struct stat st;
        if (stat(path_data_folder, &st) == -1)
            mkdir(path_data_folder, 0700);

        char fileName[100];
        GeneralUtils::hashFileName(treeKey, fileName);

        //Remove illigal '/' char from string so linux systems can accept our file names.
        //Or we can replace it with specialChar
        //And another limitation(in linux  and common filesystems) is maximum length for filename is 255(byte) and 4096 for directory+filename length
        // checkAndRemoveIlligalChar(treeKey,'/');//Mahmoud

        //2.Create sub Tree of binaryUUID's:
        //2.1.make name of tree file
        strcat(path_data_folder, "/");
        strcat(path_data_folder, fileName);
        strcat(path_data_folder, ".dat");

        FILE *file = getPostingFileHandleForWrite(path_data_folder, treeName, fileName);
        if(ftell(file)==0)
        {
            memcpy(data, &count, 4);
            if(count>1)
            {
                bt_query_t q(bt_query_t::bt_eq, (void *)treeKey);
                myGist->remove(&q);
            }
            myGist->insert((void *) &treeKey, KEY_LEN, (void *) data, DATA_LEN);
            myGist->flush();
        }
        fwrite(binaryUUID, ID_LEN, 1, file);
        //fclose(file);
    }
    //TODO: add code to create new posting-tree if key exsists!
    //myGist->close();
}

bool Part::insertId(const char *id)
{
    unsigned char binaryUUID[ID_LEN];  //shahab: I think we don't need to worry about allocating explict memory range for this, as it'll be assigned to another memory location later;
    GeneralUtils::hexStrToBin(id, binaryUUID);

    char path[257];
    strncpy(path, m_dataPath.toUtf8().data(), 256);
    strncat(path, "QGiSTId.db", 256);


    gist *myGist  =gists["QGiSTId"];
    if(myGist==nullptr)
    {
        myGist = new gist();
        gists["QGiSTId"] = myGist;
        if(myGist->create(path, &bt_binary_key_ext)!=RCOK)
        {
            if(myGist->open(path)!=RCOK)
            {
                cerr << "Can't Oepn File." << endl;
                return false;
            }
        }
    }
    bt_query_t q(bt_query_t::bt_eq, &binaryUUID);
    gist_cursor_t cursor;
    if(myGist->fetch_init(cursor, &q)!=RCOK)
    {
        cerr << "Can't initialize cursor." << endl;
        return false;   //shahab
    }
    bool eof = false;

    char data[2]="A";
    smsize_t keysz=ID_LEN, datasz=2;
    char key[ID_LEN]={0};
    while (!eof)
    {
        if(myGist->fetch(cursor, (void *)&key, keysz, (void *)&data, datasz, eof)!=RCOK)
        {
            cerr << "Can't fetch from cursor." << endl;
            return false;   //shahab
        }
        if (!eof)
        {
            cout<<"id:"<<(char*)&key<<"->"<<data<<endl;
            //myGist->close();
            return false;
        }// process key and data...
    }
    strcpy(data, "A");
    myGist->insert((void *) &binaryUUID, ID_LEN, (void *) &data, datasz);
    myGist->flush();

    //myGist->close();
    return true;
}
