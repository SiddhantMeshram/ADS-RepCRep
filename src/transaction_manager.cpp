#include <iostream>
#include <string>
#include <fstream>

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
    cout << line << endl;
  }

  input.close();
}

int main(int argc, char *argv[]) {

  TransactionManager tm(argv[1]);
  return 0;
}