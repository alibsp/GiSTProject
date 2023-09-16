#include <QCoreApplication>
#include <QDebug>
#include "headers/part_class.hpp"

#include <iostream>
#include <limits>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>


#pragma region shahab_interface

std::queue<std::string> shuntingYard(const std::string &exp)
{
    #pragma region shuntingYard_variables
    std::queue<std::string> resQueue;
    std::stack<std::string> opsVectorStack;
    std::string resString;
    std::string charToStr;
    #pragma endregion

    #pragma region shuntingYard_main_loop
    int i = 0;
    while (i < exp.length())
    {
        if (exp[i] == '(')
        {
            opsVectorStack.push((charToStr = exp[i]));
            i++;
        }
        else if (exp[i] == ')')
        {
            if (!resString.empty())
            {
                resQueue.push(resString);
                resString.clear();
            }
            while ((opsVectorStack.top() != "(") && (!opsVectorStack.empty()))
            {
                resQueue.push(opsVectorStack.top());
                opsVectorStack.pop();
            }
            if ((opsVectorStack.top() == "("))
            {
                opsVectorStack.pop();
                i++;
            }
        }
        else if ( (exp[i] >= 39 && exp[i] <= 122) || (exp[i] >= 33 && exp[i] <= 37) )
        {
            resString += exp[i];
            i++;
        }
        else if (exp[i] == '&' && exp[i + 1] == '&')
        {
            if (!resString.empty())
            {
                resQueue.push(resString);
                resString.clear();
            }
            charToStr = exp[i];
            charToStr += exp[i + 1];
            opsVectorStack.push(charToStr);
            i += 2;
        }
        else if (exp[i] == '|' && exp[i + 1] == '|')
        {
            if (!resString.empty())
            {
                resQueue.push(resString);
                resString.clear();
            }
            //size_t j = opsVectorStack.size();
            //while( j>0 )
            while (true)
            {
                if ((opsVectorStack.top() == "&&") || (opsVectorStack.top() == "||"))
                {
                    opsVectorStack.pop();
                    continue;
                }
                else
                {
                    break;
                }
            }
            charToStr = exp[i];
            charToStr += exp[i + 1];
            opsVectorStack.push(charToStr);
            i += 2;
        }
    }
    #pragma endregion

    #pragma region shuntingYard_remaining_items_loop
    while (!opsVectorStack.empty())
    {
        resQueue.push(opsVectorStack.top());
        opsVectorStack.pop();
    }

    std::cerr << "ShuntingYard is done!" << std::endl;
    return resQueue;
    #pragma endregion
}

void queryExecuter(std::queue<std::string> resQueue, Part &part, std::vector<UUID> &searchResult)
{
    std::stack<std::vector<UUID>> parseStack;

    while (!resQueue.empty())
    {
        if (((resQueue.front() != "&&") && (resQueue.front() != "||")))
        {
            std::vector<UUID> findKeyResult;
            //if statement is newly added and hasn't been FULLY tested yet!   //Shahab
            if( (resQueue.front().find('%') != std::string::npos) )
            {
                part.findKeyVectorWithRegexDriver(resQueue.front(), findKeyResult);
            }
            else
            {
                part.findKeyVectorDriver(resQueue.front().c_str(), findKeyResult);
            }
            parseStack.push(findKeyResult);
            resQueue.pop();
        }
        else
        {
            std::vector<UUID> firstArg, secondArg, operationRes;

            secondArg = parseStack.top();
            parseStack.pop();

            firstArg = parseStack.top();
            parseStack.pop();

            if (resQueue.front() == "&&")
            {
                part.uuid_intersect(firstArg, secondArg, operationRes);
                parseStack.push(operationRes);
                resQueue.pop();
            }
            else
            {
                part.uuid_union(firstArg, secondArg, operationRes);
                parseStack.push(operationRes);
                resQueue.pop();
            }
        }
    }
    searchResult = parseStack.top();
}

#pragma endregion


void myPrintPredFct(
        std::ostream &s, // what to print to
        const vec_t &pred, // pred.ptr(0) contains pointer to 8-byte aligned predicate
        int level)
{

}

void myPrintDataFct(std::ostream &s, // what to print to
                    const vec_t &data)
{

}


int intTest()
{
    gist myGist;
    //ایجاد پایگاه داده با افزونه BTree
    if (myGist.create("DataInt.db", &bt_int_ext) == RCOK)
    {

        //تولید اعداد تصادفی برای درج در پایگاه داده
        srand(time(0));
        int lb = -10000000, ub = 10000000;
        for (int i = 0; i < 1000; i++)
        {
            for (int j = 0; j < 10000; j++)
            {
                int key = (rand() % (ub - lb + 1)) + lb;
                int data = (rand() % (ub - lb + 1)) + lb;
                myGist.insert((void *) &key, sizeof(int), (void *) &data, sizeof(int));
            }
            myGist.flush();
        }
    }
    else // File maybe Exist
    {
        if (myGist.open("DataInt.db") != RCOK)
        {
            cerr << "Can't Open File." << endl;
            return 0;
        }
    }

    bt_query_t q(bt_query_t::bt_betw, new int(1200), new int(1500));
    gist_cursor_t cursor;

    if (myGist.fetch_init(cursor, &q) != RCOK)
    {
        cerr << "Can't initialize cursor." << endl;
        return (eERROR);
    }

    bool eof = false;
    int key, data;
    smsize_t keysz = sizeof(int), datasz = sizeof(int);
    while (!eof)
    {
        if (myGist.fetch(cursor, (void *) &key, keysz, (void *) &data, datasz, eof) != RCOK)
        {
            cerr << "Can't fetch from cursor." << endl;
            return (eERROR);
        }
        if (!eof)
        {
            std::cout << key << "->" << data << endl;
        }
        // process key and data...
    }
    return 1;
}

static char *rand_string(char *str, size_t size)
{
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-";

    if (size)
    {
        --size;
        for (size_t n = 0; n < size; n++)
        {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

int stringTest()
{
    gist myGist;
    //ایجاد پایگاه داده با افزونه BTree
    if (myGist.create("DataString.db", &bt_str_ext) == RCOK)
    {

        //تولید رشته های تصادفی برای درج در پایگاه داده
        srand(time(0));
        int lb = 3, ub = 100;

        for (int i = 0, k = 1; i < 10000; i++)
        {
            for (int j = 0; j < 1000; j++)
            {
                char key[100] = {0};
                int len = (rand() % (ub - lb + 1)) + lb;
                rand_string(key, len);
                int data = k++;
                myGist.insert((void *) &key, 100, (void *) &data, sizeof(int));
            }
            //12532000 + 131000 + 118000 + 166000 + 1000 + 100000
            myGist.flush();
            cout << "Insert " << k - 1 << " Record ok" << endl;
        }
    }
    else // File maybe Exist
    {
        if (myGist.open("DataString.db") != RCOK)
        {
            cerr << "Can't Open File." << endl;
            return 0;
        }
    }
    //char my_key[100]="Ali Aldaghi";
    //    int my_data = 1000;
    //    myGist.insert((void *) &my_key, 100, (void *) &my_data, sizeof(int));

    //bt_query_t qRemove(bt_query_t::bt_eq, my_key, my_key);

    //myGist.remove(&qRemove);

    char key1[100] = "Ali";
    char key2[100] = "Alizzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz";

    QElapsedTimer timer;
    timer.start();

    bt_query_t q(bt_query_t::bt_betw, key1, key2);
    //bt_query_t q(bt_query_t::bt_eq, &key1, key2);
    gist_cursor_t cursor;
    if (myGist.fetch_init(cursor, &q) != RCOK)
    {
        cerr << "Can't initialize cursor." << endl;
        return (eERROR);
    }
    bool eof = false;

    int data;
    smsize_t keysz = 100, datasz = sizeof(int);
    char key[100] = {0};
    while (!eof)
    {
        if (myGist.fetch(cursor, (void *) &key, keysz, (void *) &data, datasz, eof) != RCOK)
        {
            cerr << "Can't fetch from cursor." << endl;
            return (eERROR);
        }
        if (!eof)
        {
            cout << (char *) &key << "->" << data << endl;
        }
        // process key and data...
    }
    cout << endl << "Time is:" << timer.elapsed() << endl;
    return 0;
}


int main(int argc, char *argv[])
{
    //intTest();
    //stringTest();
    qDebug() << "start...";

    QString csvFile;
    #ifdef __linux__
    csvFile = "/usr/local/part/data.csv";
    //csvFile = "/media/ali/Data/Programming/Projects/Part/Data/data2.csv";
    //csvFile = "/home/mahmoudmahmoudinik/Data/data2.csv";
    //csvFile = "/home/shahabseddigh/Desktop/data2.csv";
    #elif _WIN32
    csvFile = "D:\\Programming\\Projects\\Part\\Data\\data.csv";
    #endif

    //char aa[100]="   a";
    //char bb[100]="  b";
    //cout<<str_cmp((void*)aa, (void*)bb)<<" "<< strcmp(aa, bb)<<endl;
    //return 0;
    /*bool clean=true;
    Part part("data/");
    GeneralUtils utils;
    if(clean)
    {
        QElapsedTimer timer;
        timer.start();
        part.dropGists();
        qDebug()<<"Drop Gists Time: "<<timer.nsecsElapsed()<<" ns";;
        timer.start();
        part.importCSV(csvFile);
        qDebug()<<"All Insert Time: "<<timer.elapsed()<<" ms";;
    }
    else
        part.loadGists();*/


    #pragma region GiSTProject_interface
    #pragma region interface_variables
    std::string queryString;
    std::vector<std::string> queryVector;
    size_t position;
    Part part("/usr/local/part/data/");
    bool loadTreeByDefault = false;
    #pragma endregion

    #pragma region interface_warning_message
    if(!loadTreeByDefault)
    {
        std::cout << "Tree have not been loaded! Type [load tree;] or [reload tree;] to load them before anything else; or SIGSEGV will happen!" << std::endl;
    }
    else
    {
        std::cout << "Tree has been loaded by default; you can change this behavior via [loadTreeByDefault] variable" << std::endl;
    }
    #pragma endregion

    #pragma region interface_main_loop
    while (true)
    {
        #pragma region interface_read_query
        std::cout << "GiSTProject:~# ";
        std::getline(std::cin, queryString);
        std::cin.clear();
        #pragma endregion

        #pragma region interface_splitting_queries
        while ((position = queryString.find(';')) != std::string::npos)
        {

            int i = 0, startPosition = 0;
            while (true)
            {
                if (queryString[i] == ' ')
                {
                    startPosition++;
                    i++;
                }
                else
                {
                    break;
                }
            }
            queryVector.push_back(queryString.substr(startPosition, position));
            queryString.erase(0, position + 1);
        }
        if (!queryString.empty())
        {
            std::cerr << "There were non-terminated string in your query discarded: >" << queryString << std::endl;
        }
        #pragma endregion

        #pragma region interface_parsing_queries
        size_t queryVectorIterator = 0;
        if (!queryVector.empty())
        {
            while (queryVectorIterator < queryVector.size())
            {
                if (queryVector[queryVectorIterator].substr(0, 9) == "load tree")
                {
                    std::cout << "loading tree" << std::endl;
                    part.loadGists();
                }

                else if (queryVector[queryVectorIterator].substr(0, 11) == "reload tree")//part.importCSV(csvFile);
                {
                    std::cout << "reloading tree" << std::endl;
                    part.dropGists();
                    part.importCSV(csvFile);
                }

                else if (queryVector[queryVectorIterator].substr(0, 9) == "list keys")
                {
                    std::cout << "listing all keys:" << std::endl;
                    int i = 0;
                    for (auto s: part.getAllTreeNames())
                    {
                        i++;
                        qDebug() << i << ":" << s;
                    }
                }

                else if (queryVector[queryVectorIterator].substr(0, 4) == "quit")
                {
                    std::cout << "quiting..." << std::endl;
                    return 0;
                }

                else if (queryVector[queryVectorIterator].substr(0, 7) == "search ")
                {
                    std::cout << "searching..." << std::endl;
                    queryVector[queryVectorIterator].erase(0, 7);
                    if ( (queryVector[queryVectorIterator].find("||") != std::string::npos) || (queryVector[queryVectorIterator].find("&&") != std::string::npos) )
                    {
                        std::vector<UUID> searchResult;
                        queryExecuter(shuntingYard(queryVector[queryVectorIterator]), part, searchResult);

                        char searchOutput[37]{0};
                        std::ofstream searchOutputFile;
                        searchOutputFile.open("searchOutput.bin");
                        for (auto uuid: searchResult)
                        {
                            GeneralUtils::binToHexStr(uuid.val, searchOutput);
                            searchOutputFile << searchOutput << std::endl;
                        }
                        searchOutputFile.close();

                        std::cout << "Searching is done. searchOutput.bin" << std::endl;
                    }
                    else if ( (queryVector[queryVectorIterator].find('(') != std::string::npos) || (queryVector[queryVectorIterator].find(')') != std::string::npos) )
                    {
                        std::cerr << "Search query contains parentheses but [||] or [&&] wasn't specified! Skipping..." << std::endl;
                    }
                    else if ( (queryVector[queryVectorIterator].find('%') != std::string::npos) )
                    {
                        std::cout << "searching using REGEX..." << std::endl;
                        std::vector<UUID> resOut;
                        part.findKeyVectorWithRegexDriver(queryVector[queryVectorIterator], resOut);
                        std::cout << "REGEX is done." << std::endl;
                    }
                    else    //Absolute search term or with single :* (old method)
                    {
                        QList<UUID> res = part.findKey(queryVector[queryVectorIterator].c_str());
                    }
                }

                else
                {
                    std::cerr << "Unrecognized command at position: [" << queryVectorIterator << "]. Command was: " << queryVector[queryVectorIterator] << std::endl;
                }

                queryVectorIterator++;
            }
            queryVector.clear(); //Empty out the vector to avoid redoing commands, if the new queryString is either empty or contains error, which won't be parsed and thus, previous contents remain in the vector.
        }
        #pragma endregion
    }
    #pragma endregion

    #pragma endregion


    #pragma region consoleInterface
    /*std::string cmd;
    Part part("data/");
    GeneralUtils generalUtils;
    bool clean= false;
    //std::string searchPharase=out[1];
    //TEMP
    //std::string searchPharase="";
    while(cmd!="finish"){
        qDebug() << "Enter command:";
        std::getline(std::cin,cmd);
        //Help
        if(startsWith(cmd,"search")){

            const char delim = ' ';
            std::vector<std::string> out;
            tokenize(cmd, delim, out);
            //std::cout << out[1] << '\n';
            if(out.size()>1){
                std::string searchPharase=out[1];
                //TEMP
                //searchPharase="participantUserName_sina.k:*";
                //END TEMP

                //do search
                if(clean)
                {
                    part.dropGists();
                    part.importCSV(csvFile);
                    clean=false;
                }
                else
                    part.loadGists();

                QElapsedTimer timer;
                timer.start();


                QList<UUID> results=part.findKey(searchPharase.c_str());
                //results.append(part.findKey("updatedAt_14010510200905000"));
                //results.append(part.findKey("createdAt_14010511150209000"));


                //QSet<UUID> ids(results.begin(), results.end());
                uint64_t time=timer.nsecsElapsed();
                uint64_t time_ms=timer.nsecsElapsed()/1000000;
                for (UUID &res:results)   //shahab
                {

                    char output[37];
                    generalUtils.binToHexStr(res.val, output);
                    qDebug() << "ID: -->\t" << output;

                }
                qDebug()<<"Execute Time(ns):         "<<time;
                qDebug()<<"Execute Time(ms):         "<<time_ms;
                qDebug()<<"Result record count:  "<<results.count();

                qDebug()<<"finish.";
            }
            else{
                qDebug() << "Please determine a Search Phrase! \n Usage: search SEARCHING_PHRASE";
            }
            //               for (auto &s: out) {
            //                   if(s!=""){
            //                    std::cout << s << '\n';
            //                   }
            //               }
        }
        if(cmd=="clean"){
            std::cout << "Clear existings tree and make whole tree again" << std::endl;
            clean=true;
        }
        else if(cmd=="getKeys"){
            int i=0;
            for(auto s:part.getAllTreeNames()){
                i++;
                qDebug() << i << ":" << s;
            }

        }

    }*/
    #pragma endregion


    #pragma region testRegion
    //QElapsedTimer timer;
    //timer.start();

    //QList<UUID> results1=part.findKey("updatedAt_14010510:*");
    //QList<UUID> results1=part.findKey("participantUsername_reza.mahmoudi");
    //QList<UUID> results1=part.findKey("labels_تخته: دات نت");
    // (
    // )
    // &
    // |
    // >
    // >=
    // <
    // <=
    // <> or !=
    // IN
    //NOT IN
    //CONTAINS
    //@>
    //<@

    // Supporting in future
    // ALI:*
    // *:odui
    //sample result: ali mahmoudi , alireza mahmoudi, aliakbar masoudi,ali shahroudi




//   std::vector<UUID> res1=results1.toVector().toStdVector();

//    QList<UUID> results2=part.findKey("updatedAt_14010512:*");

//   std::vector<UUID> res2=results2.toVector().toStdVector();

//   std::vector<UUID> intersect_result;
//   intersect_result=part.uuid_intersetct(res1,res2);

//   //make prefix
//   //updatedAt=14010510:* & participantUsername_reza.mahmoudi
//   std::string query="updatedAt=14010510:*    & participantUsername_reza.mahmoudi";

//   std::string::iterator new_end = std::unique(query.begin(), query.end(), BothAreSpaces);
//   query.erase(new_end, query.end());

//   const char delim = ' ';
//   std::vector<std::string> out;
//   std::vector<std::string> finalExpretion;
//   tokenize(query, delim, out);



    //void queryParser(std::string exp)



    // CLEAN UP unnessary
    /*

    //query_parser("updatedAt=140105:* & username=ali")


    //QList<UUID> results=part.findKey("updatedAt_14010512:*");

 //    for(int i=0;i< results.count();i++){
 //          char output[37];
 //          part.binToHexStr(results[i].val,output);
 //        qDebug() << i << ":"  << output;
 //    }

    vector<char*> vec_a;
     //PRINT result (not sorted)
    int i=0;
     for (UUID &res:results)
     {
         i++;
         //char output[37];
          char *output=(char *) malloc(37);
         utils.binToHexStr(res.val, output);

         //strncpy(cpy, output,37);
         //memcpy(cpy,output, sizeof(cpy));
         qDebug() << i << ": -->\t" << output;
         vec_a.push_back(output);
        // free(output);

     }
    //vector<unsigned char* > vec_a=results.toVector().toStdVector();
     //sort(vec_a.begin(),vec_a.end(),compare_bigger);



    std::sort(vec_a.begin(), vec_a.end(),comparisonFunc);
 //    //PRINT sroted
 //     qDebug() << "Sorted:" ;
 //    i=0;
 //    for (auto &res:vec_a)
 //    {
 //        i++;
 //        //char output[37];
 //        //utils.binToHexStr(res, output);
 //        qDebug() << i << ": -->\t" << res;
 //    }


     QList<UUID> results2=part.findKey("updatedAt_14010512:*");
     vector<char*> vec_b;
      //PRINT result (not sorted)

     i=0;
      for (UUID &res:results2)
      {
          i++;
          //char output[37];
           char *output=(char *) malloc(37);
          utils.binToHexStr(res.val, output);
          qDebug() << i << ": -->\t" << output;
          vec_b.push_back(output);

     }

      std::sort(vec_b.begin(), vec_b.end(),comparisonFunc);
   //    //PRINT sroted

       qDebug() << "=============================================";
       vector<char *> intersect_result;
       set_intersection(vec_a.begin(),vec_a.end(),vec_b.begin(),vec_b.end(),back_inserter(intersect_result), comparisonFuncEQuality);

       qDebug() << "interSection result:" ;
        i=0;
        for (auto &res:intersect_result)
        {
            i++;
            qDebug() << i << ": -->\t" << res;
        }

       //vector<char*> uuid_intersetct(vector<char*> vec_a,vector<char*> vec_b);

       //part.uuid_intersetct()
     //vector<UUID> r= part.uuid_union(results.toVector().toStdVector(),results.toVector().toStdVector());
     //qDebug() << "after union:"
   //  QList<UUID> results=part.findKey("action", "update:*");
     //results.append(part.findKey("action_:*"));
     //results.append(part.findKey("state_op:*"));
     uint64_t queryExecTime=timer.nsecsElapsed();
     //results.append(part.findKey("updatedAt_14010510200905000"));
     //results.append(part.findKey("createdAt_14010511150209000"));


     //QSet<UUID> ids(results.begin(), results.end());
     //uint64_t time=timer.nsecsElapsed();
     //qDebug()<<"Find Time: "<<queryExecTime<<" ns, record count:"<<results.count();
     /*for (UUID &res:results)   //shahab
     {

         char output[37];
         part.binToHexStr(res.val, output);
         qDebug() << "ID: -->\t" << output;
     }
     qDebug()<<"Query Exec Time: "<<queryExecTime/1000000.0<<" ms, record count:"<<results.count();
     qDebug()<<"finish.";*/
    #pragma endregion

}
