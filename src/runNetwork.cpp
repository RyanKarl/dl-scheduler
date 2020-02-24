/* runNetwork.cpp
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Frontend to train or use neural network
 */

#include <getopt.h>
#include <iostream>
#include <string>
#include <cassert>
#include "../includes/TrainingData.h"
#include "../includes/Net.h"

using namespace std;

static struct option longopts[] = {
	{ "infile", required_argument, nullptr, 'i' }, 
	{ "outfile", required_argument, nullptr, 'o' },
	{ "mode", required_argument, nullptr, 'm' },
	{ "eta", required_argument, nullptr, 'e' },
	{ "alpha", required_argument, nullptr, 'a' },
	{ "network", required_argument, nullptr, 'n' },
	{ "passes", required_argument, nullptr, 'p' }
};

enum RunMode {TRAIN, RUN};

struct Options{
	bool modeSet;
	RunMode mode;
	bool outSet;
	string outfile;
	bool inSet;
	string infile;
	bool setE;
	double e;
	bool setA;
	double a;
	bool loadNet;
	string netFile;
	int passes;
};		
//TODO write default struct constructor - forgot that existed
struct Options getFreshOpt(){
	struct Options o;
	o.setE = false;
	o.setA = false;
	o.modeSet = false;
	o.outSet = false;
	o.inSet = false;
	o.loadNet = false;
	o.passes = -1;
	return o;
}

template<typename T> void printVector(ostream & os, const vector<T> & v, char delim = ' '){
	for(auto & x : v){
		os << x << delim;
	}
	os << endl;
}

bool getOptions(int argc, char ** argv, Options & opts){
	int idx = 0;
	char c;
	string optarg_holder;
	while ((c = getopt_long(argc, argv, "i:o:m:e:a:n:p:", longopts, &idx)) != -1){
		switch(c){
			case 'i': {
				opts.inSet = true;
				opts.infile = optarg;
				break;
			}
			case 'o': {
				opts.outSet = true;
				opts.outfile = optarg;
				break;
			}
			case 'm': {
				opts.modeSet = true;
				optarg_holder = optarg;
				opts.mode = (optarg_holder =="TRAIN")? TRAIN : RUN;
				break;
			}
			case 'e': {
				opts.setE = true;
				opts.e = atof(optarg);
				break;
			}
			case 'a': {
				opts.setA = true;
				opts.a = atof(optarg);
				break;
			}
			case 'n': {
				optarg_holder = optarg;
				opts.netFile = optarg_holder;
				opts.loadNet = true;
				break;
			}
			case 'p': {
				opts.passes = atoi(optarg);
				break;
			}
			default: {
				cerr << "Unrecognized option: " << optarg << endl;
				return false;
			}
		}
	}
	return true;
}

//Require certain things to be set
//Currently, require both files as well as a mode to be set
bool checkOptions(struct Options & opts){
	if((!opts.inSet) || (!opts.outSet) || (!opts.modeSet)){
		return false;
	}
	return true;
}

void trainNet(const struct Options & opts, bool printAllOutput = false){
	//TODO: support continuation of training from previously trained net
	//Currently only supports training from scratch
	TrainingData td(opts.infile);
	vector<unsigned int> topology = td.getTopology();
	Net myNet(topology);
	if(opts.loadNet){
		myNet = Net(opts.netFile);
		if(myNet.getTopology() != topology){
			cerr << "Inconsistent topology between training data and saved net" << endl;
			assert(0);
			return;
		}
	}
	
	unsigned int numTrains = (opts.passes > 0)? (unsigned int) opts.passes : 1;
	vector<double> inputs, targets;
	vector<double> outputs;
	for(unsigned int i = 0; i < numTrains; i++){
		td = TrainingData(opts.infile);
		td.getTopology();
		unsigned int numPasses = 0;
		unsigned int numInputNeurons = topology[0];
		unsigned int numOutputNeurons = topology.back();
		while(!td.isEof()){
			numPasses++;

			//Get input data
			unsigned int inputResult = td.getNextInputs(inputs);
			if(!inputResult){
				//Reached end of file
				break;
			}
			if(inputResult != numInputNeurons){
				cerr << "Wrong number of inputs at pass " << numPasses << endl;
				break;
			}
			//Feed input data forwards through net
			myNet.feedForward(inputs);
			//Get results of the net
			//Get target outputs
			unsigned int targetsResult = td.getTargetOutputs(targets);
			if(targetsResult != numOutputNeurons || !targetsResult){
				cerr << "Wrong number of target outputs at pass " << numPasses << endl;
				break;
			}
			//Save outputs to print
			outputs = myNet.getResults();
			//Train net via backpropagation against targets
			myNet.backProp(targets);
			//Report expected and given
			if(printAllOutput){
				cout << "Expected: ";
				for(const auto & x : targets){
					cout << x << ' ';
				}

				cout << "Actual: ";
				for(const auto & y : outputs){
					cout << y << ' ';
				}
				cout << endl;
				//Report recent error
				cout << "Run: " <<  i << " Pass: " << numPasses << " error: "
					 << myNet.getRecentAverageError() << endl;
			}
			else{
				cout << myNet.getRecentAverageError() << endl;
			}
			
			
		}
	}

	myNet.writeToFile(opts.outfile);

	cout << "Training finished, results written to "
		 << opts.outfile << endl;
}

//Right now only takes input from cin - add additional file specs?
void runNet(const struct Options & opts, istream & is, ostream & os){
	Net myNet(opts.netFile);
	unsigned int numInputs;
	is >> numInputs;
	if(is.eof() || is.fail() || !numInputs){
		cout << numInputs << endl;
		cerr << "Invalid first line" << endl;
		return;
	} 
	string line;
	double d;
	vector<double> inputs, outputs;
	//unsigned int currLine = 2;
	//Read each line in
	while(getline(is, line)){
		stringstream ss(line);
		unsigned int lineCount = 1;
		inputs.clear();
		inputs.reserve(numInputs);
		//Read each input from the line
		while(ss >> d){
			/*
			if(lineCount >= numInputs){
				cerr << "ERROR: Wrong number (" << lineCount << ") of inputs at line " << currLine << endl;
				continue;
			}
			
			if(ss.eof() || ss.fail()){
				cerr << "ERROR: Invalid input at line " << currLine << endl;
				break;
			}
			*/
			inputs.push_back(d);
			lineCount++;
		}
		/*
		if(lineCount != numInputs){
			cerr << "ERROR: Wrong number (" << lineCount << ") of inputs at line " << currLine << endl;
			continue;
		}
		*/

		if(inputs.size() != numInputs){
			continue;
		}

		myNet.feedForward(inputs);
		outputs = myNet.getResults();
		printVector(os, outputs);

		//TODO more ways to show error?
		cout << "Final recent avg error: " 
			 << myNet.getRecentAverageError() << endl;

	}
}



int main(int argc, char ** argv){
	//Turn off immediate sync of IO
	ios_base::sync_with_stdio(false);
	//Get and check CLI options
	struct Options opts = getFreshOpt();
	if(!getOptions(argc, argv, opts) && checkOptions(opts)){
		return 0;
	}

	//Set scaling values for Neuron, if needed
	if(opts.setE){
		Neuron::setEta(opts.e);
	}
	if(opts.setA){
		Neuron::setAlpha(opts.a);
	}

	switch(opts.mode){
		case TRAIN:{
			trainNet(opts, false);
			break;
		}
		case RUN:{
			if(opts.outSet){
				ofstream fs(opts.outfile);
				runNet(opts, cin, fs);
			}
			else{
				runNet(opts, cin, cout);
			}
			break;
		}
	}

}