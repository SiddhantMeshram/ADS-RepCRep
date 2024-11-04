#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <cassert>

#include "transaction_manager.h"

using namespace std;

TransactionManager::TransactionManager(const string& file_name) {
  
  // Create Sites and initialize avriables if reqiuired.

  // Process Input.
  ProcessInput(file_name);
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
      assert("Unexpected command");
    }
  }

  input.close();
}

int main(int argc, char *argv[]) {

  TransactionManager tm(argv[1]);
  return 0;
}