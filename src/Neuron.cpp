/* Neuron.cpp
 * Jonathan S. Takeshita, Changhao Chenli, Ryan Karl
 * Derived from SimpleNeuralNetwork by Zehao Huang
 */

#include "../includes/Neuron.h"
#include <math.h>
#include <iostream>
#include <csignal>

using std::cerr;
using std::endl;

double Neuron::eta = 0.015; // overall net learning rate
double Neuron::alpha = 0.5; // momentum, multiplier of last deltaWeight, [0.0..n]

Neuron::Neuron(unsigned int numOutputs, unsigned int myIndex, bool rand)
{
	m_outputWeights.reserve(numOutputs);
	for(unsigned int c = 0; c < numOutputs; ++c){
		m_outputWeights.push_back(Connection());
		if(rand){
			m_outputWeights.back().weight = randomWeight();
		}	
	}
	m_myIndex = myIndex;
	m_gradient = 1.0;
}


void Neuron::updateInputWeights(Layer &prevLayer)
{
	// The weights to be updated are in the Connection container
	// in the neurons in the preceding layer

	for(unsigned int n = 0; n < prevLayer.size(); ++n)
	{
		Neuron &neuron = prevLayer[n];
		double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

		double newDeltaWeight = 
				// Individual input, magnified by the gradient and train rate:
				(eta
				* neuron.getOutputVal()
				* m_gradient)
				// Also add momentum = a fraction of the previous delta weight
				+ (alpha
				* oldDeltaWeight);
		if(std::isnan(newDeltaWeight + neuron.m_outputWeights[m_myIndex].weight)){
	    	std::raise(SIGINT);
	  	}		
		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
	}
}
double Neuron::sumDOW(const Layer &nextLayer) const
{
	double sum = 0.0;

	// Sum our contributions of the errors at the nodes we feed
	//Loop goes to nextLayer.size()-1 to handle the biases
	for (unsigned int n = 0; n < nextLayer.size() - 1; ++n)
	{
		sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
		if(std::isnan(sum) || std::isinf(sum)){
			std::raise(SIGINT);
		}
	}

	return sum;
}

void Neuron::calcHiddenGradients(const Layer &nextLayer)
{
	double dow = sumDOW(nextLayer);
	if(std::isnan(dow * Neuron::transferFunctionDerivative(m_outputVal))){
		std::raise(SIGINT);
	}
	m_gradient = dow * Neuron::transferFunctionDerivative(m_outputVal);
	if(std::isnan(m_gradient) || std::isinf(m_gradient)){
		std::raise(SIGINT);
	}
}
void Neuron::calcOutputGradients(double targetVals)
{
	double delta = targetVals - m_outputVal;
	if(std::isnan(delta * Neuron::transferFunctionDerivative(m_outputVal))){
		std::raise(SIGINT);
	}
	m_gradient = delta * Neuron::transferFunctionDerivative(m_outputVal);
	if(std::isnan(m_gradient) || std::isinf(m_gradient)){
		std::raise(SIGINT);
	}
}

long double Neuron::transferFunction(long double x)
{
	//ReLU function
	return x > 0 ? x : 0.01*x;
	//Softplus
	//return log(1+exp(x));
	//Sigmoid
	//return 1.0/(1.0+exp(-1.0*x));
}

long double Neuron::transferFunctionDerivative(long double x)
{
	//ReLU derivative - change neg. derivative to avoid holes
	long double ret = x > 0 ? 1 : 0.01;
	if(std::isnan(ret) || std::isnan(x) || std::isinf(ret) ||std::isinf(x)){
		std::raise(SIGINT);
	}
	return ret;
	//Softplus derivative
	//return 1.0/(1.0+exp(-1.0*x));
	//Sigmoid derivative
	//long double d = transferFunction(x);
	//return d*(1-d);
}

void Neuron::feedForward(const Layer &prevLayer)
{
	long double sum = 0.0;
	long double delta;

	// Sum the previous layer's outputs (which are our inputs)
    // Include the bias node from the previous layer.

	for(unsigned int n = 0 ; n < prevLayer.size(); ++n)
	{
		delta = prevLayer[n].getOutputVal() * 
				 prevLayer[n].m_outputWeights[m_myIndex].weight;
		if(std::isnan(sum + delta)){
	    	std::raise(SIGINT);
	  	}
		sum += delta;		 
	}
	
	//cerr << "Neuron " << m_myIndex << " has sum " << sum << endl;
	m_outputVal = Neuron::transferFunction(sum);
	//cerr << "Transfer(output)= " << m_outputVal << endl;
}



string Neuron::toString() const {
	string s = "";
	for(const auto & x : m_outputWeights){
		s += std::to_string(x.weight) + " " + std::to_string(x.deltaWeight) + "  ";
	}
	s += std::to_string(m_outputVal) + "  " + std::to_string(m_gradient);
	return s;
}

ostream & operator <<(ostream& os, const Neuron & n){
	os << n.toString();
	return os;
}

//Pass an istream containing a single line
//Neuron must have been constructed before reading into!
//TODO enforce this condition? May already be enforced by compiler
//TODO rewrite with range loop once debugged
istream & operator >>(istream& is, Neuron & n){
	for(auto & x : n.m_outputWeights){
		is >> x.weight >> x.deltaWeight;
	}
	is >> n.m_outputVal >> n.m_gradient;
	return is;
}


void Neuron::setEta(double eta_in){eta = eta_in;}

void Neuron::setAlpha(double alpha_in){alpha = alpha_in;}

