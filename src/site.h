#ifndef SITE_H
#define SITE_H

#include <string>
#include "data_manager.h"

class Site {
public:
    Site(const std::string &name);
    int readData(const std::string &variable);
    void writeLocal(const std::string &variable, const std::string &transaction_name, int value); 
    void commitData(const std::string &transaction_name); 
    void addVariable(const std::string &variable, int value);

    bool isUp();
    void setUp();
    void setDown();

private:
    std::string name;
    // Site status Up or Down
    bool status; 
    int last_down_timestamp; 
    DataManager data_manager; 
};


#endif