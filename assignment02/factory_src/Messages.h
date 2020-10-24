#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>

class RobotOrder {
private:
	int customer_id;
	int order_number;
	int robot_type;

public:
	RobotOrder();
	void operator = (const RobotOrder &order) {
		customer_id = order.customer_id;
		order_number = order.order_number;
		robot_type = order.robot_type;
	}
	void SetOrder(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRobotType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};


class CustomerRequest {
private:
	// For robot order: customer id who placed order. 
	// For read request: customer id to read from map.
	int customer_id;
	// # of orders issued by this customer so far.
    // Record-read request does not count as an order.
	// Set to -1 for read requests.
	int order_number;
	// Either 1 - regular robot order request, or 
	// 		  2 - customer record read request
	int request_type;

public:
	CustomerRequest();
	void operator = (const CustomerRequest &request) {
		customer_id = request.customer_id;
		order_number = request.order_number;
		request_type = request.request_type;
	}
	void SetRequest(int cid, int order_num, int req_type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRequestType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class RobotInfo {
private:
	int customer_id;
	int order_number;
	int request_type;
	int engineer_id;
	int admin_id;

public:
	RobotInfo();
	void operator = (const RobotInfo &info) {
		customer_id = info.customer_id;
		order_number = info.order_number;
		request_type = info.request_type;
		engineer_id = info.engineer_id;
		admin_id = info.admin_id;
	}
	void SetInfo(int cid, int order_num, int req_type, int engid, int admid);
	void CopyRequest(CustomerRequest request);
	void SetEngineerId(int id);
	void SetAdminId(int id);

	int GetCustomerId();
	int GetOrderNumber();
	int GetRequestType();
	int GetEngineerId();
	int GetAdminId();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class CustomerRecord {
private:
	int customer_id;
	int last_order;
public:
	CustomerRecord();
	void operator = (const CustomerRecord &cus_record) {
		customer_id = cus_record.customer_id;
		last_order = cus_record.last_order;
	}
	void SetRecord(int cid, int lo);
	void SetCustomerId(int cid);
	void SetLastOrder(int lo);

	int GetCustomerId();
	int GetLastOrder();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

#endif // #ifndef __MESSAGES_H__
