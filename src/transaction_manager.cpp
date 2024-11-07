#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <cassert>

#include "transaction_manager.h"
#include "site.h"

using namespace std;

TransactionManager::TransactionManager(const string& file_name) {
  
  // Create sites.
  for (int ii = 1; ii <= 10; ++ii) {
    site_map[ii] = make_shared<Site>(ii);
  }

  // Initialize variables in relevant sites.
  for (int ii = 1; ii <= 20; ++ii) {
    string var_name = "x" + to_string(ii);
    for (int site_index : getSitesforVariables(var_name)) {
      site_map[site_index]->addVariable(var_name, ii * 10);
    }
  }

  // Process Input.
  ProcessInput(file_name);
}

vector<int> TransactionManager::getSitesforVariables(const string& var) {

  int var_index = stoi(var.substr(1));
  if (var_index % 2) {
    return {1 + (var_index % 10)};
  }

  vector<int> ret;
  for (auto& kv : site_map) {
    ret.push_back(kv.first);
  }

  return ret;
}

void TransactionManager::ProcessInput(const string& file_name) {

  ifstream input(file_name);
  string line;
  int timer = 0;
  while (getline(input, line)) {
    ++timer;
    string command, var;
    unordered_set<char> delimiters{'(', ')', ','};
    vector<string> params;

    for (int ii = 0; ii < line.size(); ++ii) {
        if (delimiters.find(line[ii]) != delimiters.end()) {
            line[ii] = ' ';
        }
    }

    istringstream iss(line);
    iss >> command;
    while (iss >> var) {
      params.push_back(var);
    }

    if (command == "begin") {
      processBegin(params, timer);

    } else if (command == "R") {
      processRead(params);
    } else if (command == "W") {
      processWrite(params);
    } else if (command == "fail") {
      cout << "site " << params[0] << " fails" << endl;
      site_map[stoi(params[0])]->setDown(timer);
    } else if (command == "recover") {
      cout << "site " << params[0] << " recovers" << endl;
      site_map[stoi(params[0])]->setUp();
    } else if (command == "end") {
      if (isSafeToCommit(params, timer)) {
        cout << params[0] << " commits" << endl;
        // Commit.
        processCommit(params[0], timer);
      } else {
        cout << params[0] << " aborts" << endl;
        // Abort.
        processAbort(params[0], timer);
      }
    } else if (command == "dump") {
      dump();
    } else {
      assert(false && "Unexpected command");
    }
  }

  input.close();
}


void TransactionManager::addEdge(const string& source, const string& target, const string& type){
  Edge e(target, type);
  serialization_graph[source].push_back(e);
}

bool TransactionManager::hasTwoRwCycle(){

  unordered_map<string, bool> visited;
  unordered_map<string, bool> stack;
  vector<pair<string, string>> pathEdges;

  for (auto node : serialization_graph) {
      if (detectCycle(node.first, visited, stack, pathEdges)) {
          return true;
      }
  }
  return false;

}

bool TransactionManager::detectCycle(string node, unordered_map<string, bool>& visited, unordered_map<string, bool>& stack, vector<pair<string, string>>& pathEdges) {
  if(!visited[node]){
    visited[node] = true;
    stack[node] = true;

    for(auto edge: serialization_graph[node]){
      pathEdges.push_back({node, edge.edge_type});

      if(!visited[edge.target] && detectCycle(edge.target, visited, stack, pathEdges)){
        return true;
      }
      else if(stack[edge.target]){
        //Cycle detected, now check for consecutive "rw" edges in the cycle
        auto cycleStart = pathEdges.end();
        for (auto it = pathEdges.begin(); it != pathEdges.end(); ++it) {
          if (it->first == edge.target) {
            cycleStart = it;
            break;
          }
        }

        for (auto it = cycleStart; it != pathEdges.end() - 1; ++it) {
          if (it->second == "rw" && (it + 1)->second == "rw") {
            return true; 
          }
        }
      }
    }
    pathEdges.pop_back();
  }
  stack[node] = false;
  return false;
}

void TransactionManager::processRead(const vector<string>& params) {

  vector<int> sites = getSitesforVariables(params[1]);
  for (int ii : sites) {
    if (site_map[ii]->isUp()) {
      // from active_transactions find params[0] say t
      Transaction t;
      if (active_transactions.find(params[0]) == active_transactions.end()) {
        cout << "Unable to find this transaction in active transactions: "
             << params[0] << endl;
        return;
      }

      t = active_transactions[params[0]];
      if((site_map[ii]->last_down() < t.begin_time) && (site_map[ii]-> getLastCommittedTimestamp(params[1]) < t.begin_time)){
        cout << params[1] << ": " << site_map[ii]->readData(params[1]) << endl;
        break;
      }
    }
  }

  return;
}

void TransactionManager::processBegin(const vector<string>& params, int timer) {
  Transaction newTransaction(params[0], timer);
  active_transactions[params[0]] = newTransaction;
}

void TransactionManager::processWrite(const vector<string>& params) {

  const string& txn_name = params[0];
  const string& var = params[1];
  int value = stoi(params[2]);
  vector<int> sites = getSitesforVariables(params[1]);
  bool no_writes = true;
  vector<int> sites_accessed_for_curr_write;
  for (int ii : sites) {
    if (site_map[ii]->isUp()) {
      site_map[ii]->writeLocal(var, txn_name, value);
      sites_accessed_for_curr_write.push_back(ii);
      active_transactions[txn_name].sites_accessed.insert(ii);
      active_transactions[txn_name].variables_accessed.insert(var);
      no_writes = false;
    }
  }

  // TODO: Check what needs to be done if no site was up when a write came.
  if (no_writes) {
    cout << "No writes happened for variable: " << var << endl;
    return;
  }

  string ret;
  for (int ii : sites_accessed_for_curr_write) {
    ret += site_map[ii]->getName() + " ";
  }

  cout << "Variable: " << var << " written on sites: " << ret;
}

void TransactionManager::dump() {
  for (auto& kv : site_map) {
    cout << (kv.second)->getName() << " - " << (kv.second)->getDump() << endl;
  }
}

bool TransactionManager::isSafeToCommit(const vector<string>& params, int timer) {
  Transaction endTransaction;

  if (active_transactions.find(params[0]) == active_transactions.end()) {
    cout << "Unable to find this transaction in active transactions: "
          << params[0] << endl;
    return false;
  }

  endTransaction = active_transactions[params[0]];
  endTransaction.end_time = timer;

  bool is_safe = true;

  // 1st Safety Check: Available Copies Safe
  for(auto site: endTransaction.sites_accessed){
    if(site_map[site] -> last_down() > endTransaction.begin_time){
      is_safe = false;
      break;
    }
  }

  // 2nd Safety Check: Snapshot Isolation Safe
  for(auto variable: endTransaction.variables_accessed){
    vector<int> sites = getSitesforVariables(variable);
    for(int site: sites){
      int last_commit = site_map[site] -> getLastCommittedTimestamp(variable);
      if(last_commit > endTransaction.begin_time){
        is_safe = false;
        break;
      }
    }
  }

  // 3rd Safety check: Acyclic Serialization Graph
  for(auto transaction: active_transactions){
    if(endTransaction.transaction_name == transaction.second.transaction_name) continue;

    // adding "ww" edges
    for(auto var: transaction.second.variables_accessed){
      // Check if both transactions access variable var
      if(find(endTransaction.variables_accessed.begin(), endTransaction.variables_accessed.end(), var) !=  endTransaction.variables_accessed.end()){
        // Check transaction commits before endTransaction begins
        if(transaction.second.commit_time < endTransaction.begin_time){
          addEdge(transaction.second.transaction_name, endTransaction.transaction_name, "ww");
        }
      }
    }

    // adding "wr" edges
    for(auto var: transaction.second.variables_accessed){
      if(find(endTransaction.variables_accessed_for_read.begin(), endTransaction.variables_accessed_for_read.end(), var) !=  endTransaction.variables_accessed_for_read.end()){
        // Check transaction commits before endTransaction begins
        if(transaction.second.commit_time < endTransaction.begin_time){
          addEdge(transaction.second.transaction_name, endTransaction.transaction_name, "wr");
        }
      }
    }
    
    // adding "rw" edges
    for(auto var: transaction.second.variables_accessed_for_read){
      if(find(endTransaction.variables_accessed.begin(), endTransaction.variables_accessed.end(), var) !=  endTransaction.variables_accessed.end()){
        // Check transaction begins before end time of endTransaction
        if(transaction.second.begin_time < endTransaction.end_time){
          addEdge(transaction.second.transaction_name, endTransaction.transaction_name, "rw");
        }
      }
    }

  }

  if(hasTwoRwCycle()) is_safe = false;


  return is_safe;
}

  



void TransactionManager::processCommit(const string& txn_name, int time) {

  for (auto site: active_transactions[txn_name].sites_accessed) {
    site_map[site]->commitData(txn_name, time);
  }

  active_transactions[txn_name].commit_time = time;
}

void TransactionManager::processAbort(const string& txn_name, int time) {

 
}


int main(int argc, char *argv[]) {

  TransactionManager tm(argv[1]);
  return 0;
}