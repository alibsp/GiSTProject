//
// Created by shahabseddigh on 10/15/23.
//


#include "../headers/nfaQueryParser.hpp"

nfaQueryParser::nfaQueryParser()
{
    nfaStatesVector.push_back(nfaQueryParser::State(0, {{'(', 1}} ));
    nfaStatesVector.push_back(nfaQueryParser::State(1, {{'(', 1}, {' ', 1}, {'"', 2} } ));
    nfaStatesVector.push_back(nfaQueryParser::State(2, {{'"', 3}, {2}} ));
    nfaStatesVector.push_back(nfaQueryParser::State(3, {{' ', 3}, {'&', 4}, {'|', 5}, {')', 6}, {'i', 7}} ));
    nfaStatesVector.push_back(nfaQueryParser::State(4, {{' ', 4}, {'"', 2}, {'(', 1} }));
    nfaStatesVector.push_back(nfaQueryParser::State(5, {{' ', 5}, {'"', 2}, {'(', 1} }));
    nfaStatesVector.push_back(nfaQueryParser::State(6, {{' ', 6}, {')', 6}, {'&', 4}, {'|', 5} }, true));
    nfaStatesVector.push_back(nfaQueryParser::State(7, {{'n', 8}}));
    nfaStatesVector.push_back(nfaQueryParser::State(8, {{' ', 8}, {'(', 9}}));
    nfaStatesVector.push_back(nfaQueryParser::State(9, {{' ', 9}, {'"', 10}}));
    nfaStatesVector.push_back(nfaQueryParser::State(10, {{'"', 11}, {10}}));
    nfaStatesVector.push_back(nfaQueryParser::State(11, {{' ', 11}, {',', 9}, {')', 6}}));
}

nfaQueryParserReturnCode nfaQueryParser::parse(std::string &exp, std::deque<std::string> &resQueue)
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
    nfaQueryParser::Transition *tMatch=nullptr;
    for (size_t i=0;i<exp.size();i++)
    {
        char ch=exp[i];
        tMatch=nullptr;
        for (auto t : nfaStatesVector[currentState].transitions)
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
            return nfaQueryParserReturnCode::IllegalCharacterError;
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
        return nfaQueryParserReturnCode::ExtraOpenParenthesisError;
    if(openParenthesesCount<0)
        return nfaQueryParserReturnCode::ExtraCloseParenthesisError;
    if(!nfaStatesVector[currentState].accept)
        return nfaQueryParserReturnCode::IllegalCharacterError;

    /*for(const auto &q : resQueue)
        std::cout<<q<<std::endl;*/
    return nfaQueryParserReturnCode::OK;
}