# ------------------------------------------------
# Generic Makefile
#
# Original Author: yanick.rochon@gmail.com
# Date  : 2011-08-10
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
#   2017-04-24 - changed order of linker params
#		Oct 21 2018 - Rewrote for Neural Network
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = runNetwork

CC       = g++
CFLAGS   = -Wall -Werror -pedantic -std=c++14 -Wextra
CLIBS    = -I/usr/include/python2.7 -I/usr/include -lboost_python -lboost_system -lpython2.7

LINKER   = g++
LFLAGS   = $(CFLAGS)

SRCDIR   = src
INCDIR	 = includes
OBJDIR   = obj
BINDIR   = bin
PYOBJ    = SimpleNeuralNetwork
SIMSODIR = simso

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
rm       = rm -f

# location of the Python header files
PYTHON_VERSION = 2.7
PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)
# location of the Boost Python include files and library
BOOST_INC = /usr/include
BOOST_LIB = /usr/lib
# compile mesh classes

all: compile

compile: $(BINDIR)/$(TARGET)

shared: $(SIMSODIR)/$(PYOBJ).so

debug: CFLAGS += -g3 -DDEBUG
debug: clean all

#This may be outdated - newer profiling tools may be better
profile: CFLAGS += -pg
profile: clean all

release: CFLAGS += -O3 -DNDEBUG
release: clean all

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $(OBJECTS) $(LFLAGS) -o $@ $(CLIBS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(CLIBS) -I$(PYTHON_INCLUDE) -I$(BOOST_INC) -fPIC
	
#$(OBJDIR)/$(PYOBJ).o: $(SRCDIR)/$(PYOBJ).cpp
#	g++ -I$(PYTHON_INCLUDE) -I$(BOOST_INC) -fPIC -c $(PYOBJ).cpp -o $(PYOBJ).o
	
$(SIMSODIR)/$(PYOBJ).so: $(OBJDIR)/$(PYOBJ).o $(OBJECTS)
	g++ -o $(SIMSODIR)/$(PYOBJ).so -shared -Wl,--export-dynamic $(OBJDIR)/$(PYOBJ).o $(OBJDIR)/Net.o $(OBJDIR)/TrainingData.o $(OBJDIR)/Neuron.o -L$(BOOST_LIB) -lboost_python -L/usr/lib/python$(PYTHON_VERSION)/config -lpython$(PYTHON_VERSION)
	
#Individual dependencies
#TrainingData.o: TrainingData.h TrainingData.cpp
#Neuron.o: Neuron.h Neuron.cpp
#Net.o: Neuron.h Net.h Net.cpp
#compileTest.o: TrainingData.h Net.h	Neuron.h
#runNetwork.o: TrainingData.h Net.h Neuron.h
#boost_pyobjects.o: TrainingData.o Neuron.o Net.o

.PHONY: clean
clean:
	$(rm) $(OBJECTS) $(BINDIR)/$(TARGET) $(SIMSODIR)/$(PYOBJ).so

#.PHONY: remove
#remove: clean
#	@$(rm) $(BINDIR)/$(TARGET)
#	@echo "Executable removed!"
