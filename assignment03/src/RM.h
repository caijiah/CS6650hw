#ifndef __RM_H__
#define __RM_H__

#include <string>
class RM {
private:
	std::string rm_ip;
	int rm_port;
    int num_kv_pairs;
    int base_key;
	// bool status;
public:
    RM();
    RM(std::string ip, int port, int n_kv, int bk);
    void SetRMIP(std::string ip);
    void SetRMPort(int port);
    void SetNumKvPairs(int port);
    void SetBaseKey(int bk);

    std::string GetRMIP();
    int GetRMPort();
    int GetNumKvPairs();
    int GetBaseKey();
};

#endif // #ifndef __RM_H__
