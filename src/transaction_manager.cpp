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
      site_map[site_index]->addVariable(var_name, site_index * 10);
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
  while (getline(input, line)) {
    string command, var;
    unordered_set<char> delimiters{'(', ')', ','};
    vector<string> variables;

    for (int ii = 0; ii < line.size(); ++ii) {
        if (delimiters.find(line[ii]) != delimiters.end()) {
            line[ii] = ' ';
        }
    }

    istringstream iss(line);
    iss >> command;
    while (iss >> var) {
      variables.push_back(var);
    }

    if (command == "begin") {

    } else if (command == "R") {

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

int main(int argc, char *argv[]) {

  TransactionManager tm(argv[1]);
  return 0;
}