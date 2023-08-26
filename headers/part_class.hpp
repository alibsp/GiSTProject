#ifndef PART_CLASS_HPP
#define PART_CLASS_HPP

#include "includes.hpp"
#include "types.hpp"
class Part : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dataPath READ dataPath WRITE setDataPath NOTIFY dataPathChanged)

public:
    explicit Part(QString dataPath, QObject *parent = nullptr);
    void insertRecord(const char * id, const char *keys);

    QList<UUID> findKey(const char *key_value);
    QList<UUID> findKey(const char * key, const char * value);

    QList<UUID> query(const QString query);


    void printAllKeys(QString treeName);    //Mr. Aladaghi + shahab
    bool isKeyExist(const char *key, const char *value, void * outpuData);   //Mr. Mahmoudi
    bool isKeyExist(const char *key_value, void *outpuData); //Mr. Aladaghi + Mr. Mahmoudi
    void importCSV(QString filePath);
    void loadGists();
    void dropGists();
    void testInserts();
    FILE *openFile(const char *fileName, const char *mode);
    void checkAndRemoveIlligalChar(char* str, char c);//mahmoud

    void hashFileName(char *fileName, char *output);
    //-------------------------------------------------------------------------------|Shahab|--------------------------------------------------------------------------------------------
    void hexStrToBin(const char* uuid, unsigned char *bins);   //shahab
    void binToHexStr(const unsigned char* bins, char* out); //shahab
    char compareBins(unsigned char* first, unsigned char* second);  //Shahab
    int openFileV2(const char* pathname, int flags);   //shahab
    int readPostingFile(int fd, int countOfDataInside, unsigned char* readBuffer, char* isEof, int &currentPage);    //shahab
    int closePostingFd(int fd);  //Shahab
    //-----------------------------------------------------------------------------|END Shahab|------------------------------------------------------------------------------------------

    QString dataPath() const;

public slots:
    void setDataPath(QString dataPath);

signals:
    void dataPathChanged(QString dataPath);

private:
    QMap<QString, gist*> gists;
    //QMap<QString, FILE*> files;
    QList<FileStateManager> postingFilesForRead;
    QList<FileStateManager> postingFilesForWrite;

    void insertTerm(const char *id, const char *term);
    bool insertId(const char *id);
    //void extractKeyValue(const char *term, char *key, char *value);
    QStringList findFiles(const QString &startDir, const QStringList &filters);

    FILE *getPostingFileHandle(const char *path, const char *keyname, const char *filename, unsigned char mode);
    FILE *getPostingFileHandleForWrite(const char *path, const char *keyname, const char *filename);
    FILE *getPostingFileHandleForRead(const char *path, const char *keyname, const char *filename);
    QString m_dataPath;
};


#endif // PART_CLASS_HPP
