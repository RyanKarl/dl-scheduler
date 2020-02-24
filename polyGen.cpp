#include <iostream>
#include <string>

using namespace std;

double poly(double x, double y, double z){
  //Modify polynomial here - should be positive for positive inputs, at least while we're learning with ReLU
  return (z*y) + (x*x) + (2*y) + 1;
}

int main(){
  const double start = 0;
  const double end = 5;
  const double delta = 0.5;
  cout << "topology: 3 2 1" << endl;
  for(double x = start; x < end; x += delta){
    for(double y = start; y < end; y += delta){
      for(double z = start; z < end; z += delta){
        cout << "in: " << x << ' ' << y << ' ' << z << endl;
        cout << "out: " << poly(x,y,z) << endl;
      }
    }
  }
}
