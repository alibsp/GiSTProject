/*
* Created by th3r0b0t on 9/23/23.
*/
#ifndef SOCKET_INTERFACE_H
#define SOCKET_INTERFACE_H

#include "part_class.hpp"

class socket_interface
{
private:
    std::vector<std::string> queryVector;
    size_t position;
    Part* _part;
    QString _csvFile;

public:

    socket_interface(Part* part, QString csvFile);

    enum interface_errno
    {
        OK = 0,
        illegal_char,
        extra_open_parenthesis,
        extra_close_parenthesis
    };
    bool loadTreeByDefault = false;

    interface_errno shuntingYard(std::string &exp, std::queue<std::string>& resQueue);
    void queryExecuter(std::queue<std::string> resQueue, std::vector<UUID> &searchResult);
    void commandParser(std::string& queryString);
};


#endif //SOCKET_INTERFACE_H
