#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <string>

using namespace std;

// Forward Declarations.
class Site;

class TransactionManager {

  public:
    TransactionManager(const string& file_name);
  
  private:
    void ProcessInput(const string& file_name);

    unordered_map<int, shared_ptr<Site>> site_map;
    unordered_map<string, int> variable_to_site_map;
};

#endif