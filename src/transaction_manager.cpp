#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <algorithm>

#include "transaction_manager.h"
#include "site.h"

using namespace std;

TransactionManager::TransactionManager() {
  
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
  ProcessInput();
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

void TransactionManager::ProcessInput() {

  string line;
  int timer = 0;
  while (getline(cin, line)) {
    if (line.empty() || line[0] == '/') {
      continue;
    }
    ++timer;
    string command, var;
    unordered_set<char> delimiters{'(', ')', ','};
    vector<string> params;

    for (size_t ii = 0; ii < line.size(); ++ii) {
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
      processRead(params, timer);
    } else if (command == "W") {
      processWrite(params, timer);
    } else if (command == "fail") {
      cout << "site " << params[0] << " fails" << endl;
      site_map[stoi(params[0])]->setDown(timer);
    } else if (command == "recover") {
      processRecover(stoi(params[0]), timer);
    } else if (command == "end") {
      if(active_transactions[params[0]].isaborted){
        continue;
      }
      string s = isSafeToCommit(params, timer);
      if (s.empty()) {
        cout << params[0] << " commits" << endl;
        // Commit.
        processCommit(params[0], timer);
      } else {
        // Abort.
        processAbort(params[0], s);
      }
    } else if (command == "dump") {
      dump();
    } else {
      assert(false && "Unexpected command");
    }
  }
}

void TransactionManager::processRecover(int site, int timer) {

  cout << "site " << site << " recovers" << endl;
  site_map[site]->setUp();

  if (recovery_map.find(site) == recovery_map.end()) {
    return;
  }

  cout << "Processing waiting requests on site: " << site_map[site]->getName() << endl;
  for (auto req : recovery_map[site]) {
    if (already_recovered_set.find(req) != already_recovered_set.end()) {
      continue;
    }

    if (req.size() == 3) {
      // It is a read request.
      readData(site, req[0], req[1], timer);
    } else {
      writeData(site, req[0], req[1], stoi(req[2]), timer);
    }

    already_recovered_set.insert(req);
  }

  recovery_map.erase(site);
}

void TransactionManager::addEdge(const string& source, const string& target, const string& type){
  Edge e(target, type);
  temp_graph[source].push_back(e);
}


bool TransactionManager::hasTwoRwCycle(){

  vector<string> path;

  for (auto node : temp_graph) {
      if (detectCycle(node.first, node.first, path)) {
          return true;
      }
  }
  return false;

}

bool TransactionManager::detectCycle(const string& startNode, const string& currentNode, vector<string>& path) {

    path.push_back(currentNode);
    for (const auto& edge : temp_graph[currentNode]) {
        if (edge.target == startNode && path.size() > 1) {
            // A cycle is detected
            if (analyzeCycle(path, edge.edge_type)) {
                return true;
            }
        } else if (std::find(path.begin(), path.end(), edge.target) == path.end()) {
            if (detectCycle(startNode, edge.target, path)) {
                return true;
            }
        }
    }

    path.pop_back();
    return false;
}

bool TransactionManager::analyzeCycle(const vector<string>& cycle, const string& lastEdgeType) {

    vector<string> edgeTypes;

    for (size_t i = 0; i < cycle.size(); i++) {
        const string& from = cycle[i];
        const string& to = cycle[(i + 1) % cycle.size()];
        for (const auto& edge : temp_graph[from]) {
            if (edge.target == to) {
                edgeTypes.push_back(edge.edge_type);
                break;
            }
        }
    }
    edgeTypes.push_back(lastEdgeType);

    // Check for consecutive "rw" edges
    for (int i = 0; i < (int) edgeTypes.size() - 1; i++) {
        if (edgeTypes[i] == "rw" && edgeTypes[i + 1] == "rw") {
            return true;
        }
    }
    return false;
}

void TransactionManager::readData(int site, const string& txn_name, const string& var, int timer) {

  Transaction t = active_transactions[txn_name];
  cout << var << ": " << site_map[site]->readData(var, txn_name, t.begin_time) << endl;
  active_transactions[txn_name].variables_accessed_for_read.insert(var);
  if (!active_transactions[txn_name].sites_accessed.count(site)) {
    active_transactions[txn_name].sites_accessed.insert({site, timer});
  }
}

void TransactionManager::writeData(int site, string txn_name, string var, int value, int timer) {

  site_map[site]->writeLocal(var, txn_name, value);
  if (!active_transactions[txn_name].sites_accessed.count(site)) {
    active_transactions[txn_name].sites_accessed.insert({site, timer});
  }
  active_transactions[txn_name].variables_accessed.insert(var);
  
  cout << "Transaction: " << txn_name << " Variable: " << var
          << " written locally with value: " << value << " on sites: "
          << site_map[site]->getName() << endl;
}

void TransactionManager::processRead(vector<string> params, int timer) {

  vector<int> sites = getSitesforVariables(params[1]);
  vector<int> sites_down;
  for (int ii : sites) {
    // from active_transactions find params[0] say t
    Transaction t;
    if (active_transactions.find(params[0]) == active_transactions.end()) {
      cout << "Unable to find this transaction in active transactions: "
            << params[0] << endl;
      return;
    }

    t = active_transactions[params[0]];

    // If the site failed between the last commit recorded on that site and
    // the time t began, we can't read from that site unless a commit happens.
    if (site_map[ii]->getLastCommittedTimestamp(params[1], t.begin_time) < site_map[ii]->last_down() &&
        site_map[ii]->last_down() < t.begin_time) {
      // If it is non-replicated variable, we can return whatever value is
      // present locally. For replicated variable, we need a commit to happen
      // for this variable on this site before allowing reads.
      if (sites.size() != 1) {
        continue;
      }
    }

    if (site_map[ii]->isUp()) {
      readData(ii, params[0], params[1], timer);
      return;
    } else {
      sites_down.push_back(ii);
    }
  }

  if (sites_down.size() > 0) {
    params.push_back(to_string(timer));
    string ret;
    for (int ii : sites_down) {
      recovery_map[ii].push_back(params);
      ret += site_map[ii]->getName() + " ";
    }

    cout << "Waiting for sites: " << ret << " to recover in order to process read request" << endl;
    return;
  }

  // If we come here, some site was up but each site failed between the last
  // commit recorded on that site and the time transaction began. In this case,
  // we need to abort.
  processAbort(params[0], "No safe site to read");
  return;
}

void TransactionManager::processBegin(const vector<string>& params, int timer) {
  Transaction newTransaction(params[0], timer);
  active_transactions[params[0]] = newTransaction;
}

void TransactionManager::processWrite(vector<string> params, int timer) {

  const string& txn_name = params[0];
  const string& var = params[1];
  int value = stoi(params[2]);
  vector<int> sites = getSitesforVariables(params[1]);
  bool no_writes = true;
  vector<int> sites_accessed_for_curr_write;
  vector<int> sites_down;
  for (int ii : sites) {
    if (site_map[ii]->isUp()) {
      site_map[ii]->writeLocal(var, txn_name, value);
      sites_accessed_for_curr_write.push_back(ii);
      if (!active_transactions[txn_name].sites_accessed.count(ii)) {
        active_transactions[txn_name].sites_accessed.insert({ii, timer});
      }
      active_transactions[txn_name].variables_accessed.insert(var);
      no_writes = false;
    } else {
      sites_down.push_back(ii);
    }
  }

  if (no_writes) {
    params.push_back(to_string(timer));
    string ret;
    for (int ii : sites_down) {
      recovery_map[ii].push_back(params);
      ret += site_map[ii]->getName() + " ";
    }

    cout << "Waiting for sites " << ret << " to recover in order to process write request" << endl;
    return;
  }

  string ret;
  for (int ii : sites_accessed_for_curr_write) {
    ret += site_map[ii]->getName() + " ";
  }

  cout << "Transaction: " << txn_name << " Variable: " << var
       << " written locally with value: " << value << " on sites: " << ret
       << endl;
}

void TransactionManager::dump() {
  for (auto& kv : site_map) {
    cout << (kv.second)->getName() << " - " << (kv.second)->getDump() << endl;
  }
}

string TransactionManager::isSafeToCommit(const vector<string>& params, int timer) {
  Transaction endTransaction;

  if (active_transactions.find(params[0]) == active_transactions.end()) {
    cout << "Unable to find this transaction in active transactions: "
          << params[0] << endl;
    return "No active transactions";
  }

  endTransaction = active_transactions[params[0]];
  endTransaction.end_time = timer;

  // 1st Safety Check: Available Copies Safe
  for(auto [site, earliest_access_time]: endTransaction.sites_accessed){
    if(site_map[site] -> last_down() > earliest_access_time){
      return "Failed Available Copies check";
    }
  }

  // 2nd Safety Check: Snapshot Isolation Safe
  for(auto variable: endTransaction.variables_accessed){
    vector<int> sites = getSitesforVariables(variable);
    for(int site: sites){
      int last_commit = site_map[site] -> getLastCommittedTimestamp(variable);
      if(last_commit > endTransaction.begin_time){
        return "Failed Snapshot Isolation check";
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

  if(hasTwoRwCycle()) return "Cycle with 2 consecutive RW Detected";


  return "";
}

  



void TransactionManager::processCommit(const string& txn_name, int time) {

  for (auto [site, earliest_access_time]: active_transactions[txn_name].sites_accessed) {
    site_map[site]->commitData(txn_name, time);
  }

  serialization_graph = temp_graph;

  active_transactions[txn_name].commit_time = time;
}

void TransactionManager::processAbort(const string& txn_name, string str) {

  cout << txn_name << " aborts, " << "WHY: " << str  << endl;

  active_transactions[txn_name].isaborted = true;
  temp_graph = serialization_graph;
}


int main(int argc, char *argv[]) {

  TransactionManager tm;
  return 0;
}