#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <string>
#include <map>
#include <fstream>

using namespace std;

// Forward Declarations.
class Site;

class TransactionManager {

  public:
    TransactionManager(const string& input_file, const string& output_file);
  
  private:
    void ProcessInput(const string& file_name);
    vector<int> getSitesforVariables(const string& var);
    void processRead(vector<string> params, int timer);
    void processBegin(const vector<string>& params, int timer);
    void processWrite(vector<string> params, int timer);
    string isSafeToCommit(const vector<string>& params, int timer);
    void processCommit(const string& txn_name, int time);
    void processAbort(const string& txn_name, string str);
    void processRecover(int site, int timer);
    void readData(int site, const string& txn_name, const string& var, int timer);
    void writeData(int site, string var, string txn_name, int value, int timer);
    void dump();

    // Custom hash function for vector<string>.
    struct VectorStringHash {
        size_t operator()(const vector<string>& vec) const {
            size_t seed = 0;
            for (const auto& str : vec) {
                seed ^= hash<string>()(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    // Custom equality comparator for vector<string>.
    struct VectorStringEqual {
        bool operator()(const vector<string>& a, const vector<string>& b) const {
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
    ofstream outFile;

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
    bool detectCycle(const string& startNode, const string& currentNode, vector<string>& path);
    bool analyzeCycle(const vector<string>& cycle, const string& lastEdgeType);

};

#endif