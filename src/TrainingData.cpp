/* TrainingData.cpp
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Derived from SimpleNeuralNetwork by Zehao Huang
 */

#include "../includes/TrainingData.h"

TrainingData::TrainingData(const string & filename)
{
    m_filename = filename;
    if(m_trainingDataFile.is_open()){
        m_trainingDataFile.close();
    }
    m_trainingDataFile.open(m_filename.c_str());
}

TrainingData::TrainingData(const TrainingData & rhs){
    m_filename = rhs.m_filename;
    if(m_trainingDataFile.is_open()){
        m_trainingDataFile.close();
    }
    m_trainingDataFile.open(m_filename.c_str());
}

TrainingData & TrainingData::operator=(const TrainingData& other){
    m_filename = other.m_filename;
    if(m_trainingDataFile.is_open()){
        m_trainingDataFile.close();
    }
    m_trainingDataFile.open(m_filename.c_str());
    return *this;
}

bool TrainingData::noMoreInput() const {
    return m_trainingDataFile.eof();
}

vector<unsigned int> TrainingData::getTopology()
{
    vector<unsigned int> topology;
	string line;
	string label;

	getline(m_trainingDataFile, line);
	stringstream ss(line);
	ss >> label;
	if(this->isEof() || label.compare("topology:") != 0)
	{
		abort();
	}

	while(!ss.eof())
	{
		unsigned int n;
		ss >> n;
		topology.push_back(n);
	}
	return topology;
}

//Returns 0 in case of error
unsigned int TrainingData::getNextInputs(vector<double> &inputVals)
{
    inputVals.clear();

    string line;
    getline(m_trainingDataFile, line);
    stringstream ss(line);

    string label;
    ss >> label;
    if(this->isEof() || label.compare("in:")){
        return 0;
    }

    double oneValue;
    while (ss >> oneValue) {
        inputVals.push_back(oneValue);
    }

    return inputVals.size();
}

//Returns 0 in case of error
unsigned int TrainingData::getTargetOutputs(vector<double> &targetOutputVals)
{
    targetOutputVals.clear();

    string line;
    getline(m_trainingDataFile, line);
    stringstream ss(line);

    string label;
    ss>> label;
    if(this->isEof() || label.compare("out:")){
        return 0;
    }
    double oneValue;
     while (ss >> oneValue) {
        targetOutputVals.push_back(oneValue);
     }
    

    return targetOutputVals.size();
}
