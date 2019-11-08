#pragma once
#include "LoopTimer.h"

LoopTimer::LoopTimer()
{
	verbose = false;
}

void LoopTimer::init()
{
	start_time = Clock::now();
	iterations = 0;
}

// Calculate time at which this loop iteration starts
void LoopTimer::loop_start()
{
	loop_start_time = Clock::now();
	++iterations;
}

// Calculate time at which this loop iteration ends and average time for one iteration
void LoopTimer::loop_end()
{
	loop_end_time = Clock::now();

	run_duration = std::chrono::duration_cast<Units>(loop_end_time - start_time);
	avg_loop_duration = std::chrono::duration_cast<Units>(run_duration / iterations);

	if (verbose)
	{
		printf("run_duration: %d\n", run_duration.count());
		printf("avg_loop_duration: %d\n", avg_loop_duration.count());
	}
}

// Check if total run duration exceeds max_millis
bool LoopTimer::check_duration(int max_millis)
{
	Clock::time_point next_loop_end_time = Clock::now() + avg_loop_duration;
	return next_loop_end_time - start_time > std::chrono::milliseconds(max_millis);
}

LoopTimer::~LoopTimer()
{
}
