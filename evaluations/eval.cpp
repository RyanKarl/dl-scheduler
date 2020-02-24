#include <iostream>
#include <string>

#include "keywords.h"

using namespace std;

int main(int argc, char ** argv){
  //Input from stdin, output to stdout
  
  
  
  unsigned int n_abortions = 0;
  unsigned int n_terminations = 0;
  unsigned int n_preemptions = 0;
  unsigned int n_activations = 0;
  
  string line;
  while(cin >> line){
    if(line.find(ABORTIONS) != string::npos){
      n_abortions++;
      continue;
    }
    if(line.find(TERMINATE) != string::npos){
      n_terminations++;
      continue;
    }
    if(line.find(PREEMPT) != string::npos){
      n_preemptions++;
      continue;
    }
    if(line.find(ACTIVATIONS) != string::npos){
      n_activations++;
      continue;
    }
  }
  
  
  cout << "Activations: " << n_activations << endl;
  cout << "Abortions: " << n_abortions << endl;
  cout << "Terminations: " << n_terminations << endl;
  cout << "Preemptions: " << n_preemptions << endl;
}
