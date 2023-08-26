#ifndef TYPES_HPP
#define TYPES_HPP

#include "configs.hpp"

struct UUID
{
    //public:
    UUID(const unsigned char * _val)
    {
        memcpy(val, _val, ID_LEN);
    }
    UUID(const UUID &other)
    {
        memcpy(val, other.val, ID_LEN);
    }
    unsigned char val[ID_LEN];
    //friend  qhash_result_t qHash(const UUID &c, qhash_result_t seed) noexcept;
};

//Mahmoud
struct FileStateManager
{
    //FileStateManager(const char* _path){
    //memcpy(path, _path, sizeof(_path));
    FileStateManager()
    {
        accessTime=0;
        accessCount=0;
    }
    FILE* fp;
    char keyName[KEY_LEN];
    char fileName[KEY_LEN];
    long accessTime;
    unsigned int accessCount;
};

#endif // TYPES_HPP
