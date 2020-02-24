#ifndef TRAININGDATA_H
#define TRAININGDATA_H

/* TrainingData.h
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Derived from SimpleNeuralNetwork by Zehao Huang
 */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using std::string;
using std::vector;
using std::stringstream;
using std::ifstream;

class TrainingData
{
public:
	TrainingData(const string & filename);
	TrainingData(const TrainingData & rhs);
	TrainingData & operator=(const TrainingData& other);
	bool isEof(void) const
	{
		return m_trainingDataFile.eof();
	}
	vector<unsigned int> getTopology();

	// Returns the number of input values read from the file:
	unsigned int getNextInputs(vector<double> &inputVals);
	unsigned int getTargetOutputs(vector<double> &targetOutputVals);
	bool noMoreInput() const;
private:
	string m_filename;
	ifstream m_trainingDataFile;
};

#endif