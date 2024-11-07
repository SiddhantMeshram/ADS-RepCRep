#include "site.h"
#include "data_manager.h"
#include <cassert>

Site::Site(const int &index) : site_index(index), name("site " + to_string(site_index)), status(true), last_down_timestamp(0) {}

int Site::readData(const string &variable) {
     return data_manager.getValue(variable);
}

void Site::writeLocal(const string &variable, const string &transaction_name, int value) {
    
    data_manager.writeLocal(variable, transaction_name, value);
}

void Site::commitData(const string &transaction_name, int time) {

    unordered_map<string, int> variables_to_commit = getVariablesForTxn(transaction_name);
    for (const auto &kv : variables_to_commit) {
        data_manager.commitData(kv.first, kv.second, time);
    }
}

unordered_map<string, int> Site::getVariablesForTxn(const string& txn_name) {

    unordered_map<string, int> ret;
    for (const auto& kv : data_manager.variables) {
        if (kv.second.transaction_to_local_writes_map.find(txn_name) !=
            kv.second.transaction_to_local_writes_map.end()) {
            ret[kv.first] = kv.second.transaction_to_local_writes_map.at(txn_name);
        }
    }

    return ret;
}

int Site::getLastCommittedTimestamp(const std::string &variable) {
    return data_manager.getLastCommittedTimestamp(variable);
}

bool Site::isUp() {
    return status;
}

void Site::setUp() {
    status = true;
}

void Site::setDown(int timer) {
    status = false;
    last_down_timestamp = timer;
}

void Site::addVariable(const string &variable, int value) {
    data_manager.addVariable(variable, value);
}

int Site::last_down() {
    return last_down_timestamp;
}

string Site::getName() {
    return name;
}

string Site::getDump() {
    return data_manager.getDump();
}
