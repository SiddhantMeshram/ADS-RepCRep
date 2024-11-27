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
    bool isSafeToCommit(const vector<string>& params, int timer);
    void processCommit(const string& txn_name, int time);
    void processAbort(const string& txn_name);
    void dump();

    map<int, shared_ptr<Site>> site_map;
    unordered_map<string, int> variable_to_site_map;

    struct Transaction
    {
      string transaction_name;
      int begin_time;
      int end_time;
      int commit_time;
      unordered_set<int> sites_accessed;
      unordered_set<string> variables_accessed;
      unordered_set<string> variables_accessed_for_read;

      Transaction(): transaction_name(""), begin_time(0), end_time(INT_MAX), sites_accessed({}) {}
      Transaction(const string& tn, int bt): transaction_name(tn), begin_time(bt), end_time(INT_MAX), sites_accessed({}) {}

    };


    unordered_map<string, Transaction> active_transactions;

    struct Edge
    {
      string target;
      string edge_type;
  
      Edge(): target(""), edge_type("") {}
      Edge(const string& et, const string& edt): target(et), edge_type(edt) {}
    };

    unordered_map<string, vector<Edge>> serialization_graph;
    unordered_map<string, vector<Edge>> temp_graph;

    void addEdge(const string& source, const string& target, const string& type);
    bool hasTwoRwCycle();
    bool detectCycle(string node, unordered_map<string, bool>& visited, unordered_map<string, bool>& stack, vector<pair<string, string>>& pathEdges);


};

#endif