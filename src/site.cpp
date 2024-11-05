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

void Site::commitData(const string &transaction_name) {

    vector<string> variables_to_commit = {}; // TODO: get all the variables that need to be committed when this transaction ends

    for (const auto &variable : variables_to_commit) {
        int value = -1; // TODO: Get value from local
        data_manager.commitData(variable, value);
    }
   
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
