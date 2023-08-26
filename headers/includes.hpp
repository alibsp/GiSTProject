#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#include <fcntl.h>      //for open(2)   //Shahab
#include <unistd.h>     //for lseek(2)  //Shahab   //Unnecessary
#include <sys/stat.h>   //For stat(2)   //Shahab
#include <QObject>
#include <QString>
#include <QElapsedTimer>
#include <QMap>
#include "../core_GiST/gist.h"
#include "../extension_implementation/core_BTree/gist_btree.h"
#include "../core_GiST/gist_cursor.h"


//Came directly from old part.cpp
#include <QDir>
#include <QFileInfoList>
#include <sys/types.h>
#include <sys/stat.h>
#include <QCryptographicHash>
#include <unistd.h>
#include <string.h>

#endif // INCLUDES_HPP
