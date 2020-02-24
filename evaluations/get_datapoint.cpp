#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>

#include "keywords.h"

using namespace std;

void usage(){
  cout << "Provide a single keyword for input" << endl;
}

//Takes a list of files on standard input
int main(int argc, char ** argv){
  if(argc != 2){
    usage();
    return 0;
  }
  
  vector<string> kw = keywords();
  string word = argv[1];
  
  auto loc = find(kw.begin(), kw.end(), word);
  if(loc == kw.end()){
    cout << "Your keyword was invalid: " << word << endl;
    return 0;
  }
  
  string filename;
  unsigned long long i = 1;
  while(cin >> filename){
    std::fstream fs;
    fs.open(filename);
    if(!fs.is_open()){
      cerr << "Invalid filename: " << filename << endl;
      continue;
    }
    string s;
    while(fs >> s){
      if(s == word){
        unsigned int datapoint;
        fs >> datapoint;
        cout << i << ' '<< datapoint << '\n';
        i++;
      }
    }
    fs.close();
  }
  
  
  
  
  
  
  
  
}
