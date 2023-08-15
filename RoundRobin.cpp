#include "RoundRobin.h"

/*
	This is a constructor for RoundRobin Scheduler, you should use the extractProcessInfo function first
	to load process information to process_info and then sort process by arrival time;
	Also initialize time_quantum
*/
RoundRobin::RoundRobin(string file, int time_quantum) {
	set_time_quantum(time_quantum); // save time quantum
	extractProcessInfo(file); // extract file info into processes
}

// Schedule tasks based on RoundRobin Rule
void RoundRobin::schedule_tasks() {
	// 2 queues, rrqueue and pendingQ (processVec), copy processVect to pendingQueue
	queue<shared_ptr<Process>> rrQ;
	
	int currQuantCount = 0;
	// loop until rrqueue is empty (below signature means iterate so long as either has processes in it)
	for (int timestamp = 0; rrQ.size() || processVec.size(); timestamp++) {
		shared_ptr<Process> currProc;

		// for each itr, bring procs from pending to rr that have an arival time the same as the timestamp
		// while condition just making sure to go into procVec if procs inside
		while ((processVec.size() > 0) && (currProc = processVec.front())->get_arrival_time() == timestamp) {
			rrQ.push(currProc); // perror check?
			processVec.pop();
		}

		// go through rrQ, print out any completed processes at this time
		// and pop from rrQ (also reset time quantum count)
		while ((rrQ.size() > 0) && (currProc = rrQ.front())->get_remaining_time() == 0) {
			this->print(timestamp, currProc->getPid(), true);
			rrQ.pop();
			currQuantCount = 0; // reset (this quant count counts up versus typical down)
		}

		// if process runs for entire time quantum, push to end
		if (currQuantCount == this->get_time_quantum()) {
			rrQ.push(rrQ.front()); // no errors for if rrQ.size() == 0 should occur at this time, function should work always
			rrQ.pop();
			currQuantCount = 0;
		}

		// run and print next process for one second
		if (rrQ.size() > 0) {
			currProc = rrQ.front();
			currProc->Run(1); // run for 1 second
			this->print(timestamp, currProc->getPid(), false);
			// here, we don't process whether or not it finished
			// we simply run it, check if it finished in NEXT itr
			currQuantCount++;

		} else if (processVec.size() > 0) {
			// if nothing in rrQ, but stuff still in processVec / pending...
			// this is an NOP case
			this->print(timestamp, -1, false);
		}
	}
}
// you can modify the run function in Process.cpp to print out for every second, is possible (potentially recommended)

/*************************** 
extra functions 
***************************/

// Default constructor
RoundRobin::RoundRobin() { time_quantum = 0; }

// Time quantum setter
void RoundRobin::set_time_quantum(int quantum) { this->time_quantum = quantum; }

// Time quantum getter
int RoundRobin::get_time_quantum() { return time_quantum; }

// Print function for outputting system time as part of the schedule tasks function
void RoundRobin::print(int system_time, int pid, bool isComplete)
{
	string s_pid = pid == -1 ? "NOP" : to_string(pid);
	cout << "System Time [" << system_time << "].........Process[PID=" << s_pid << "] ";
	if (isComplete)
		cout << "finished its job!" << endl;
	else
		cout << "is Running" << endl;
}

// Read a process file to extract process information
// All content goes to proces_info vector
void RoundRobin::extractProcessInfo(string file) {
	// open file
	ifstream processFile(file);
	if (!processFile.is_open()) {
		perror("could not open file");
		exit(1);
	}

	// read contents and populate process_info vector
	string curr_line, temp_num;
	int curr_pid, curr_arrival_time, curr_burst_time;
	while (getline(processFile, curr_line)) {
		// use string stream to seperate by comma
		stringstream ss(curr_line);
		getline(ss, temp_num, ',');
		curr_pid = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_arrival_time = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_burst_time = stoi(temp_num);
		shared_ptr<Process> p(new Process(curr_pid, curr_arrival_time, curr_burst_time));
		processVec.push(p);
	}

	// close file
	processFile.close();
}
