#include "RoundRobin.h"

RoundRobin::RoundRobin(string file, int time_quantum) {
	set_time_quantum(time_quantum);
	extractProcessInfo(file);
}

void RoundRobin::schedule_tasks() {
	queue<shared_ptr<Process>> rrQ;
	
	int currQuantCount = 0;
	for (int timestamp = 0; rrQ.size() || processVec.size(); timestamp++) {

		shared_ptr<Process> currProc;
		while ((processVec.size() > 0) && (currProc = processVec.front())->get_arrival_time() == timestamp) {
			rrQ.push(currProc);
			processVec.pop();
		}

		while ((rrQ.size() > 0) && (currProc = rrQ.front())->get_remaining_time() == 0) {
			this->print(timestamp, currProc->getPid(), true);
			rrQ.pop();
			currQuantCount = 0; 
		}

		if (currQuantCount == this->get_time_quantum()) {
			rrQ.push(rrQ.front()); 
			rrQ.pop();
			currQuantCount = 0;
		}

		if (rrQ.size() > 0) {
			currProc = rrQ.front();
			currProc->Run(1); 
			this->print(timestamp, currProc->getPid(), false);
			currQuantCount++;

		} else if (processVec.size() > 0) {
			this->print(timestamp, -1, false);
		}
	}
}

/*************************** 
ALL FUNCTIONS UNDER THIS LINE ARE COMPLETED FOR YOU
You can modify them if you'd like, though :)
***************************/

RoundRobin::RoundRobin() {
	time_quantum = 0;
}

void RoundRobin::set_time_quantum(int quantum) {
	this->time_quantum = quantum;
}

int RoundRobin::get_time_quantum() {
	return time_quantum;
}

void RoundRobin::print(int system_time, int pid, bool isComplete) {
	string s_pid = pid == -1 ? "NOP" : to_string(pid);
	cout << "System Time [" << system_time << "].........Process[PID=" << s_pid << "] ";
	if (isComplete)
		cout << "finished its job!" << endl;
	else
		cout << "is Running" << endl;
}

void RoundRobin::extractProcessInfo(string file) {
	ifstream processFile(file);
	if (!processFile.is_open()) {
		perror("could not open file");
		exit(1);
	}
	string curr_line, temp_num;
	int curr_pid, curr_arrival_time, curr_burst_time;
	while (getline(processFile, curr_line)) {
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
	processFile.close();
}
