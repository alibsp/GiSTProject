//
// Created by shahabseddigh on 10/15/23.
//

#ifndef GISTPROJECT_NFAQUERYPARSER_HPP
#define GISTPROJECT_NFAQUERYPARSER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <stack>

enum class nfaQueryParserReturnCode
{
    OK = 0,
    IllegalCharacterError,
    ExtraOpenParenthesisError,
    ExtraCloseParenthesisError
};


class nfaQueryParser
{
    public:

        nfaQueryParser();

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

            State(const unsigned int _state, const std::vector<Transition> _transitions, const bool _accept =false)
            {
                state = _state;
                accept = _accept;
                transitions = _transitions;
            }

        };

        nfaQueryParserReturnCode parse(std::string &exp, std::deque<std::string> &resQueue);

    private:
        std::vector<nfaQueryParser::State> nfaStatesVector;
};



#endif //GISTPROJECT_NFAQUERYPARSER_HPP
