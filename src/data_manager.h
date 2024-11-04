#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <ctime>

class DataManager {
public:
    DataManager();

    std::time_t getLastCommittedTimestamp(const std::string &variable);


};

#endif