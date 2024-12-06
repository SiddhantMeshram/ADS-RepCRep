/*
Authors:
  Sujana Maithili Chindam (sc10648)
  Siddhant Meshram (sm10954)

Date:
  11/18/2024

The Transaction Manager is the top level class that manages all the
transactions and maintains site data which is used while processing the
transactions. The transactions data is maintained as a struct within this
class. This includes fields such as begin_time, end_time, list of all sites
transaction has accessed etc. All the sites are also maintained in a data
structure within this class. Additionally, we also maintain the serialization
graph within this class.
*/

#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <climits>

using namespace std;

// Forward Declarations.
class Site;

class TransactionManager {

  public:
    TransactionManager();
  
  private:
    // This function reads the ipnut file line by line and calls the
    // appropriate function for processing the request.
    void ProcessInput();

    // Returns a list of sites that host this variable.
    vector<int> getSitesforVariables(const string& var);

    // Processes a read request.
    void processRead(vector<string> params, int timer);

    // Initializes a transaction when it begins.
    void processBegin(const vector<string>& params, int timer);

    // Processes a write request.
    void processWrite(vector<string> params, int timer);

    // When an end(T) is encountered, this function checks if it is safe for it
    // to commit.
    string isSafeToCommit(const vector<string>& params, int timer);

    // Commits a transaction.
    void processCommit(const string& txn_name, int time);

    // Aborts a transaction.
    void processAbort(const string& txn_name, string str);

    // Processes a recover request.
    void processRecover(int site, int timer);

    // Forwards the read request to the site.
    void readData(int site, const string& txn_name, const string& var, int timer);

    // Forwards the write request to the site.
    void writeData(int site, string var, string txn_name, int value, int timer);

    // Dump the data on each site.
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

    // Map to hold data related to each site.
    map<int, shared_ptr<Site>> site_map;

    // Map to keep track of requests that are waiting for sites to recover.
    unordered_map<int, vector<vector<string>>> recovery_map;

    // In case one request was waiting on multiple sites, the request will get
    // processed once the first site recovers. Other sites should not process
    // this request when they recover. We use this set to dedup.
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

    // Map to keep track of all the transactions.
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

    // Adds the relevant edge to the graph.
    void addEdge(const string& source, const string& target, const string& type);
    
    // Functions to check if there is a cycle in the graph where there are 2 RW
    // edges adjacent.
    bool hasTwoRwCycle();
    bool detectCycle(const string& startNode, const string& currentNode, vector<string>& path);
    bool analyzeCycle(const vector<string>& cycle, const string& lastEdgeType);

};

#endif