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
      site_map[stoi(params[0])]->setDown(timer);
    } else if (command == "recover") {
      site_map[stoi(params[0])]->setUp();
    } else if (command == "end") {
      processEnd(params, timer);
    } else if (command == "dump") {
      dump();
    } else {
      assert(false && "Unexpected command");
    }
  }

  input.close();
}

void TransactionManager::processRead(const vector<string>& params) {

  vector<int> sites = getSitesforVariables(params[1]);
  for (int ii : sites) {
    if (site_map[ii]->isUp()) {
      // from active_transactions find params[0] say t
      Transaction t;
      bool transaction_found = false;
      for(auto transaction : active_transactions){
        if(transaction.transaction_name == params[0]){
          t = transaction;
          transaction_found = true;
          break;
        }
      }
      if(transaction_found == false) cout << "Expected Transaction" << endl; 
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
  active_transactions.push_back(newTransaction);
}

void TransactionManager::processWrite(const vector<string>& params) {

  const string& txn_name = params[0];
  const string& var = params[1];
  int value = stoi(params[2]);
  vector<int> sites = getSitesforVariables(params[1]);
  bool no_writes = true;
  for (int ii : sites) {
    if (site_map[ii]->isUp()) {
      site_map[ii]->writeLocal(var, txn_name, value);
      no_writes = false;
    }
  }

  // TODO: Check what needs to be done if no site was up when a write came.
  if (no_writes) {

  }
}

void TransactionManager::dump() {
  for (auto& kv : site_map) {
    cout << (kv.second)->getName() << " - " << (kv.second)->getDump() << endl;
  }
}

void TransactionManager::processEnd(const vector<string>& params, int timer) {
  Transaction endTransaction;
  bool found = false;
  for(auto transaction: active_transactions){
    if(transaction.transaction_name == params[0]){
      endTransaction = transaction;
      found = true;
      break;
    }
  }
  if(!found){
    cout << "Transaction to end has not been found" << endl; 
  }

  bool is_safe = true;

  // 1st Safety Check: Available Copies Safe
  for(auto site: endTransaction.sites_accessed){
    if(site_map[site] -> last_down() > endTransaction.begin_time){
      is_safe = false;
      break;
    }
  }

  // 2nd Safety Check: Snapshot Isolation Safe
  for(auto variable: endTransaction.variables_acessed){
    vector<int> sites = getSitesforVariables(variable);
    for(int site: sites){
      int last_commit = site_map[site] -> getLastCommittedTimestamp(variable);
      if(last_commit > endTransaction.begin_time){
        is_safe = false;
        break;
      }
    }
  }


}

int main(int argc, char *argv[]) {

  TransactionManager tm(argv[1]);
  return 0;
}