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

//Shall be moved to somewhere appropriate?
struct
{
    bool operator() (const UUID & a, const UUID & b) const
    {
        uint64_t h1=0, h2=0, l1=0, l2=0;

        memcpy(&l1, a.val, 8);
        memcpy(&l2, b.val, 8);

        memcpy(&h1, a.val+8, 8);
        memcpy(&h2, b.val+8, 8);

        //printf("%s:(%lu %lu) %s:(%lu %lu)\n", out1, h1, l1, out2, h2, l2);

        if(h1>h2)       return false;
        else if(h1<h2)  return true;
        else if(h1==h2)
        {
            if(l1<l2)
                return true;

            return false;
        }
        return false;
    }
}uuidLess;

struct
{
    bool operator() (const UUID & a, const UUID & b) const
    {
        uint64_t h1=0, h2=0, l1=0, l2=0;

        char output1[37];
        char output2[37];
        GeneralUtils::binToHexStr(a.val, output1);
        GeneralUtils::binToHexStr(b.val, output2);
        printf("%s %s\n", output1, output2);


        memcpy(&l1, a.val, 8);
        memcpy(&l2, b.val, 8);

        memcpy(&h1, a.val+8, 8);
        memcpy(&h2, b.val+8, 8);

        //printf("%s:(%lu %lu) %s:(%lu %lu)\n", out1, h1, l1, out2, h2, l2);

        return ((h1 ^ h2)==0 && (l1 ^l2)==0) ;
    }
}uuidEqual;

int uuid_cmp(const UUID & a, const UUID & b)
{
    uint64_t h1=0, h2=0, l1=0, l2=0;

    //binToHexStr(a, out1);
    //binToHexStr(b, out2);


    memcpy(&l1, a.val, 8);
    memcpy(&l2, b.val, 8);

    memcpy(&h1, a.val+8, 8);
    memcpy(&h2, b.val+8, 8);

    //printf("%s:(%lu %lu) %s:(%lu %lu)\n", out1, h1, l1, out2, h2, l2);

    if(h1>h2)       return 1;
    else if(h1<h2)  return -1;
    else if((h1^h2)==0)
    {
        if(l1<l2)
            return -1;
        else if(l1>l2)
            return 1;
    }
    return 0;
}
//-->END<-- Shall be moved to somewhere appropriate?

void Part::vect_intersect(const std::vector<UUID> &list1, const std::vector<UUID> &list2, std::vector<UUID> &result)
{
    //std::vector<UUID> result;
    for(int i=0, j=0;i<list1.size() && j<list2.size(); )
    {
        if(uuid_cmp(list1[i], list2[j])==0)
        {
            result.push_back(list1[i]);
            i++, j++;
        }
        else if(uuid_cmp(list1[i], list2[j])<0)
            i++;
        else if(uuid_cmp(list1[i], list2[j])>0)
            j++;
    }
    //return result;
}

void Part::vect_union(const std::vector<UUID> &list1, const std::vector<UUID> &list2, std::vector<UUID> &result)
{
    //std::vector<UUID> result;
    int i=0, j=0;
    while(i<list1.size() && j<list2.size())
    {
        if(uuid_cmp(list1[i], list2[j])==0)
        {
            result.push_back(list1[i]);
            i++, j++;
        }
        else if(uuid_cmp(list1[i], list2[j])<0)
        {
            result.push_back(list1[i]);
            i++;
        }
        else if(uuid_cmp(list1[i], list2[j])>0)
        {
            result.push_back(list2[j]);
            j++;
        }
    }
    for(;i<list1.size() ; i++)
        result.push_back(list1[i]);
    for(;j<list2.size() ; j++)
        result.push_back(list2[j]);
    //return result;
}

void Part::uuid_intersect(vector<UUID> &vec_a, vector<UUID> &vec_b, std::vector<UUID>& res_intersect)
{
    //vector<UUID> res_intersect;// Result vector

    // STEPS:

    // 1.Sort both Vectors
    std::sort(vec_a.begin(), vec_a.end(), uuidLess); // uuidLess:is comparing method
    std::sort(vec_b.begin(), vec_b.end(), uuidLess);

    // 2.intersection
    vect_intersect(vec_a, vec_b, res_intersect);// Use custom implemented intersection


    // std intersect
    //set_intersection(vec_a.begin(), vec_a.end(),  vec_b.begin(), vec_b.end(),std::back_inserter(res_intersect),uuidEqual);

    //return res_intersect;
}

void Part::uuid_union(vector<UUID> &vec_a, vector<UUID> &vec_b, std::vector<UUID>& res_union)
{
    //vector<UUID> res_union;// Result vector

    // STEPS:
    // 1.Sort both Vectors
    std::sort(vec_a.begin(), vec_a.end(), uuidLess); // uuidLess:is comparing method
    std::sort(vec_b.begin(), vec_b.end(), uuidLess);

    // 2.uinin
    vect_union(vec_a, vec_b,res_union);

    //return res_union;
}
