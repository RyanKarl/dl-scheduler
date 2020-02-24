/* Net.cpp
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Derived from SimpleNeuralNetwork by Zehao Huang
 */

#include <cassert>
#include <iostream> //Might remove if able
#include <fstream>
#include <stdexcept>
#include <csignal>
#include "../includes/Net.h"

#define MAXLINE 256

using std::string;
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::endl;
using std::cout;
namespace py = boost::python;

// Number of training samples to average over
double Net::m_recentAverageSmoothingFactor = 100.0;

Net::Net(const py::list & list){
	vector<unsigned int> vals;
	for(int i = 0; i < len(list); i++){
			vals.push_back(boost::python::extract<unsigned int>(list[i]));
	}
	*this = Net(vals);
}

Net::Net(const vector<unsigned int> & topology): m_recentAverageError(1){
	initRandomNeurons(topology);
}

Net::Net(const string & filename): m_recentAverageError(1){
	
	bool gotTopology = false;
	string line;
	ifstream fin(filename);
	vector<unsigned int> topology;
	//Get the topology
	while(!gotTopology){
		if(fin.eof()){
			throw std::invalid_argument("No topology in file!");
		}
		getline(fin, line);
		if(fin.eof() || fin.fail() || (!line.size()) || (line[0] == '#')){
			continue;
		}
		unsigned int top_entry;
		stringstream lineStream(line);
		while(lineStream >> top_entry){
			if(lineStream.fail()){
				throw std::invalid_argument("Invalid topology");
			}
			topology.push_back(top_entry);
		}
		gotTopology = true;
	}
	//We have a topology vector, now initialize array
	if(topology.size() < 3){
		string s = "ERROR: Invalid topology size: " + topology.size();
		std::cerr << s << endl;
		throw std::invalid_argument(s);
	}
	/*
	else{
		cout << "Topology vector: ";
		for(auto & x : topology){
			cout << x << ' ';
		}
		cout << endl;
	}
	*/
	m_layers.resize(topology.size());
	for(unsigned int i = 0; i < topology.size(); i++){
		m_layers[i].reserve(topology[i]);
	}
	//Now iterate through the vectors and read into neurons
	//Workaround - not able to handle variable-length lines
	//TODO fix to take into account variable length and bias neuron
	string neuronData = "";
	string scrap;
	bool seenData = false;
	for(unsigned int i = 0; i < m_layers.size() && !fin.eof(); i++){
		//Subtract one here from the topology to account for not sending anything to the bias neuron in the next layer
		//In this constructor (from a file), the full size of the topology is accounted for
		//In the previous one, 1 bias neuron is added to each layer, but we remember that neuron in the file
		unsigned int numOutputs = (i == topology.size() - 1) ? 0 : topology[i + 1]-1;
		for(unsigned int j = 0; j < topology[i]; j++){
			
			if(fin.eof()){
				std::cerr << "Reached end of net input file!" << endl;
				break;
				/*
				string s = "Not enough neurons at end of file! neuron " + std::to_string(j) + " of layer " + std::to_string(i);
				throw std::invalid_argument(s);
				*/
			}
			
			if(fin.fail()){
				string s = "Bad input to fin! neuron " + std::to_string(j) + " of layer " + std::to_string(i);
				throw std::invalid_argument(s);
			}
			//Skip over comments
			do{
				getline(fin, neuronData);
			} while((!fin.eof()) && (neuronData[0] == '#'));

			if(fin.eof()){
				break;
			}

			if(neuronData == ""){
				throw std::runtime_error("neuronData is bad!");
			}
			//Passing a stream containing only a single line
			stringstream ss(neuronData);
			Neuron n(numOutputs, j);
			ss >> n;
			m_layers[i].push_back(n);
			seenData = true;
		}
	}


	if(!seenData){
		for(auto & x : topology){
			if(!x){
				throw std::invalid_argument("Zero-neuron layer argued");
			}
			//Trying to handle bias neuron - remove?
			//x--;
		}
		initRandomNeurons(topology);
	}
}

void Net::initRandomNeurons(const vector<unsigned int> & topology){
	unsigned int numLayers = topology.size();
	m_layers.clear();
	m_layers.reserve(topology.size());
	for(unsigned int layerNum = 0; layerNum < numLayers; ++layerNum){
		m_layers.push_back(Layer());
		m_layers.back().reserve(topology[layerNum]);
		// numOutputs of layer[i] is the numInputs of layer[i+1]
		// numOutputs of last layer is 0
		unsigned int numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

		// We have made a new Layer, now fill it ith neurons, and
		// add a bias neuron to the layer:
		for(unsigned int neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum){
			//Change last arg to false to prevent random weight assignment
			m_layers.back().push_back(Neuron(numOutputs, neuronNum, true));
#ifdef DEBUG			
			//std::cout << "New neuron: " << layerNum << ' ' << neuronNum << endl;
#endif			
		}

		// Force the bias node's output value to 1.0. It's the last neuron created above
		m_layers.back().back().setOutputVal(1.0);
	}
}
	


vector<double> Net::getResults() const
{
	vector<double> resultVals;
	unsigned int outputSize = m_layers.back().size() - 1;
	resultVals.reserve(outputSize);
	for(unsigned int n = 0; n < outputSize; n++)
	{
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}
	return resultVals;
}

unsigned int Net::numResults() const {
	return m_layers.back().size() - 1;
}

void Net::backProp(const std::vector<double> &targetVals)
{
	// Calculate overal net error (RMS of output neuron errors)

	Layer &outputLayer = m_layers.back();
	m_error = 0.0;
	long double delta;

	for(unsigned int n = 0; n < outputLayer.size() - 1; ++n)
	{
		delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta *delta;

	}

	assert(outputLayer.size() > 1);
	m_error /= outputLayer.size() - 1; // get average error squared
	assert(m_error >= 0);
	m_error = sqrt(m_error); // RMS

	// Implement a recent average measurement:

	m_recentAverageError = 
			(m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
			/ (m_recentAverageSmoothingFactor + 1.0);
	// Calculate output layer gradients
	//Bound here is n < outputLayer.size() - 1 to account for the bias node (in output layer???)
	for(unsigned int n = 0; n < outputLayer.size() - 1; ++n)
	{
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}
	// Calculate gradients on hidden layers
	//Bound here is m_layers.size() - 2 to account for already doing output gradients
	for(unsigned int layerNum = m_layers.size() - 2; layerNum > 0; --layerNum)
	{
		Layer &hiddenLayer = m_layers[layerNum];
		Layer &nextLayer = m_layers[layerNum + 1];

		for(unsigned int n = 0; n < hiddenLayer.size(); ++n)
		{
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}

	// For all layers from outputs to first hidden layer,
	// update connection weights

	for(unsigned int layerNum = m_layers.size() - 1; layerNum > 0; --layerNum)
	{
		Layer &layer = m_layers[layerNum];
		Layer &prevLayer = m_layers[layerNum - 1];

		for(unsigned int n = 0; n < layer.size() - 1; ++n)
		{
			layer[n].updateInputWeights(prevLayer);
		}
	}
}

void Net::feedForward(const vector<double> & inputVals)
{
	// Check the num of inputVals equal to neuronnum expect bias
	if(inputVals.size() != m_layers[0].size() - 1){
		std::cout << "inputVals size is " << inputVals.size() << ", expected " << m_layers[0].size() - 1 << std::endl;
		assert(inputVals.size() == m_layers[0].size() - 1);
	}	

	// Assign {latch} the input values into the input neurons
	for(unsigned int i = 0; i < inputVals.size(); ++i){
		m_layers[0][i].setOutputVal(inputVals[i]); 
	}

	// Forward propagate
	for(unsigned int layerNum = 1; layerNum < m_layers.size(); ++layerNum){
		Layer &prevLayer = m_layers[layerNum - 1];
		for(unsigned int n = 0; n < m_layers[layerNum].size() - 1; ++n){
			m_layers[layerNum][n].feedForward(prevLayer);
		}
	}
}


 void Net::writeToFile(const string & filename) const {
	ofstream outfile;
	outfile.open(filename);
	outfile << "# Topology of neural net" << endl;
	for(const auto & t : m_layers){
		outfile << t.size() << ' ';
	}
	outfile << endl << 
	"# Neuron weights and biases, see README.md for format" << endl;

	for(const auto & x: m_layers){
		for(const auto & y : x){
			//Using overloaded operator for Neuron
			outfile << y << endl;
		}
	}
	
	outfile << "# End of Neuron weights and biases";
	outfile.close();
}



//Returns the layer and position from the index
std::pair<unsigned int, unsigned int> Net::position(unsigned int neuronNum) const {
	unsigned int originalNum = neuronNum;
	unsigned int topSum = 0;
	std::pair<unsigned int, unsigned int> ret;
	for(unsigned int i = 0; i < m_layers.size(); i++){
		topSum += m_layers[i].size();
		if(neuronNum >= m_layers[i].size()){
			neuronNum -= m_layers[i].size();
		}
		else{
			ret.first = i;
			ret.second = neuronNum;
			return ret;
		}
	}
	if(topSum <= originalNum){
		throw std::invalid_argument(std::to_string(originalNum));
	}
	return ret;
}

void Net::printPositions(ostream & out) const {
	out << "Printing positions:" << std::endl;
	for(const auto & layer : m_layers){
		for(const auto & neuron : layer){
			auto pos = position(neuron.getIndex());
			out << pos.first << ',' << pos.second << ' ';
		}
		out << std::endl;
	}
	out << std::endl;
}

void Net::printTopology(ostream & out) const {
	out << "Printing topology:" << std::endl;
	for(const auto & layer : m_layers){
		out << layer.size() << ' ';
	}
	out << std::endl;
}

void Net::printAllNeurons(ostream & out) const {
	out << "Printing neurons with output vector sizes:" << std::endl;
	for(const auto & layer : m_layers){
		for(const auto & neuron: layer){
			out << neuron.numOutputs() << ' ';
		}
		out << std::endl;
	}
	out << std::endl;
}

vector<unsigned int> Net::getTopology() const {
	vector<unsigned int> v;
	v.reserve(m_layers.size());
	for(unsigned int i = 0; i < m_layers.size(); i++){
		v.push_back(m_layers[i].size());
	}
	return v;
}

void Net::pythonFeedForward(const py::list & l){
	vector<double> vals;
	for(int i = 0; i < len(l); i++){
			vals.push_back(boost::python::extract<double>(l[i]));
	}
	feedForward(vals);
}

py::list Net::pythonGetResults() const {
	py::list ret;
	auto results = getResults();
	for(const auto & x : results){
		ret.append(x);
	}
	return ret;
}

py::list Net::pythonGetTopology() const{
	auto top = getTopology();
	cout << endl;
	py::list ret;
	for(const auto & x : top){
		ret.append(x);
	}
	return ret;	
}