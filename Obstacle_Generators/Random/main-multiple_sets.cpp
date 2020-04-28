#include <iostream>
#include <math.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
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

int readdistribution(string input, vector<double> &set_angle, vector<double> &concentration)
{
  int nset = 0;
  std::ifstream inputFile(input.c_str());
  
    if (inputFile.is_open())
    {
      while (1)
	{
	  string line;
	  getline(inputFile,line);
	  if (inputFile.good() == false) break;

	  std::stringstream stream(line);

	  double temp_angle;
	  double temp_concentration;
	  
	  stream >> temp_angle;
	  stream >> temp_concentration;

	  set_angle.push_back(temp_angle);
	  concentration.push_back((double) temp_concentration/((double) 100));
	}
      nset = set_angle.size();

       for(int i=1;i<concentration.size()-1;i++)
	 {
	   concentration[i] += concentration[i-1];
	 }
       concentration[concentration.size()-1]=1;
    }

    for(int i=1;i<set_angle.size();i++)
      {
	cout << "angle set " << i << ":" << set_angle[i] << endl;
      }

    return nset;
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
  string idistribution;

  // Initialize random seed:
  srand ( time(NULL) );

  CLParser cmd_line(argc,argv,true);

  temp=cmd_line.get_arg("-n");
  numberObstacle = atoi(temp.c_str());

  cout << "Number of obstacles = " << numberObstacle << "\n";
  // cin >> numberObstacle;

  
  x = new double[numberObstacle];
  y = new double[numberObstacle];
  angle = new double[numberObstacle];

  int nset;

  vector<double> set_angle;
  vector<double> concentration;

  idistribution=cmd_line.get_arg("-d");

  nset = readdistribution(idistribution,set_angle,concentration);
  
  // for(i=1;i<=nset;i++)
  //   {
  //     double temp_angle;
  //     double temp_concent;
  //     if (i==1)	cout << "Breaking angle of the 1st set: ";
  //     if (i==2) cout << "Breaking angle of the 2nd set: ";
  //     if (i==3) cout << "Breaking angle of the 3rd set: ";
  //     if (i>=4) cout << "Breaking angle of the " << i << "th set: ";
  //     cin >> temp_angle;
  //     set_angle.push_back(temp_angle);
  //     if (i!=nset)
  // 	{
  // 	  cout << "and it's concentration (in %): ";
  // 	  cin >> temp_concent;
  // 	  concentration.push_back((double) temp_concent/((double) 100));
  // 	}
  //   }

  // for(i=1;i<concentration.size();i++)
  //   {
  //     concentration[i] += concentration[i-1];
  //   }
  // concentration.push_back(1);
  //  for(i=0;i<concentration.size();i++)
  //    {
  //      cout << "concentration[" << i << "] = " << concentration[i] << endl;
  //   }

  // Generate a random distribution:
  
  for(i=1;i<=numberObstacle;i++)
    {
      x[i] = (double) rand()/RAND_MAX;
      y[i] = (double) rand()/RAND_MAX;
      double tmp_angle = set_angle[0];
      for(int k=0;k<nset;k++)
	{
	  if((double)i/((double)numberObstacle) <= concentration[k])
	    {
	      tmp_angle = set_angle[k]*M_PI/((double)180);
	      break;
	    }
	}
      angle[i] = tmp_angle;
    }
  
  
  output=cmd_line.get_arg("-o");
  std::ofstream outputFile(output.c_str());

  for(i=1;i<=numberObstacle;i++)
    {
      outputFile << x[i] << "\t" << y[i] << "\t" << angle[i] << "\n";
    }

  cout << output.c_str() << " generated!!\n";

  return 0;
}
