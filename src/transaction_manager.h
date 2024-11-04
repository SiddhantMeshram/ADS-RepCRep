#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <string>

using namespace std;

class TransactionManager {

  public:
    TransactionManager(const string& file_name);
  
  private:
    void ProcessInput(const string& file_name);
};

#endif