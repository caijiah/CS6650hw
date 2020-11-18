 #include "RM.h"

RM::RM() {
    rm_ip = "";
    rm_port = -1;
    num_kv_pairs = -1;
    base_key = -1;
}

RM::RM(std::string ip, int port, int n_kv, int bk) {
    rm_ip = ip;
    rm_port = port;
    num_kv_pairs = n_kv;
    base_key = bk;
}

void RM::SetRMIP(std::string ip) {
    rm_ip = ip;
}

void RM::SetRMPort(int port) {
    rm_port = port;
}

void RM::SetNumKvPairs(int n_kv) {
    num_kv_pairs = n_kv;
}

void RM::SetBaseKey(int bk) {
    base_key = bk;
}

std::string RM::GetRMIP() {
    return rm_ip;
}

int RM::GetRMPort() {
    return rm_port;
}

int RM::GetNumKvPairs() {
    return num_kv_pairs;
}

int RM::GetBaseKey() {
    return base_key;
}
