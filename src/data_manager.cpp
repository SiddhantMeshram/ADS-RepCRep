#include <cassert>
#include <algorithm>

#include "data_manager.h"

using namespace std;

DataManager::DataManager() {

}

pair<int, int> DataManager::getLastCommitted(const string& variable, int time) {
    
    pair<int, int> ret = {-1, -1};
    if (variables.find(variable) == variables.end()) {
        return ret;
    }

    for (auto p : variables[variable].value_vec) {
        if (p.second >= time) {
            break;
        }

        ret = p;
    }

    return ret;
}

int DataManager::getLastCommittedTimestamp(const string &variable, int time) {
    
    if (variables.find(variable) != variables.end()) {
        return getLastCommitted(variable, time).second;
    }
    return 0;
    
}

int DataManager::getLastCommittedTimestamp(const string &variable) {
    
    if (variables.find(variable) != variables.end()) {
        return variables[variable].getLastCommitTime();
    }
    return 0;
    
}

int DataManager::getValue(const string &variable, const string& txn_name, int time) {
    
    if (variables.find(variable) != variables.end()) {
        // If there is a write performed by this transaction, then we should read
        // from that uncommitted transaction.
        if (variables[variable].transaction_to_local_writes_map.count(txn_name)) {
            return variables[variable].transaction_to_local_writes_map[txn_name];
        }
        return getLastCommitted(variable, time).first;
    }
    return 0;
}

void DataManager::commitData(const string &variable, int value, int time) {
    if (variables.find(variable) == variables.end()) {
        string msg = "Variable not found: " + variable;
        assert(false && msg.c_str());
    }

    variables[variable].value_vec.push_back({value, time});
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

    vector<pair<string, int>> vec;
    for (auto& kv : variables) {
        vec.push_back({kv.first, kv.second.getValue()});
    }

    sort(vec.begin(), vec.end(), dumpCompare);
    string ret;
    for (size_t ii = 0; ii < vec.size(); ++ii) {
        ret += vec[ii].first + ": " + to_string(vec[ii].second) + ", ";
    }

    ret.erase(ret.size() - 2);
    return ret;
}