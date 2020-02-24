#ifndef NEURON_H
#define NEURON_H

/* Neuron.h
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Derived from SimpleNeuralNetwork by Zehao Huang
 * Source files including Neuron.h gain access to Neuron, Connection, and Layer
 */

#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using std::vector;
using std::string;
using std::ostream;
using std::istream;
using std::istringstream;

struct Connection
{
	double weight;
	double deltaWeight;
	Connection(){
		weight = 1.0;
		deltaWeight = 1.0;
	}
};

class Neuron;

typedef vector<Neuron> Layer;

class Neuron
{
public:
	Neuron(unsigned int numOutputs, unsigned int myIndex, bool rand = false);
	void setOutputVal(double val) { m_outputVal = val; }
	double getOutputVal(void) const { return m_outputVal; }
	void feedForward(const Layer &prevLayer);
	void calcOutputGradients(double targetVals);
	void calcHiddenGradients(const Layer &nextLayer);
	void updateInputWeights(Layer &prevLayer);
	unsigned int getIndex() const {return m_myIndex;}
	unsigned int numOutputs() const {return m_outputWeights.size();}
	//JST
	string toString() const;
	static void setEta(double eta_in);
	static void setAlpha(double alpha_in);
	friend ostream& operator<<(ostream & os, const Neuron & n);
	friend istream& operator>>(istream & is, Neuron & n);
private:
	static double eta; // [0.0...1.0] overall net training rate
	static double alpha; // [0.0...n] multiplier of last weight change [momentum]
	static long double transferFunction(long double x);
	static long double transferFunctionDerivative(long double x);
	// randomWeight: 0 - 1
	static double randomWeight(void) { return rand() / double(RAND_MAX); }
	double sumDOW(const Layer &nextLayer) const;
	long double m_outputVal;
	vector<Connection> m_outputWeights;
	unsigned int m_myIndex;
	double m_gradient;
};


#endif
