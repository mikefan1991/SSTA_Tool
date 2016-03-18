#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;
struct GATE
{
  string type;
  int model;
  double cost;
  double d0;
  double d1;
  double d2;
  double dR;
};

struct WIRE
{
  double d0;
  double d1;
  double d2;
  double dR;
  int start;
  int stop;
};

struct VERTEX
{
  int index;
  double a0;
  double a1;
  double a2;
  double aR;
  int dad;
  GATE *gate;
  double cost;
  vector<int> in;
  vector<int> out;
};

vector<GATE> gate_lib;
