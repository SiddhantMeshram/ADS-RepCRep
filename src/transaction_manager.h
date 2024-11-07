#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <string>
#include <map>

using namespace std;

// Forward Declarations.
class Site;

class TransactionManager {

  public:
    TransactionManager(const string& file_name);
  
  private:
    void ProcessInput(const string& file_name);
    vector<int> getSitesforVariables(const string& var);
    void processRead(const vector<string>& params);
    void processBegin(const vector<string>& params, int timer);
    void processWrite(const vector<string>& params);
    void processEnd(const vector<string>& params, int timer);
    void dump();

    map<int, shared_ptr<Site>> site_map;
    unordered_map<string, int> variable_to_site_map;

    struct Transaction
    {
      string transaction_name;
      int begin_time;
      int end_time;
      vector<int> sites_accessed;
      vector<string> variables_acessed;

      Transaction(): transaction_name(""), begin_time(0), end_time(INT_MAX), sites_accessed({}) {}
      Transaction(const string& tn, int bt): transaction_name(tn), begin_time(bt), end_time(INT_MAX), sites_accessed({}) {}

    };

    vector<Transaction> active_transactions;

};

#endif