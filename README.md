# dl-scheduler

Scheduler using deep learning and neural networks.

Network frontend args:
	-i: Specify the input file. Required.
	-o: Specify the output file. Required (right now, will support different i/o combinations later).
	-m: Mode. If "TRAIN", will run in training mode. If "RUN" (or anything else currently), will run in run mode.
	-e: Specify the eta hyperparameter.
	-a: Specify the alpha hyperparameter.

Input data format:
The first line of the file has a single positive integer, the number of inputs per line. Each line after that has the specified number of inputs. There are no comments.

TrainingData file format:
The first line starts with "topology: ", followed by a series of positive integers describing the topology of the net.
Afterwards, the lines come in in-out pairs. The first line will start with "in: ", followed by an appropriate number of inputs. The second line is similar.

Topology file format:
Comments are marked by a # at the start of the line.
Each file's first non-comment line will be the topology of the neural net. This is a list of positive integers that denotes the size of each layer. There must be at least 3 integers in this line.
After the topology line, the file may contain no more non-comment lines. If this is the case, then the neural net will be initialized to have random weights.
If not, then each line will have the values for a single neuron in the net. The lines will consist of space-delimited weight,deltaWeight pairs, themselves delimited by two spaces. These will be followed by the output value and gradient of the neural network, also two-space delimited.
Having fewer (but nonzero) neurons specified is an error. More neurons may cause undefined behavior.

Note that for when initializing with Net(const vector<unsigned int> & topology), an extra neuron ("bias neuron") will be added to each layer. (Is this how biases are done? Neurons don't have biases on their own...)
