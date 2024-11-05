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

    } else if (command == "R") {
      processRead(params);
    } else if (command == "W") {

    } else if (command == "fail") {

    } else if (command == "recover") {

    } else if (command == "end") {

    } else if (command == "fail") {

    } else if (command == "dump") {

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
      cout << params[1] << ": " << site_map[ii]->readData(params[1]) << endl;
      break;
    }
  }

  return;
}

int main(int argc, char *argv[]) {

  TransactionManager tm(argv[1]);
  return 0;
}