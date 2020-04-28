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

  cout << "Number of obstacles = " << numberObstacle << "\n";
  // cin >> numberObstacle;

  
  x = new double[numberObstacle];
  y = new double[numberObstacle];
  angle = new double[numberObstacle];

  int nset=3;

  vector<double> set_angle;
  vector<double> concentration;

  temp=cmd_line.get_arg("-a1");
  set_angle.push_back(atof(temp.c_str()));
  temp=cmd_line.get_arg("-c1");
  concentration.push_back((double) atof(temp.c_str())/((double) 100));

  temp=cmd_line.get_arg("-a2");
  set_angle.push_back(atof(temp.c_str()));                    
  temp=cmd_line.get_arg("-c2");
  concentration.push_back((double) atof(temp.c_str())/((double) 100));


  temp=cmd_line.get_arg("-a3");
  set_angle.push_back(atof(temp.c_str()));

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

  for(i=1;i<concentration.size();i++)
    {
      concentration[i] += concentration[i-1];
    }
  concentration.push_back(1);
   for(i=0;i<concentration.size();i++)
     {
       cout << "concentration[" << i << "] = " << concentration[i] << endl;
    }

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
