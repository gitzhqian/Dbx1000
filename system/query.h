#pragma once

#include <atomic>
#include "global.h"
#include "helper.h"

class workload;
class ycsb_query;
class tpcc_query;
class tatp_query;

class base_query {
public:
	virtual void init(uint64_t thd_id, workload * h_wl) = 0;
	uint64_t waiting_time;
	uint64_t part_num;
	uint64_t * part_to_access;
#if WORKLOAD == TPCC && TPCC_SPLIT_DELIVERY
	uint32_t sub_query_id;
	uint32_t max_sub_query_id;
#endif
};

// All the querise for a particular thread.
class Query_thd {
public:
	void init(workload * h_wl, int thread_id);
	base_query * get_next_query(); 
	int q_idx;

#if WORKLOAD == YCSB
	ycsb_query * queries;
#elif WORKLOAD == TPCC
	tpcc_query * queries;
#elif WORKLOAD == TATP
	tatp_query * queries;
#endif
	char pad[CL_SIZE - sizeof(void *) - sizeof(int)];
	drand48_data buffer;
};

// TODO we assume a separate task queue for each thread in order to avoid 
// contention in a centralized query queue. In reality, more sofisticated 
// queue model might be implemented.
class Query_queue {
public:
	void init(workload * h_wl);
	void init_per_thread(int thread_id);
	base_query * get_next_query(uint64_t thd_id); 
	
private:
	static void * threadInitQuery(void * This);

	Query_thd ** all_queries;
	workload * _wl;
	static int _next_tid;
};
