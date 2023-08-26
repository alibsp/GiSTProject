#include "../headers/include_handler.hpp"

FILE* Part::openFile(const char* fileName, const char* mode)
{
    FILE* fp = fopen(fileName, mode);
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    return fp;
}

int Part::openFileV2(const char* pathname, int flags)   //shahab
{
    // O_RDONLY | O_NOATIME == 00 | 01000000 --> in octal
    return open(pathname, flags);   //Less than zero means error!
}

int Part::readPostingFile(int fd, int countOfDataInside, unsigned char* readBuffer, char* isEof, int &currentPage)    //shahab
{
    //--------->NOTE: Never use SEEK_CUR if you've used read()! Reading forwards file offset!!!!
    //static int currentPage = 1;
    int numRead = 0;
    //int fdCourser = lseek(fd, 0, SEEK_CUR);

    //lseek(fd, PAGING_COUNT*RECORD_SIZE, SEEK_CUR );
    if( (currentPage*PAGING_COUNT) > countOfDataInside )
    {
        const int remainingRecords = ( countOfDataInside-( (currentPage-1)*PAGING_COUNT ) );  //Why if we don't store this expression in a variable, it evaluates wrong when setting '\0' for readBuffer??
        if(!(*isEof))
        {
            if( (numRead = read(fd, readBuffer, remainingRecords*RECORD_SIZE)) > 0 )
            {
                *isEof = 1;
                currentPage++;
                *(readBuffer + ( remainingRecords*RECORD_SIZE) ) = '\0';
                return numRead;
            }
        }
        else
        {
            return -1;
            //You are ignoring EOF!
        }
    }
    else if( (numRead = read(fd, readBuffer, PAGING_COUNT*RECORD_SIZE)) > 0 )
    {
        //fdCourser = lseek(fd, PAGING_COUNT*RECORD_SIZE, SEEK_CUR );
        currentPage++;
        return numRead;
    }

    return -2;
    //Supressing error! Undefiend behavior happened!
}

int Part::closePostingFd(int fd)  //Shahab
{
    //currentPage = 0;
    return close(fd);
}

QStringList Part::findFiles(const QString &startDir, const QStringList &filters)
{
    QStringList names;
    QDir dir(startDir);

    const auto files = dir.entryList(filters, QDir::Files);
    for (const QString &file : files)
        names += startDir + '/' + file;

    const auto subdirs =  dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const QString &subdir : subdirs)
        names += findFiles(startDir + '/' + subdir, filters);
    return names;
}

FILE *Part::getPostingFileHandle(const char *path, const char *keyname, const char *filename, unsigned char mode)
{
    QList<FileStateManager> &postingFiles=postingFilesForWrite;
    if(mode==0)//for read else for write
        postingFiles = postingFilesForRead;

    struct timespec currentNano;
    clock_gettime(CLOCK_MONOTONIC,&currentNano);
    //aldaghi
    FILE* file = nullptr;
    unsigned long min=1000000000;
    int i=0, minIndex=0;
    for (auto &dupFileInfo:postingFiles)
    {
        if(currentNano.tv_nsec - dupFileInfo.accessTime > 1000000 && dupFileInfo.accessCount)
            dupFileInfo.accessCount--;
        if(!file && strncmp(dupFileInfo.keyName, keyname, KEY_LEN)==0 && strncmp(dupFileInfo.fileName, filename, KEY_LEN)==0)
        {
            file = dupFileInfo.fp;
            dupFileInfo.accessCount+=100;
            dupFileInfo.accessTime=currentNano.tv_nsec;
        }
        if(dupFileInfo.accessCount<min)
        {
            min = dupFileInfo.accessCount;
            minIndex=i;
        }
        i++;
    }

    if(file==nullptr)
    {
        if(postingFiles.size()>=OPEN_FILE_LIMIT)
            fclose(postingFiles[minIndex].fp);

        if(mode==0)
            file = openFile(path, "rb");
        else
            file = openFile(path, "ab");
        // Mahmoud
        //******************************************************
        FileStateManager stm;
        strncpy(stm.keyName, keyname, KEY_LEN);
        strncpy(stm.fileName, filename, KEY_LEN);
        stm.accessCount=100;
        stm.fp = file;
        clock_gettime(CLOCK_MONOTONIC, &currentNano);
        stm.accessTime=currentNano.tv_nsec;
        if(postingFiles.size()<OPEN_FILE_LIMIT)
            postingFiles.append(stm);
        else
            postingFiles[minIndex]=stm;
        //******************************************************
    }
    return file;
}


FILE * Part::getPostingFileHandleForWrite(const char *path, const char *keyname, const char *filename)
{
    return getPostingFileHandle(path, keyname, filename, 1);
}

FILE *Part::getPostingFileHandleForRead(const char *path, const char *keyname, const char *filename)
{
    return getPostingFileHandle(path, keyname, filename, 0);
}
