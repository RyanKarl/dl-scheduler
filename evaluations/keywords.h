#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <string>
#include <vector>

using std::string;
using std::vector;

const static string ABORTIONS = "aborted";
const static string TERMINATE = "Terminated";
const static string PREEMPT = "Preempted";
const static string ACTIVATIONS = "Activated";

vector<string> keywords(){
  vector<string> ret;
  ret.push_back("Activations:");
  ret.push_back("Abortions:");
  ret.push_back("Terminations:");
  ret.push_back("Preemtions:");
  return ret;
}

#endif
