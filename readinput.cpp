#include <iostream>
#include <math.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>

#include "header.h"
using namespace std;


void readinput(vector<obstacle> &obs, string input)
{
  int i;
    std::ifstream inputFile(input.c_str());

  if (inputFile.is_open())
    {
      i=0;
      while(inputFile.good())
	{
	  i++;
	  string line;
	  getline(inputFile,line);

	  std::stringstream stream(line);

	  // Get the obstacle coordinates from the line:
	  double x;
	  double y;
	  stream >> x;
	  stream >> y;
	  //	  std::cout << "Coordinate: x=" << x << " y=" << y << endl;

	  obstacle dummy;
	  dummy.x=x;
	  dummy.y=y;

	  obs.push_back(dummy);
	}
      inputFile.close();
      
      // for(int j=1;j<i;j++)
      //  	{
      //  	  std::cout << j << "x=" << obsX[j] << " y=" << obsY[j] << endl;
      // }

    }
  else std::cout << "Unable to open the input file " << input.c_str() << endl; 
}


