#include "RoundRobin.h"

/*
This is a constructor for RoundRobin Scheduler, you should use the extractProcessInfo function first
to load process information to process_info and then sort process by arrival time;

Also initialize time_quantum
*/
RoundRobin::RoundRobin(string file, int time_quantum)
{
	this->time_quantum = time_quantum;
	extractProcessInfo(file);
}

// Schedule tasks based on RoundRobin Rule
// the jobs are put in the order the arrived
// Make sure you print out the information like we put in the document
void RoundRobin::schedule_tasks()
{
	std::queue<shared_ptr<Process>> active_processes;

	int quantum_count = 0;
	for (int time = 0; active_processes.size() || processVec.size(); time++)
	{
		shared_ptr<Process> proc;

		// Add all available processes
		while (processVec.size() && (proc = processVec.front())->get_arrival_time() == time)
		{
			active_processes.push(proc);
			processVec.pop();
		}

		// Filter out all completed processes until the next one is available
		while (active_processes.size() && (proc = active_processes.front())->get_remaining_time() == 0)
		{
			RoundRobin::print(time, proc->getPid(), true);
			active_processes.pop();

			// reset quantum
			quantum_count = 0;
		}

		// Change process if quantum
		if (quantum_count == get_time_quantum())
		{
			if (active_processes.size() == 0)
			{
				printf("This shouldn't happen.");
				exit(1);
			}

			active_processes.push(active_processes.front());
			active_processes.pop();

			quantum_count = 0;
		}

		// Run next available proc
		if (active_processes.size())
		{
			proc = active_processes.front();

			proc->Run(1);
			RoundRobin::print(time, proc->getPid(), false);

			// Update quantum when running
			quantum_count++;
		}
		// Run NOP if there is no proc available
		else if (processVec.size())
		{
			RoundRobin::print(time, -1, false);
		}
	}
}

/***************************
ALL FUNCTIONS UNDER THIS LINE ARE COMPLETED FOR YOU
You can modify them if you'd like, though :)
***************************/

// Default constructor
RoundRobin::RoundRobin()
{
	time_quantum = 0;
}

// Time quantum setter
void RoundRobin::set_time_quantum(int quantum)
{
	this->time_quantum = quantum;
}

// Time quantum getter
int RoundRobin::get_time_quantum()
{
	return time_quantum;
}

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
void RoundRobin::extractProcessInfo(string file)
{
	// open file
	ifstream processFile(file);
	if (!processFile.is_open())
	{
		perror("could not open file");
		exit(1);
	}

	// read contents and populate process_info vector
	string curr_line, temp_num;
	int curr_pid, curr_arrival_time, curr_burst_time;
	while (getline(processFile, curr_line))
	{
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
