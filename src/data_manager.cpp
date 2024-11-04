#include "data_manager.h"

DataManager::DataManager() {

}

int DataManager::getLastCommittedTimestamp(const std::string &variable) {
    
    if (variables.find(variable) != variables.end()) {
        return variables[variable].commit_timestamp;
    }
    return 0;
    
}

int DataManager::getValue(const std::string &variable) {
    if (variables.find(variable) != variables.end()) {
        return variables[variable].value;
    }
    return -1;
}

void DataManager::commitData(const std::string &variable, int value) {
    if (variables.find(variable) != variables.end()) {
        variables[variable].value = value;
    }

    variables[variable].commit_timestamp = 0; // TODO: change 0 to current time counter
}

