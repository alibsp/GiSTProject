#include "../headers/general_utils.h"

void GeneralUtils::checkAndRemoveIlligalChar(char *str, char c)
{
    if(strchr(str, c) != NULL){
        char *pr = str, *pw = str;
        while (*pr) {
            *pw = *pr++;
            pw += (*pw != c);
        }
        *pw = '\0';
    }
}

void GeneralUtils::hashFileName(char *fileName, char *output)
{
    strncpy(output, QCryptographicHash::hash(fileName, QCryptographicHash::Md5).toHex().data(), 100);
}

void GeneralUtils::hexStrToBin(const char* uuid, unsigned char* bins)   //aldaghi
{
    unsigned char i=0, byte=0, res=0;
    //unsigned char* bins = (unsigned char*)malloc( 16*sizeof(char) );    //Always assign a new, free memory to this pointer! As the reference will be copied to the *out parameter of the very first call to this function and thus, never will be deleted after going out of scope!
    while(uuid[i]!=0)
    {
        if(uuid[i]=='-') {i++; continue;}

        if(uuid[i] > 47 && uuid[i] < 58) {res = (uuid[i]-48)*16;}
        else if(uuid[i] > 96 && uuid[i] < 103) {res = (uuid[i]-87)*16;}

        if(uuid[i+1] > 47 && uuid[i+1] < 58) {res += (uuid[i+1]-48);}
        else if(uuid[i+1] > 96 && uuid[i+1] < 103) {res += (uuid[i+1]-87);}

        bins[byte] = res;
        byte++; i+=2;
    }

    //*out = bins;
    //We cannot free bins here as the memory address now being used by *out !
}

void GeneralUtils::binToHexStr(const unsigned char* bins, char* out) //shahab
{
    //char output[37];
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    for( int i = 0, j = 0; i < 16; i++ )
    {
        char const byte = bins[i];

        (out)[j] = hex_chars[ ( byte & 0xF0 ) >> 4 ];
        (out)[j+1] = hex_chars[ ( byte & 0x0F ) ];
        j+=2;
        if( i==3 || i==5 || i==7 || i==9 ) {(out)[j] = '-'; j++;}
    }
    (out)[36] = 0;
    //printf("%s\n\n", (*out));
}

//term = "source_gitlab\0"
void GeneralUtils::extractKeyValue(const char *term, char *key, char *value)
{
    for (int i=0, sw=0, j=0;;i++)
    {
        if(sw==0)
            key[j++]=term[i];
        if(sw==1)
            value[j++]=term[i];
        if(term[i]=='_')
        {
            if(sw==0)
            {
                key[j-1]=0;
                j=0;
            }
            sw=1;
        }
        if(!term[i])
            break;
    }
}
