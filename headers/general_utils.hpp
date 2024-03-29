#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H
#include "../headers/includes.hpp"

class GeneralUtils
{
public:
    GeneralUtils(){}
    static void checkAndRemoveIlligalChar(char *str, char c);
    static void hashFileName(char *fileName, char *output);
    static void hexStrToBin(const char* uuid, unsigned char* bins);   //aldaghi
    static void binToHexStr(const unsigned char* bins, char* out); //Shahab
    static void extractKeyValue(const char *term, char *key, char *value);
    static void regexBuilder(std::string& userRegex);   //Shahab

};

#endif // GENERAL_UTILS_H
