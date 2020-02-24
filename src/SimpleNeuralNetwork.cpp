#include "../includes/Neuron.h"
#include "../includes/TrainingData.h"
#include "../includes/Net.h"
//Boost.Python stuff
#include <boost/python.hpp>
#include <Python.h>
using namespace boost::python;

BOOST_PYTHON_MODULE(SimpleNeuralNetwork){
    class_<Neuron>("Neuron", init<unsigned int, unsigned int, bool>())
            .def("setOutputVal", &Neuron::setOutputVal)
            .def("getOutputVal", &Neuron::getOutputVal)
            .def("feedForward", &Neuron::feedForward)
            .def("calcOutputGradients", &Neuron::calcOutputGradients)
            .def("calcHiddenGradients", &Neuron::calcHiddenGradients)
            .def("updateInputWeights", &Neuron::updateInputWeights)
            .def("getIndex", &Neuron::getIndex)
            .def("numOutputs", &Neuron::numOutputs)
            .def("toString", &Neuron::toString)
            .def("setEta", &Neuron::setEta)
            .def("setAlpha", &Neuron::setAlpha)
            ;
     class_<TrainingData>("TrainingData", init<const string &>())
            .def("isEof", &TrainingData::isEof)
            .def("getTopology", &TrainingData::getTopology)
            .def("getTargetOutputs", &TrainingData::getTargetOutputs)
            .def("getNextInputs", &TrainingData::getNextInputs)
            .def("noMoreInput", &TrainingData::noMoreInput)
            ;       
     class_<Net>("Net", init<const vector<unsigned int>>())
		.def(init<const string &>())
        .def(init<const boost::python::list &>())
		//.def("initFromTopology", &Net::initFromTopology)
		.def("feedForward", &Net::feedForward)
		.def("backProp", &Net::backProp)
		.def("getResults", &Net::getResults)
		.def("getRecentAverageError", &Net::getRecentAverageError)
		.def("writeToFile", &Net::writeToFile)
		.def("getTopology", &Net::getTopology)
		.def("printPositions", &Net::printPositions)
		.def("printTopology", &Net::printTopology)
		.def("printAllNeurons", &Net::printAllNeurons)
		.def("numResults", &Net::numResults)
        .def("pythonFeedForward", &Net::pythonFeedForward)
        .def("pythonGetResults", &Net::pythonGetResults)
        .def("pythonGetTopology", &Net::pythonGetTopology)
  ;       
}
