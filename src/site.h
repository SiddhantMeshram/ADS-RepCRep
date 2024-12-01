#ifndef SITE_H
#define SITE_H

#include <string>
#include "data_manager.h"

class Site {
public:
    Site(const int& index);
    int readData(const std::string &variable, int time);
    void writeLocal(const std::string &variable, const std::string &transaction_name, int value); 
    void commitData(const std::string &transaction_name, int time); 
    void addVariable(const std::string &variable, int value);
    int getLastCommittedTimestamp(const std::string &variable, int time);
    unordered_map<string, int> getVariablesForTxn(const string& txn_name);

    bool isUp();
    void setUp();
    int last_down();
    void setDown(int timer);
    string getName();
    string getDump();

private:
    int site_index;
    std::string name;
    // Site status Up or Down
    bool status; 
    int last_down_timestamp; 
    DataManager data_manager; 
};


#endif