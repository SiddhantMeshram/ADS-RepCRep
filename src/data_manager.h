/*
Authors:
  Sujana Maithili Chindam (sc10648)
  Siddhant Meshram (sm10954)

Date:
  11/25/2024

Data Manager is used to manage the data stored on a particular site.
*/

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <ctime>

using namespace std;

class DataManager {
public:
    DataManager();

    // Gets the time when this variable was committed before "time".
    int getLastCommittedTimestamp(const string &variable, int time);

    // Gets the last committed timestamp for this variable.
    int getLastCommittedTimestamp(const string &variable);

    // If this transaction has made a local write, returns the locally
    // written value. Otherwise, returns the value that was committed before
    // "time".
    int getValue(const string &variable, const string& txn_name, int time);

    // Commits the locally written data for the variable.
    void commitData(const string &variable, int value, int time);

    // Adds the variable to the site.
    void addVariable(const string &variable, int value);

    // Writes the variable locally without committing.
    void writeLocal(const string &variable,
                    const string &transaction_name,
                    int value);
    
    // Returns <value, timestamp> pair for the given variable which was
    // committed before "time".
    pair<int, int> getLastCommitted(const string& variable, int time);

    // Returns the dump of this site.
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

    // Custom comparator.
    static bool dumpCompare(pair<string, int>& p1, pair<string, int>& p2) {
        if (p1.first.size() == p2.first.size()) {
            return p1 < p2;
        }

        return p1.first.size() < p2.first.size();
    }

    // Map which maintains all the variables on this site.
    unordered_map<string, Variable> variables;
    
};

#endif