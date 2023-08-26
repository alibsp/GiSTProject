#include "../headers/include_handler.hpp"
#include "../headers/part_class.hpp"

char Part::compareBins(unsigned char* first, unsigned char* second) //shahab
{
    //char notEqual = 0;
    for (char i = 0; i < 16; i++)   //equal
    {
        if( (first[i] ^ second[i]) == 0 ) { continue;}
        else
        {
            if( (first[i] - second[i]) < 0 ) {return -1;}   //Second is bigger
            return 1;   //They weren't equal, nor second was bigger! So first must be bigger!
        }
    }
    return 0;   //They were equal
}
