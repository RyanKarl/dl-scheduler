#ifndef NET_H
#define NET_H

/* Net.h
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Derived from SimpleNeuralNetwork by Zehao Huang
 */

#include <utility>
#include <boost/python.hpp>
#include "../includes/Neuron.h"

namespace py = boost::python;

class Net
{
public:
	Net(const vector<unsigned int> &topology);
	Net(const string & filename);
	Net(const py::list & list);
	//void initFromTopology(const vector<unsigned int> & topology);
	void feedForward(const vector<double> &inputVals);
	void backProp(const vector<double> &targetVals);
	vector<double> getResults() const;
	double getRecentAverageError(void) const { return m_recentAverageError; }
	void writeToFile(const string & filename) const ;
	vector<unsigned int> getTopology() const;
	void printPositions(ostream & out) const;
	void printTopology(ostream & out) const;
	void printAllNeurons(ostream & out) const ;
	unsigned int numResults() const;

	void pythonFeedForward(const py::list & l);
	py::list pythonGetResults() const;
	py::list pythonGetTopology() const;

private:
	std::pair<unsigned int, unsigned int> position(unsigned int neuronNum) const;
	void initRandomNeurons(const vector<unsigned int> & topology);

	vector<Layer> m_layers; //m_layers[layerNum][neuronNum]
	long double m_error;
	long double m_recentAverageError;
	static double m_recentAverageSmoothingFactor;
};


#endif
