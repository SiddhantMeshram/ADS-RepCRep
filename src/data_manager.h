#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <ctime>

using namespace std;

class DataManager {
public:
    DataManager();

    int getLastCommittedTimestamp(const string &variable);
    int getValue(const string &variable);
    void commitData(const string &variable, int value, int time);
    void addVariable(const string &variable, int value);
    void writeLocal(const string &variable,
                    const string &transaction_name,
                    int value);
    string getDump();

    struct Variable
    {
        int value;
        int commit_timestamp;
        unordered_map<string, int> transaction_to_local_writes_map;

        Variable() : value(0), commit_timestamp(0) {}
        Variable(int val) : value(val), commit_timestamp(0) {}
        Variable(int val, int ct) : value(val), commit_timestamp(ct) {} 
    };

    static bool dumpCompare(pair<string, int>& p1, pair<string, int>& p2) {
        if (p1.first.size() == p2.first.size()) {
            return p1 < p2;
        }

        return p1.first.size() < p2.first.size();
    }

    unordered_map<string, Variable> variables;
    
};

#endif