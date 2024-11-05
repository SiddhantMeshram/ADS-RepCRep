#include <cassert>

#include "data_manager.h"

DataManager::DataManager() {

}

int DataManager::getLastCommittedTimestamp(const string &variable) {
    
    if (variables.find(variable) != variables.end()) {
        return variables[variable].commit_timestamp;
    }
    return 0;
    
}

int DataManager::getValue(const string &variable) {
    if (variables.find(variable) != variables.end()) {
        return variables[variable].value;
    }
    return -1;
}

void DataManager::commitData(const string &variable, int value) {
    if (variables.find(variable) != variables.end()) {
        variables[variable].value = value;
    }

    variables[variable].commit_timestamp = 0; // TODO: change 0 to current time counter
}

void DataManager::addVariable(const string &variable, int value) {
    if (variables.find(variable) == variables.end()) {
        variables[variable] = Variable(value);
    }
}

void DataManager::writeLocal(const string &variable, const string &transaction_name, int value) {

    if (variables.find(variable) == variables.end()) {
        string msg = "Variable not found: " + variable;
        assert(false && msg.c_str());
    }

    variables[variable].transaction_to_local_writes_map[transaction_name] = value;
}

string DataManager::getDump() {
}