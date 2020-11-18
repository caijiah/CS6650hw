#include <iomanip>
#include <iostream>

#include "2PCClientTimer.h"

ClientTimer::ClientTimer() {
	sum = duration<double, std::micro>(0);
	max = duration<double, std::micro>(0);
	min = duration<double, std::micro>(9999999999.9f);
	op_count = 0;
	commit_count = 0;
	abort_count = 0;
}

void ClientTimer::Start() {
	start_time = high_resolution_clock::now();
}

void ClientTimer::End() {
	auto end_time = high_resolution_clock::now();
	elapsed_time = (end_time - start_time);
}

void ClientTimer::EndAndMerge() {
	End();
	op_count++;
	sum += elapsed_time;
	if (elapsed_time < min) {
		min = elapsed_time;
	}
	if (elapsed_time > max) {
		max = elapsed_time;
	}
}

void ClientTimer::CommitIncrement() {
	commit_count++;
}

void ClientTimer::AbortIncrement() {
	abort_count++;
}

void ClientTimer::Merge(ClientTimer timer) {
	sum += timer.sum;
	op_count += timer.op_count;
	if (timer.min < min) {
		min = timer.min;
	}
	if (timer.max > max) {
		max = timer.max;
	}	
}

void ClientTimer::PrintStats() {
	std::cout << std::fixed << std::setprecision(3);
	std::cout << commit_count << "\t";
	std::cout << abort_count << "\t";
	std::cout << commit_count / (commit_count + abort_count) << "\t";
	std::cout << (commit_count + abort_count) / elapsed_time.count() * 1000000.0f << "\t";
	std::cout << commit_count / elapsed_time.count() * 1000000.0f << std::endl;
}

