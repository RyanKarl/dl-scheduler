/* Net.cpp
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Derived from SimpleNeuralNetwork by Zehao Huang
 * This is directly from the original SimpleNeuralNetwork code - intended to test compilation after I Frankenstein'd the original source
 */

#include <cassert>
#include <cmath>
#include "../includes/TrainingData.h"
#include "../includes/Net.h"

using namespace std;

void showVectorVals(string label, vector<double> &v)
{
	cout << label << " ";
	for(unsigned int i = 0; i < v.size(); ++i)
	{
		cout << v[i] << " ";
	}
	cout << endl;
}

void originalTest(){
	TrainingData trainData("trainingData.txt");
	//e.g., {3, 2, 1 }
	vector<unsigned int> topology;
	//topology.push_back(3);
	//topology.push_back(2);
	//topology.push_back(1);

	//This line modified so it compiles with changed interface
	//trainData.getTopology(topology);
	topology = trainData.getTopology();
	Net myNet(topology);

	vector<double> inputVals, targetVals, resultVals;
	int trainingPass = 0;
	while(!trainData.isEof())
	{
		++trainingPass;
		cout << endl << "Pass" << trainingPass;

		// Get new input data and feed it forward:
		if(trainData.getNextInputs(inputVals) != topology[0])
			break;
		showVectorVals(": Inputs :", inputVals);
		myNet.feedForward(inputVals);

		// Collect the net's actual results:
		resultVals = myNet.getResults();
		showVectorVals("Outputs:", resultVals);

		// Train the net what the outputs should have been:
		trainData.getTargetOutputs(targetVals);
		showVectorVals("Targets:", targetVals);
		assert(targetVals.size() == topology.back());

		myNet.backProp(targetVals);

		// Report how well the training is working, average over recnet
		cout << "Net recent average error: "
		     << myNet.getRecentAverageError() << endl;
	}

	cout << endl << "Done" << endl;
}

int testFileOutput(int argc, char ** argv){
	if (argc != 2){
		cout << "Expected a single output filename for this test" << endl;
		return 0;
	}

	vector<unsigned int> topology;
	topology.push_back(5);
	topology.push_back(3);
	topology.push_back(2);
	topology.push_back(1);

	Net myNet(topology);

	myNet.writeToFile(argv[1]);

	return 0;
}

void testFileIO(){
	string outfileA = "out_A.txt";
	string outfileB = "out_B.txt";
	string outfileC = "out_C.txt";
	ofstream logfile("top.txt");


	vector<unsigned int> topology;
	topology.push_back(5);
	topology.push_back(3);
	topology.push_back(2);
	topology.push_back(1);

	Net netA(topology);
	//netA.printPositions(cout);
	netA.printAllNeurons(logfile);
	netA.writeToFile(outfileA);

	Net netB(outfileA);
	//netB.printPositions(cout);
	netB.printAllNeurons(logfile);
	netB.writeToFile(outfileB);

	/*
	Net netC(outfileB);
	netC.printPositions(cout);
	netC.writeToFile(outfileC);
	*/
}

void testRandNet(){
	string infileA = "in_A.txt";
	string outfileA = "out_A.txt";

	Net netA(infileA);
	netA.writeToFile(outfileA);

}

/*
int main(){
	testFileIO();
}
*/

