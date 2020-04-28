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
  double* angle = NULL;

  string temp;
  string output;

  // Initialize random seed:
  srand ( time(NULL) );

  CLParser cmd_line(argc,argv,true);

  temp=cmd_line.get_arg("-n");
  numberObstacle = atoi(temp.c_str());

  if (numberObstacle==0)
    {
      cout << "Error can't identify the number of obstacle to generate. Use -n option to specify it." << "\n";
      return 0;
    }
  // cin >> numberObstacle;

  
  x = new double[numberObstacle];
  y = new double[numberObstacle];
  angle = new double[numberObstacle];

  // int nset;
  // cout << "Number of set of obstacles: ";
  // cin >> nset;

  // vector<double> set_angle;
  // vector<double> concentration;

  double temp_angle;
  temp= cmd_line.get_arg("--angle");
  temp_angle = atof(temp.c_str());

    // Generate a random distribution:

  for(i=1;i<=numberObstacle;i++)
    {
      x[i] = (double) rand()/RAND_MAX;
      y[i] = (double) rand()/RAND_MAX;

      angle[i] = temp_angle*M_PI/180;
    }
  
  output=cmd_line.get_arg("-o");
  std::ofstream outputFile(output.c_str());

  for(i=1;i<=numberObstacle;i++)
    {
      cout << x[i] << "\t" << y[i] << "\t" << angle[i] << "\n";
    }


  for(i=1;i<=numberObstacle;i++)
    {
      outputFile << x[i] << "\t" << y[i] << "\t" << angle[i] << "\n";
    }

  cout << output.c_str() << " generated!!\n";

  return 0;
}
