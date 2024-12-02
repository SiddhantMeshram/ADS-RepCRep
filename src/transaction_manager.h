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
    void processRead(vector<string> params, int timer);
    void processBegin(const vector<string>& params, int timer);
    void processWrite(const vector<string>& params, int timer);
    bool isSafeToCommit(const vector<string>& params, int timer);
    void processCommit(const string& txn_name, int time);
    void processAbort(const string& txn_name);
    void processRecover(int site);
    void processReadAfterRecovery(vector<string> params, int site);
    void dump();

    // Custom hash function for std::vector<std::string>
    struct VectorStringHash {
        std::size_t operator()(const std::vector<std::string>& vec) const {
            std::size_t seed = 0;
            for (const auto& str : vec) {
                seed ^= std::hash<std::string>()(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    // Custom equality comparator for std::vector<std::string>
    struct VectorStringEqual {
        bool operator()(const std::vector<std::string>& a, const std::vector<std::string>& b) const {
            return a == b;
        }
    };

    map<int, shared_ptr<Site>> site_map;
    unordered_map<string, int> variable_to_site_map;

    // Map to keep track of requests that are waiting for sites to recover.
    unordered_map<int, vector<vector<string>>> recovery_map;
    unordered_set<vector<string>, VectorStringHash, VectorStringEqual> already_recovered_set;

    struct Transaction
    {
      string transaction_name;
      int begin_time;
      int end_time;
      int commit_time;
      bool isaborted;
      unordered_map<int, int> sites_accessed;
      unordered_set<string> variables_accessed;
      unordered_set<string> variables_accessed_for_read;

      Transaction(): transaction_name(""), begin_time(0), end_time(INT_MAX), isaborted(false), sites_accessed({}) {}
      Transaction(const string& tn, int bt): transaction_name(tn), begin_time(bt), end_time(INT_MAX), isaborted(false), sites_accessed({}) {}

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