//#include <string>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//#include <initializer_list>
//#include <limits>
#include <vector>
#include <tuple>
//#include <memory>

//using std::string;
using std::abs;
//using std::cin;
//using std::cout;
//using std::cerr;
//using std::endl;
//using std::ofstream;
using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;
using std::shared_ptr;
using std::make_shared;

#include "io.h"
#include "matrix.h"
#include "MyObject.h"

typedef unsigned int uint;
struct objPar
{
  int S;
  int right,left, up,down;
};

struct border
{ 
  int left,right, up,down;
};

int gemetry(const Matrix<uint> &l, int n, vector<shared_ptr<IObject>> &results);
int aroundInt(float n);
tuple<int,int> getCentre(const border b, const Matrix<uint> &l, int num);
float getAngle(float n);
int getRadiusMin(const border b, const Matrix<uint> &l, const int xc,const int yc);
int getRadiusMax(const border b, const Matrix<uint> &l, const int xc,const int yc);
int getCogs(const border b, const Matrix<uint> &l, const int xc,const int yc);



