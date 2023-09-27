/*
* Created by th3r0b0t on 9/23/23.
*/

#include "../headers/socket_interface.hpp"
#include <QDebug>

socket_interface::socket_interface(Part *part, QString csvFile)
{
    _part = part;
    _csvFile = csvFile;
}

socket_interface::interface_errno socket_interface::shuntingYard(std::string &exp, std::queue<std::string>& resQueue)
{
    #pragma region shuntingYard_variables
    //std::queue<std::string> resQueue;
    std::stack<std::string> opsVectorStack;
    std::string resString;
    std::string charToStr;
    #pragma endregion

    #pragma region shuntingYard_main_loop
    exp = '(' + exp + ')';

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

            if(opsVectorStack.empty())
            {
                std::cerr << "Shunting yard extra close Parenthesis detected" << std::endl;
                return socket_interface::extra_close_parenthesis;
            }

            while( !opsVectorStack.empty() )
            {
                if( opsVectorStack.top() != "(" )
                {
                    resQueue.push(opsVectorStack.top());
                    opsVectorStack.pop();
                }
                else
                {
                    opsVectorStack.pop();
                    i++;
                    break;
                }
            }
        }
        else if (exp[i] == '"')
        {
            i++;
            while (i < exp.length() && exp[i] != '"')
            {
                resString += exp[i];
                i++;
            }
            i++;
        }
        else if ( (exp[i] >= 39 && exp[i] <= 122) || (exp[i] >= 35 && exp[i] <= 37) )
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
                    resQueue.push(opsVectorStack.top());
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
        else if(exp[i] == ' '){i++;}    //Skip white-spaces
        else
        {
            std::cerr << "Illegal character detected in Shunting yard!" << std::endl;
            return socket_interface::interface_errno::illegal_char;
        }
    }
    #pragma endregion

    #pragma region shuntingYard_remaining_items_check
    if(!opsVectorStack.empty())
    {
        std::cerr << "Shunting yard extra open Parenthesis detected" << std::endl;
        return socket_interface::interface_errno::extra_open_parenthesis;
    }

    std::cerr << "ShuntingYard is done!" << std::endl;
    return socket_interface::interface_errno::OK;
    #pragma endregion
}

void socket_interface::queryExecuter(std::queue<std::string> resQueue, std::vector<UUID> &searchResult)
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
                (*_part).findKeyVectorWithRegexDriver(resQueue.front(), findKeyResult);
            }
            else
            {
                (*_part).findKeyVectorDriver(resQueue.front().c_str(), findKeyResult);
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
                (*_part).uuid_intersect(firstArg, secondArg, operationRes);
                parseStack.push(operationRes);
                resQueue.pop();
            }
            else
            {
                _part->uuid_union(firstArg, secondArg, operationRes);
                parseStack.push(operationRes);
                resQueue.pop();
            }
        }
    }
    searchResult = parseStack.top();
}

void socket_interface::commandParser(std::string& queryString)
{
    #pragma region commandParser_main_loop

    #pragma region commandParser_splitting_queries
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

    #pragma region commandParser_parsing_queries
    size_t queryVectorIterator = 0;
    if (!queryVector.empty())
    {
        while (queryVectorIterator < queryVector.size())
        {
            if (queryVector[queryVectorIterator].substr(0, 9) == "load tree")
            {
                std::cout << "loading tree" << std::endl;
                (*_part).loadGists();
            }

            else if (queryVector[queryVectorIterator].substr(0, 11) == "reload tree")
            {
                std::cout << "reloading tree" << std::endl;
                (*_part).dropGists();
                (*_part).importCSV(_csvFile);
            }

            else if (queryVector[queryVectorIterator].substr(0, 9) == "list keys")
            {
                std::cout << "listing all keys:" << std::endl;
                int i = 0;
                for (const auto& s: (*_part).getAllTreeNames())
                {
                    i++;
                    qDebug() << i << ":" << s;
                }
            }

            else if (queryVector[queryVectorIterator].substr(0, 4) == "quit")
            {
                std::cout << "quiting..." << std::endl;
                exit(0);
            }

            else if (queryVector[queryVectorIterator].substr(0, 7) == "search ")
            {
                std::cout << "searching..." << std::endl;
                queryVector[queryVectorIterator].erase(0, 7);

                /*size_t queryWhiteSpacePosition = 0;
                while( (queryWhiteSpacePosition = queryVector[queryVectorIterator].find(' ', queryWhiteSpacePosition)) != std::string::npos )
                {
                    queryVector[queryVectorIterator].erase(queryWhiteSpacePosition, 1);
                }*/

                /*
                if ( (queryVector[queryVectorIterator].find("||") != std::string::npos) || (queryVector[queryVectorIterator].find("&&") != std::string::npos) )
                {
                    std::vector<UUID> searchResult;
                    std::queue<std::string> resQueue;
                    if(shuntingYard(queryVector[queryVectorIterator], resQueue) == interface_errno::OK)
                    {
                        queryExecuter(resQueue, part, searchResult);

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
                */

                std::vector<UUID> searchResult;
                std::queue<std::string> resQueue;
                if(shuntingYard(queryVector[queryVectorIterator], resQueue) == interface_errno::OK)
                {
                    queryExecuter(resQueue, searchResult);

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

    #pragma endregion
}