#ifndef CONFIGS_HPP
#define CONFIGS_HPP

#define PAGING_COUNT 512 //Shahab    //For readPostingFile()
#define RECORD_SIZE 16  //Shahab    //For readPostingFile
#define ID_LEN 16       //Mr. Aladaghi
#define DATA_LEN 164    //4+16*10   //Mr. Aladaghi
#define KEY_LEN 101     //Mr. Aladaghi
#define BUCKET_SIZE 10 //Mr.mahmoudi??    //Used in findkey() if(count == 10) section
#define OPEN_FILE_LIMIT 200 //Mr. Aladaghi
#define CSV_FILE_PATH "/home/shahabseddigh/Desktop/data2.csv"
#define TREE_DATA_FOLDER "data/"

#endif // CONFIGS_HPP
