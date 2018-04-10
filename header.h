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

using namespace std;

#ifndef _OBSTACLE_STRUCT_
#define _OBSTACLE_STRUCT_
class obstacle
{
public:
  double x;  //Abscissa
  double y;  //Coordinate
  double angle; //Breaking angle in radians.

  double tau_break; //Force required to break the node
  double alpha; //Angle that makes two dislocations pinned to the obstacle.

  bool alive; // TRUE if the obstacle is not broken, FALSE otherwise

  obstacle();
};

obstacle::obstacle()
{
  x=0;
  y=0;
  angle = M_PI_2;

  tau_break=-1;
  alpha=-1;

  alive = true;
}

#endif

#ifndef _DISLOCATION_SEGMENT_
#define _DISLOCATION_SEGMENT_

class segment
{
  // The dislocation arc is defined as the clockwise rotation around
  // the center from it's point A to it's point B.
public:
  int A;
  int B;

  double tau_meet;
  double tau_cross;
  double tau_orowan;

  int loc_obs_to_meet; //Values: -1 (x in [-1;0]); 0 (x in [0;1]) and 1 (x in [1;2])

  int meet_obs;

  // Because of the periodic boundary conditions, the dislocation can
  // cross cell boundaries. There is therefore three types associated:
  // - type=0: the segment doesn't cross any cell limits
  // - type=1: the point A of the segment is on the left side of the boundary
  // - type=2: the point A of the segment is on the right side of the boundary

  int type;
  
  segment();
  segment(int,int);
  segment(int,int,int);
};

segment::segment()
{
  A=0;
  B=0;

  tau_meet=-1;
  tau_cross=-1;
  tau_orowan=-1;

  type=0;
}

segment::segment(int id_A, int id_B)
{
  A = id_A;
  B = id_B;

  tau_meet=-1;
  tau_cross=-1;
  tau_orowan=-1;

  type=0;
}

segment::segment(int id_A, int id_B, int typ)
{
  A = id_A;
  B = id_B;

  tau_meet=-1;
  tau_cross=-1;
  tau_orowan=-1;

  type=typ;
}

#endif

#ifndef _CRSS_
#define _CRSS_

class crss
{
public:
  double current;
  double max;
};

#endif

#ifndef _CL_PARSER_
#define _CL_PARSER_

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


#endif

#ifndef _READ_INPUT_
#define _READ_INPUT_

void readinput(vector<obstacle> &obs, string input);

double sweptarea(vector<segment> dislocation);

void stress_meet(segment &seg, vector<obstacle> &obs);
void stress_orowan(segment &seg,  vector<obstacle> &obs);
void stress_break(vector<segment> &dislo, vector<obstacle> &obs);

int break_node(vector<segment> &dislocation, vector<obstacle> &obs, int segment1, int segment2);
int meet_node(vector<segment> &dislocation, vector<obstacle> &obs, int seg_meeting, int node_to_meet);

int inside_triangle(double[], double[], double[], double[], bool);
int inside_loop(vector<segment> loop, vector<obstacle> &obs, int I);
int kill_obstacles_inside_loop(vector<segment> loop, vector<obstacle> &obs);

double crossprod(double[],double[]);
double dotprod(double[],double[]);
double angle(double v[],double w[]);

int sign(double);
int mod(int, int);

int clean_dislocation(vector<segment> &dislocation, vector<obstacle> &obs);

#endif

