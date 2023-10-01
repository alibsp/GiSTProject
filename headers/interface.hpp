/*
* Created by th3r0b0t on 9/23/23.
*/
#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H

#include "part_class.hpp"

enum class ParserReturnCode
{
    OK = 0,
    IllegalCharacterError,
    ExtraOpenParenthesisError,
    ExtraCloseParenthesisError
};

class NFA
{
public:
    struct Transition
    {
        unsigned char inputChar;
        unsigned int nextState;
        bool allChars;
        Transition(const unsigned char _inputChar, const unsigned int _nextState)
        {
            inputChar = _inputChar;
            nextState=_nextState;
            allChars=false;
        }
        Transition(const unsigned int _nextState)
        {
            inputChar = '*';
            nextState=_nextState;
            allChars=true;
        }
    };
    struct State
    {
        unsigned int state;
        bool accept;
        std::vector<Transition> transitions;

        State(const unsigned int _state,
              const std::vector<Transition> _transitions,
              const bool _accept =false)
        {
            state = _state;
            accept = _accept;
            transitions = _transitions;
        }

    };
};

class Interface
{
private:


    std::vector<NFA::State> searchParserNFA;

    Part* _part;
    QString _csvFile;

    void makeSearchParserNFA();
    ParserReturnCode parseSearch(std::string &exp, std::deque<std::string> &resQueue);
    ParserReturnCode shuntingYard(std::string &exp, std::queue<std::string>& resQueue);
    void execQuery(std::deque<string> resQueue, std::vector<UUID> &searchResult);
public:

    Interface(Part* part, QString csvFile);
    bool loadTreeByDefault = false;

    void parseCommand(std::string& queryString);
};


#endif //SOCKET_INTERFACE_H
