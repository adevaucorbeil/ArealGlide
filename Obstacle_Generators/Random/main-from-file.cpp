#include <iostream>
#include <math.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

class CLParser
{
public:

    CLParser(int argc_, char * argv_[],bool switches_on_=false);
    ~CLParser(){}

    string get_arg(int i);
    string get_arg(string s);

private:

    int argc;
    vector<string> argv;

    bool switches_on;
    map<string,string> switch_map;
};

CLParser::CLParser(int argc_, char * argv_[],bool switches_on_)
{
    argc=argc_;
    argv.resize(argc);
    copy(argv_,argv_+argc,argv.begin());
    switches_on=switches_on_;

    //map the switches to the actual
    //arguments if necessary
    if (switches_on)
    {
        vector<string>::iterator it1,it2;
        it1=argv.begin();
        it2=it1+1;

        while (true)
        {
            if (it1==argv.end()) break;
            if (it2==argv.end()) break;

            if ((*it1)[0]=='-')
                switch_map[*it1]=*(it2);

            it1++;
            it2++;
        }
    }
}

string CLParser::get_arg(int i)
{
    if (i>=0&&i<argc)
        return argv[i];

    return "";
}

string CLParser::get_arg(string s)
{
    if (!switches_on) return "";

    if (switch_map.find(s)!=switch_map.end())
        return switch_map[s];

    return "";
}

int main(int argc, char * argv[])
{
  int i,j;
  int numberObstacle;
  int percentCore;
  int numberCore;
  int numberFreeObs;
  int outsideCore;


  double* x = NULL;
  double* y = NULL;
  double* xCore = NULL;
  double* yCore = NULL;
  double* xObs = NULL;
  double* yObs = NULL;

  double maxRadius;
  double d;
  double h;

  string temp;
  string output;

  // Initialize random seed:
  srand ( time(NULL) );

  CLParser cmd_line(argc,argv,true);

  temp=cmd_line.get_arg("-n");
  numberObstacle = atoi(temp.c_str());

  cout << "Number of obstacles = " << numberObstacle << "\n";
  // cin >> numberObstacle;

  
  x = new double[numberObstacle];
  y = new double[numberObstacle];


  temp=cmd_line.get_arg("-r");
  maxRadius = atof(temp.c_str());

  cout << "Maximum allowed radius around cluster cores= " << maxRadius <<"\n";


  // Assign memory for the x and y arrays:

  
  temp=cmd_line.get_arg("-c");
  percentCore = atoi(temp.c_str());


  cout << "Percentage of cluster cores = " << percentCore <<"\n";

  numberCore = (int) floor(percentCore*numberObstacle/double (100));

  // Assign memory for the xCore and yCore arrays:

  xCore = new double[numberCore];
  yCore = new double[numberCore];

  numberFreeObs = numberObstacle - numberCore;

  xObs = new double[numberFreeObs];
  yObs = new double[numberFreeObs];
  

  // Generate a random distribution for the cluster cores:

  for(i=1;i<=numberCore;i++)
    {
      xCore[i] = (double) rand()/RAND_MAX;
      yCore[i] = (double) rand()/RAND_MAX;
    }

  // Generate random obstacles:

  i=1;
  while(i<=numberFreeObs)
    {

      xObs[i] = (double) rand()/RAND_MAX;
      yObs[i] = (double) rand()/RAND_MAX;


      //Assign outsideCore value. If the obstacle is within the limits of one core, outsideCore value is 0.
      outsideCore = 1;

      // Test if the generated obstacle is outside of the maxRadius limit:
      for(j=1;j<=numberCore;j++)
	{
	  d = sqrt(pow(xObs[i] - xCore[j],2) + pow(yObs[i] - yCore[j],2));

	  if (d <= maxRadius)
	    {
	      outsideCore = 0;
	      break;
	    }
	  else
	    {
	      outsideCore = 1;
	    }
	}

      // If the obstacle is within the limit, generate a new one, regenerate it otherwise.

      if(outsideCore == 0)
	{
	  i++;
	}
    }

  // The generate is done, output the result in a file:

  
  output=cmd_line.get_arg("-o");
  std::ofstream outputFile(output.c_str());

  for(i=1;i<=numberCore;i++)
    {
      x[i] = xCore[i];
      y[i] = yCore[i];
      outputFile << xCore[i] << "\t" << yCore[i] << "\n";
    }

  for (j=1;j<=numberFreeObs;j++)
    {
      i++;
      
      x[i] = xObs[j];
      y[i] = yObs[j];
      outputFile << xObs[j] << "\t" << yObs[j] << "\n";
    }

  cout << output.c_str() << " generated!!\n";

  return 0;
}
