#include "../headers/include_handler.hpp"

Part::Part(QObject *parent) : QObject(parent)
{
    // testInserts();
    //dropGists();
    //loadGists();

    char path[10]="data/";
    struct stat st;
    if (stat(path, &st) == -1)
        mkdir(path, 0700);
}
