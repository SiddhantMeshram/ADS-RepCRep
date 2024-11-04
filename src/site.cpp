#include "site.h"

Site::Site(const std::string &name) : name(name), status(true), last_down_timestamp(0) {}

int Site::readData(const std::string &variable) {
    return 0;
}

void Site::writeLocal(const std::string &variable, const std::string &transaction_name, int value) {
    
}

void Site::commitData(const std::string &transaction_name) {
   
}

bool Site::isUp() {
    return false;
}

void Site::setUp(bool status) {
}

void Site::setDown(bool status) {
}

