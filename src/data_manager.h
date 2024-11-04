#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <ctime>

class DataManager {
public:
    DataManager();

    int getLastCommittedTimestamp(const std::string &variable);
    int getValue(const std::string &variable);
    void commitData(const std::string &variable, int value);

    struct Variable
    {
        int value;
        int commit_timestamp;
    };

    std::unordered_map<std::string, Variable> variables;
    
};

#endif