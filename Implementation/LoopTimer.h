#pragma once

#include <chrono>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <iostream>


class LoopTimer
{
private:
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::milliseconds Units;
	int iterations;

public:

	bool verbose;
	Clock::time_point start_time;
	Clock::time_point loop_start_time;
	Clock::time_point loop_end_time;

	Units avg_loop_duration;
	Units run_duration;

	LoopTimer();

	void init();
	void loop_start();
	void loop_end();
	bool check_duration(int);

	~LoopTimer();
};

#pragma once
