/*
Authors:
  Sujana Maithili Chindam (sc10648)
  Siddhant Meshram (sm10954)

Date:
  11/25/2024

Each site maintains all the states relevant to a site. It stores all the
variables for that site and also has a Data Manager that manages all the
variables in that site.
*/

#ifndef SITE_H
#define SITE_H

#include <string>
#include "data_manager.h"

class Site {
public:
    Site(const int& index);

    // Reads the value of the provided variable.
    int readData(const std::string &variable, const string& txn_name, int time);
    
    // This function writes the data locally without committing it.
    void writeLocal(const std::string &variable, const std::string &transaction_name, int value); 
    
    // Commits all the local data related to this transaction.
    void commitData(const std::string &transaction_name, int time);

    // Adds the variable to the site. 
    void addVariable(const std::string &variable, int value);

    // Gets the time when this variable was last committed.
    int getLastCommittedTimestamp(const std::string &variable);

    // Gets the time when this variable was committed before "time".
    int getLastCommittedTimestamp(const std::string &variable, int time);

    // Returns all the vriables relevant to this transaction.
    unordered_map<string, int> getVariablesForTxn(const string& txn_name);

    // Returns true if the site is up, else returns false.
    bool isUp();

    // Sets the site up.
    void setUp();

    // Returns the time when the site was last down.
    int last_down();

    // Sets the site down.
    void setDown(int timer);

    // Returns the name of the site.
    string getName();

    // Dumps the data related to the site.
    string getDump();

private:

    // Index of this site.
    int site_index;

    // Name of the site.
    std::string name;

    // Site status Up or Down.
    bool status; 
    
    // Stores the time when the site was last down.
    int last_down_timestamp;

    // Data manager for this site.
    DataManager data_manager; 
};


#endif