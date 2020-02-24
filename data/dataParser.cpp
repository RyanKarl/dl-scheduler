#include <iostream>
#include <unordered_map>
#include <sstream>
#include <memory>
#include <array>
#include <stdexcept>
#include <vector>

using namespace std;

string execute(const char* cmd);

struct Process{
	//Desired output value
	double duration;
	//Input values
	unsigned int total_size;
	unsigned int text;
	unsigned int bss;
	unsigned int data;
	Process(): duration(-1), total_size(0), 
				text(0), bss(0), data(0){
	}
	bool isValid(){
		return (duration > 0) && (total_size) &&
				(text) && (bss);
	}
	void set_duration(double d){
		duration = d;
	}
	//Gets data using size call
	int get_stat(const string & command){
		stringstream cs(command);
		string aliased_command;
		cs >> aliased_command;
		string search = "which " + aliased_command;
		string bin_loc = execute(search.c_str());
		//Result now stores path to executable
		//TODO error handling
		string size_cmd = "size " + bin_loc;
		string result = execute(size_cmd.c_str());
		//No standard output returned in case of error
		if(!result.size()){
			return -1;
		}
		stringstream ss(result);
		string data, line;
		//Ignore first line
		getline(ss, line);
		//Second line has data
		ss >> text >> data >> bss >> total_size;
		return 0;
	}
	void shrink_inputs(){
#define CUT 1000
		total_size /= CUT;
		text /= CUT;
		bss /= CUT;
		data /= CUT;
	}
	int write_to_file(ostream & os){
		if(!isValid()){
			return -1;
		}
		shrink_inputs();
		os << "in: " << total_size << ' ' << text <<
			' ' << bss << ' ' << data << endl;
		os << "out: " << duration << endl; 	
		return 0;
	}
};

//From https://stackoverflow.com/questions/478898/
//how-to-execute-a-command-and-get-output-of-command-within-c-using-posix#478960
string execute(const char* cmd) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe){
    	throw std::runtime_error("popen() failed!");
    } 
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr){
            result += buffer.data();
        }
    }
    return result;
}
//Assumes we have called $ forkstat -e exec,exit
unsigned int parse_forkstat_to_file(istream & is, ostream & os, const vector<unsigned int> & topology){
	const string EXEC = "exec";
	const string EXIT = "exit";
	const string TIME = "Time";
	const string UNKNOWN = "unknown";
	string line, scrap, event, duration_string, command;
	unsigned int pid;
	double duration;
	//Remove first line of forkstat
	getline(is, scrap);
	//Send first line of training data (the topology)
	os << "topology: ";
	for(const auto & x : topology){
		os << x << ' ';
	}
	os << endl;
	//Iterate through input
	while(getline(is, line)){
		istringstream ss(line);
		//Time - test and discard
		ss >> scrap;
		if(scrap == TIME){
			continue;
		}
		//Event type
		ss >> event;
		//PID
		ss >> pid;
		//A process has exited
		if(event == EXIT){
			//Get info (discard) and duration in seconds
			ss >> scrap >> duration_string >> command;
			if(duration_string != UNKNOWN){
				//Create process
				//Assign duration and write to file
				Process proc;
				duration = atof(duration_string.c_str());
				proc.set_duration(duration);
				proc.get_stat(command);
				proc.write_to_file(os);
			}
			else{
				continue;
			}
		}
	}
	return 0;

}

int main(int argc, char ** argv){
	//Table may be large - allocate on heap
	//Take in topology as cmdline args
	vector<unsigned int> top;
	for(int i = 1; i < argc; i++){
		top.push_back((unsigned int) atoi(argv[i]));
	}
	

	parse_forkstat_to_file(cin, cout, top);

	return 0;
}