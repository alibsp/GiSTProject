/*
* Created by th3r0b0t on 9/23/23.
*/

#include "../headers/commMan.hpp"
#include <QDebug>



commMan::commMan(Part *part, QString csvFile)
{
    _part = part;
    _csvFile = csvFile;
    _nfaQueryParserObj = new nfaQueryParser();
}

/*void commMan::makeSearchParserNFA()
{
    searchParserNFA.push_back(NFA::State(0,  { {'(', 1}} ));
    searchParserNFA.push_back(NFA::State(1,  { {'(', 1}, {' ', 1}, {'"', 2} } ));
    searchParserNFA.push_back(NFA::State(2,  { {'"', 3}, {2}} ));
    searchParserNFA.push_back(NFA::State(3,  { {' ', 3}, {'&', 4}, {'|', 5}, {')', 6}, {'i', 7}} ));
    searchParserNFA.push_back(NFA::State(4,  { {' ', 4}, {'"', 2}, {'(', 1} }));
    searchParserNFA.push_back(NFA::State(5,  { {' ', 5}, {'"', 2}, {'(', 1} }));
    searchParserNFA.push_back(NFA::State(6,  { {' ', 6}, {')', 6}, {'&', 4}, {'|', 5} }, true));
    searchParserNFA.push_back(NFA::State(7,  { {'n', 8}}));
    searchParserNFA.push_back(NFA::State(8, { {' ', 8}, {'(', 9}}));
    searchParserNFA.push_back(NFA::State(9, { {' ', 9}, {'"', 10}}));
    searchParserNFA.push_back(NFA::State(10, { {'"', 11}, {10}}));
    searchParserNFA.push_back(NFA::State(11, { {' ', 11}, {',', 9}, {')', 6}}));
}*/

/*ParserReturnCode commMan::parseSearch(std::string &exp, std::deque<std::string> &resQueue)
{

    std::stack<std::string> opsVectorStack;
    std::string token="";
    std::string value="";
    std::vector<std::string> values;
    int openParenthesesCount=0;

    #pragma region NFA engine
    exp="("+exp+")";
    unsigned int currentState=0;
    unsigned int nextState=0;
    NFA::Transition *tMatch=nullptr;
    for (size_t i=0;i<exp.size();i++)
    {
        char ch=exp[i];
        tMatch=nullptr;
        for (auto t : searchParserNFA[currentState].transitions)
        {
            if( t.inputChar==ch)
            {
                nextState = t.nextState;
                tMatch = &t;
                break;
            } else if( t.allChars)
            {
                nextState = t.nextState;
                tMatch = &t;
                break;
            }
        }
        if(!tMatch)
            return ParserReturnCode::IllegalCharacterError;
    #pragma endregion
        //Check the number of parentheses in the case that if all characters are not accepted in the current state
        if(!tMatch->allChars)
        {
            if(ch=='(')
            {
                openParenthesesCount++;
                opsVectorStack.push("(");
            }
            else if(ch==')')
            {
                openParenthesesCount--;
                while (!opsVectorStack.empty() && opsVectorStack.top() != "(" )
                {
                    resQueue.push_back(opsVectorStack.top());
                    opsVectorStack.pop();
                }
                if (!opsVectorStack.empty() && opsVectorStack.top() == "(")
                    opsVectorStack.pop();
            }
            else if(ch=='&')
                opsVectorStack.push("&");
            else if(ch=='|')
            {
                while (true)
                {
                    if (opsVectorStack.top() == "&" || opsVectorStack.top() == "|")
                    {
                        resQueue.push_back(opsVectorStack.top());
                        opsVectorStack.pop();
                        continue;
                    }
                    else
                        break;
                }
                opsVectorStack.push("|");
            }
        }
        if(currentState==1 || currentState==4 || currentState==5)
            token.clear();
        if(currentState==2 && nextState == 2)
            token+=ch;

        if(currentState==2 && nextState == 3)// use of token
        {
            //std::cout<<"token:"<<token<<std::endl;
            resQueue.push_back(token);
        }
        if(currentState==8)
            values.clear();
        if(currentState==9)
            value.clear();
        if(currentState==10 && nextState == 10)
            value+=ch;

        if(currentState==10 && nextState == 11)// use of value
        {
            //std::cout<<"value:"<<value<<std::endl;
            values.push_back(value);
        }
        if(currentState==11 && nextState == 6)// use of values
        {
            for (size_t j=0; j<values.size(); j++)
            {
                if(j==0)
                    resQueue.back() = token+"_"+values[j];
                else
                {
                    resQueue.push_back(token+"_"+values[j]);
                    resQueue.push_back("|");
                }
            }
        }

        currentState = nextState;
    }
    if(openParenthesesCount>0)
        return ParserReturnCode::ExtraOpenParenthesisError;
    if(openParenthesesCount<0)
        return ParserReturnCode::ExtraCloseParenthesisError;
    if(!searchParserNFA[currentState].accept)
        return ParserReturnCode::IllegalCharacterError;

    for(const auto &q : resQueue)
        std::cout<<q<<std::endl;
    return ParserReturnCode::OK;
}*/

//[[deprecated("Use nfaQueryParser class instead.")]]
ParserReturnCode commMan::shuntingYard(std::string &exp, std::queue<std::string>& resQueue)
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
                return ParserReturnCode::ExtraCloseParenthesisError;
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
        else if (exp[i] == '&')
        {
            if (!resString.empty())
            {
                resQueue.push(resString);
                resString.clear();
            }
            charToStr = exp[i];
            //charToStr += exp[i + 1];
            opsVectorStack.push(charToStr);
            i += 1;
        }
        else if (exp[i] == '|')
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
                if ((opsVectorStack.top() == "&") || (opsVectorStack.top() == "|"))
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
            //charToStr += exp[i + 1];
            opsVectorStack.push(charToStr);
            i += 1;
        }
        else if(exp[i] == ' '){i++;}    //Skip white-spaces
        else
        {
            std::cerr << "Illegal character detected in Shunting yard!" << std::endl;
            return ParserReturnCode::IllegalCharacterError;
        }
    }
    #pragma endregion

    #pragma region shuntingYard_remaining_items_check
    if(!opsVectorStack.empty())
    {
        std::cerr << "Shunting yard extra open Parenthesis detected" << std::endl;
        return ParserReturnCode::ExtraOpenParenthesisError;
    }

    std::cerr << "ShuntingYard is done!" << std::endl;
    return ParserReturnCode::OK;
    #pragma endregion
}

void commMan::execQuery(std::deque<std::string> resQueue, std::vector<UUID> &searchResult)
{
    std::stack<std::vector<UUID>> parseStack;

    while (!resQueue.empty())
    {
        if (((resQueue.front() != "&") && (resQueue.front() != "|")))
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
            resQueue.pop_front();
        }
        else
        {
            std::vector<UUID> firstArg, secondArg, operationRes;

            secondArg = parseStack.top();
            parseStack.pop();

            firstArg = parseStack.top();
            parseStack.pop();

            if (resQueue.front() == "&")
            {
                (*_part).uuid_intersect(firstArg, secondArg, operationRes);
                parseStack.push(operationRes);
                resQueue.pop_front();
            }
            else
            {
                _part->uuid_union(firstArg, secondArg, operationRes);
                parseStack.push(operationRes);
                resQueue.pop_front();
            }
        }
    }
    searchResult = parseStack.top();
}

void commMan::commandHandler(std::string& queryString, std::vector<UUID>& searchCommandResult)
{
    #pragma region commandParser_main_loop
    std::vector<std::string> queryVector;
    size_t position;

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
        queryVector.push_back(queryString.substr(startPosition, position - startPosition));
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
                    if(shuntingYard(queryVector[queryVectorIterator], resQueue) == ParserErrorCode::OK)
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

                //std::vector<UUID> searchResult; //Defined as parameter now!
                std::deque<std::string> resQueue;
                nfaQueryParserReturnCode error = _nfaQueryParserObj->parse(queryVector[queryVectorIterator], resQueue);
                //ParserReturnCode error=parseSearch(queryVector[queryVectorIterator], resQueue);
                if(error == nfaQueryParserReturnCode::OK)
                {
                    QElapsedTimer timer;
                    timer.start();
                    execQuery(resQueue, searchCommandResult);
                    int elapsedTime=timer.nsecsElapsed();

                    /*char searchOutput[37]{0};
                    std::ofstream searchOutputFile;
                    searchOutputFile.open("searchOutput.bin");
                    for (const auto &uuid: searchResult)
                    {
                        GeneralUtils::binToHexStr(uuid.val, searchOutput);
                        searchOutputFile << searchOutput << std::endl;
                    }
                    searchOutputFile.close();*/

                    std::cout << "Searching is done. time elapsed:"<<elapsedTime/1000000.0<< " ms; Count:"<< searchCommandResult.size()<< std::endl;
                    //std::cout << "Searching is done. searchOutput.bin. time elapsed:"<<elapsedTime/1000000.0<< " ms; Count:"<< searchResult.size()<< std::endl;
                }
                else
                    std::cerr << "Search Parsing error! Error: " <<static_cast<int>(error)<< std::endl;

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
