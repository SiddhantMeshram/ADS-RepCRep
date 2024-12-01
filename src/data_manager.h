#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <ctime>

using namespace std;

class DataManager {
public:
    DataManager();

    int getLastCommittedTimestamp(const string &variable, int time);
    int getValue(const string &variable, int time);
    void commitData(const string &variable, int value, int time);
    void addVariable(const string &variable, int value);
    void writeLocal(const string &variable,
                    const string &transaction_name,
                    int value);
    pair<int, int> getLastCommitted(const string& variable, int time);
    string getDump();

    struct Variable
    {
        // Vector storing <value, commit time> for each commit that happened
        // for this variable.
        vector<pair<int, int>> value_vec;
        unordered_map<string, int> transaction_to_local_writes_map;

        Variable() : value_vec{{0, 0}} {}
        Variable(int val) : value_vec{{val, 0}} {}
        Variable(int val, int ct) : value_vec{{val, ct}} {} 

        int getValue() {
            return value_vec.back().first;
        };

        int getLastCommitTime() {
            return value_vec.back().second;
        }
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