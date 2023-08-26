#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H
#include "../headers/include_handler.hpp"

class GeneralUtils
{
public:
    GeneralUtils(){}
    static void checkAndRemoveIlligalChar(char *str, char c);
    static void hashFileName(char *fileName, char *output);
    static void hexStrToBin(const char* uuid, unsigned char* bins);   //aldaghi
    static void binToHexStr(const unsigned char* bins, char* out); //shahab
    static void extractKeyValue(const char *term, char *key, char *value);

};

#endif // GENERAL_UTILS_H
