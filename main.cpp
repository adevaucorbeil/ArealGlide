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
#include "functions.cpp"

using namespace std;

int main(int argc, char * argv[])
{
  
  double CRSS=0;

  // Declaration of all the variables:
  vector<obstacle> obs; //Obstacle description (position, breaking angle)

  int n_obs; //Number of obstacles
  double num_dens; //Number density

  string input;
  int i;

  // Command line options:
  CLParser cmd_line(argc,argv,true);

  string temp;
  int num_set; // Only one and two sets of obstacles are considered here (--type 1 & --type 2)
  int angle_strong; //Breaking angle of the strong obstacles
  int angle_weak; // Breaking angle of the weak obstacles

  double strong_concentration; //Concentration of strong obstacles

  vector<int> broken_obs; // List of the broken obstacles;


  // The first step is to read the input file:

  input=cmd_line.get_arg("-i");

  readinput(obs,input);
  n_obs = obs.size();
  num_dens = 1/sqrt(n_obs);

  // The three first points have to be on the buttom and perfectly aligned in order to start with a straight dislocation line:

  obs[0].x=0.2;
  obs[0].y=0;

  obs[1].x=0.5;
  obs[1].y=0;

  obs[2].x=0.75;
  obs[2].y=0;

  // cout << "Input file read" << endl;

  // The dislocation is defined as a series (vector) of segments.

  vector<segment> dislocation;

  // To start with a straight dislocation, the segments are:
  segment dummy(0,1);
  dislocation.push_back(dummy);

  segment dummy1(1,2);
  dislocation.push_back(dummy1);

  segment dummy2(2,0,1);
  dislocation.push_back(dummy2);

  // cout << "The starting dislocation is:"<< endl;

  // for(int i=0;i<dislocation.size();i++)
  //   {
  //     cout << i << "\t" << dislocation[i].A << " " << dislocation[i].B << " " << dislocation[i].type << endl;
  //   }

  double sweptarea=0;
  double max_y=0;

  int n=0;

  string output;
  output=cmd_line.get_arg("-o");

  std::ofstream logfile;
  logfile.open(output.c_str());

  while(1)//(n<step)
    {
      n++;
#ifdef DEBUG
      cout << "-----------------------------------------------------------" << endl;
      cout << "Step " << n << endl;
#endif

      // Compute the stress required by every segment to meet the closest node:
      // Compute only the stress for the new segments (tau_meet == -1)

      for(int i=0;i<dislocation.size();i++)
  	{
  	  if(dislocation[i].tau_meet == -1)
  	    {
  	      stress_meet(dislocation[i],obs); // Compute tau_meet and store the obstacle that would be met.
	    }
  	}


      // Compute the Orowan stress:

       for(int i=0;i<dislocation.size();i++)
  	{
  	  if(dislocation[i].tau_orowan == -1)
  	    {
  	      stress_orowan(dislocation[i],obs); // Compute tau_orowan and store it
	    }
  	}

       // Compute the stress required to break every node of the dislocation:
       
       for(int i=0;i<dislocation.size();i++)
	 {
	   if (obs[dislocation[i].A].tau_break == -1)
	     {
	       if(i==0)
		 {
		   stress_break(dislocation[dislocation.size()-1],dislocation[i],obs);
		 }
	       else
		 {
		   stress_break(dislocation[i-1],dislocation[i],obs);
		 }
	     }
	 }

       //Find the step that requires the least stress:
       double min_tau=1;
       int action_type; //Type 1: Break a node, type 2: meet an obstacle, type 3: Orowan bypassing.
       int node_to_break;
       int node_to_meet;
       int node_to_bypass;
       int seg_to_break;
       int seg_meeting;
       int seg_orowan;
       int seg_to_join[2];

       for(int i=0;i<dislocation.size();i++)
	 {
	   // Breaking a node?
	   if(obs[dislocation[i].A].tau_break < min_tau)
	     {
	       min_tau = obs[dislocation[i].A].tau_break;
	       node_to_break = dislocation[i].A;
	       seg_to_join[1] = (i-1)*((int) i>0) + (dislocation.size()-1)*((int) i==0);
	       seg_to_join[2] = i;
	       action_type = 1;
	     }
	   
	   //Meeting a new obstacle?
	   if(dislocation[i].tau_meet < min_tau)
	     {
	       min_tau = dislocation[i].tau_meet;
	       seg_meeting = i;
	       node_to_meet = dislocation[i].meet_obs;
	       action_type = 2;
	     }

	   //Leaving an Orowan loop around an obstacle?
	   if(dislocation[i].tau_orowan < min_tau)
	     {
	       min_tau = dislocation[i].tau_orowan;
	       seg_orowan = i;
	       action_type = 3;
	     }
	 }

       // Perform the required action that requires the lowest stress to be achieved:
       
       if(action_type == 1)
	 {
#ifdef DEBUG
	   cout << "Break a node" << endl;
#endif
	   int break_output;
	   break_output = break_node(dislocation,obs, seg_to_join[1], seg_to_join[2]);
	   // Add the broken node to the list of broken nodes:
	   broken_obs.push_back(node_to_break);
	   if(break_output == -1) return -1;
	 }
       else if(action_type == 2)
	 {
	   int meet_node_output;
#ifdef DEBUG
	   cout << "Meet the node " << node_to_meet << " in " << seg_meeting << endl;
#endif

	   meet_node_output = meet_node(dislocation, obs, seg_meeting, node_to_meet);
	   
	   if (meet_node_output == -1) return 0;
	 }
       else if(action_type == 3)
	 {
#ifdef DEBUG
	   cout << "By-pass a node: " << endl;
#endif
	   int seg_to_join[2];
	   // The obstacle which is going to be by-passed is the obstacle that has the lowest tau_break.
	   // Which one of the two pinned obstacles has the lowest tau_break?

	   if(obs[dislocation[seg_orowan].A].tau_break > obs[dislocation[seg_orowan].B].tau_break)
	     {
	       seg_to_join[1] = seg_orowan;
	       seg_to_join[2] = seg_orowan + 1;
	     }
	   else if(obs[dislocation[seg_orowan].A].tau_break < obs[dislocation[seg_orowan].B].tau_break)
	     {
	       seg_to_join[1] = seg_orowan - 1;
	       seg_to_join[2] = seg_orowan;
	     }
	   else
	     {
	       if(obs[dislocation[seg_orowan].A].alpha > obs[dislocation[seg_orowan].B].alpha)
		 {
		   seg_to_join[1] = seg_orowan - 1;
		   seg_to_join[2] = seg_orowan;
		 }
	       else
		 {
		   seg_to_join[1] = seg_orowan;
		   seg_to_join[2] = seg_orowan + 1;		   
		 }
	     }
	   
	   //Deal with the case when the two segments to join are on both sides of a boundary:
	   
	   seg_to_join[1] = seg_to_join[1] - (int) dislocation.size()*floor((double) (seg_to_join[1])/dislocation.size());
	   seg_to_join[2] = seg_to_join[2] - (int) dislocation.size()*floor((double) (seg_to_join[2])/dislocation.size());

	   //That way, the obstacle to be broken is always the obstacle "A" of the segment seg_orowan. Now it comes down to breaking that node:
	   int break_out;
	   // Add the broken node to the list of broken nodes:
	   node_to_bypass = dislocation[seg_to_join[2]].A;

	   broken_obs.push_back(node_to_bypass);
	   break_out = break_node(dislocation,obs, seg_to_join[1], seg_to_join[2]);
	   if(break_out == -1) return -1;
	 }
       else
	 {
	   std::cout << "Error, can't determine what action to perform" << endl;
	   return 0;
	 }

       //Clean dislocation:

       int cleaning = clean_dislocation(dislocation,obs);
       if (cleaning == -1)
	 {
	   cout << "Cleaning failed" << endl;
	   return 0;
	 }
#ifdef DEBUG
       cout << "Step " << n << " completed" << endl;
       cout << "Dislocation:" << endl;
       for(int i=0;i<dislocation.size();i++)
       	 {
       	   cout << i << "\t" << dislocation[i].A << " " << dislocation[i].B << " " << dislocation[i].type << endl;
       	 }     
       cout << "Current step shear stress = " << min_tau << endl;
       cout << "CRSS = " << CRSS << endl;
#endif

       //Stress achieved:
       CRSS = max(CRSS,min_tau);
       

       //The loop breaks if the highest point touched by the dislocation is over y=0.95.
       
       for(int i=0;i<dislocation.size();i++)
	 {
	   if (obs[dislocation[i].A].y > max_y)
	     {
	       max_y=obs[dislocation[i].A].y;
	     }
	   if (obs[dislocation[i].B].y > max_y)
	     {
	       max_y=obs[dislocation[i].B].y;
	     }
	 }
       
       
       // Record each step in the logfile:
       
       logfile << n << "\t"; // Step

       // Dislocation line:
       for(int i=0; i<dislocation.size(); i++)
	 {
	   logfile << dislocation[i].A << " ";
	 }
       logfile << dislocation[dislocation.size()-1].B << " CRSS ";
       logfile << min_tau;
       // Node that has been broken (if exists):
       if(action_type == 1)
	 {
	   logfile << "\tBreak\t" << node_to_break;
	 }
       // Node that has been by-pass:
       else if(action_type == 3)
	 {
	   logfile << "\tBreak\t" << node_to_bypass;
	 }
       // Type of dislocation segment:
       bool f = true;
       for(int i=0; i<dislocation.size(); i++)
	 {
	   if (dislocation[i].type != 0)
	     {
	       if (f == true)
		 {
		   f = false;
		   logfile << "\tType\t";
		 }
	       logfile << i << " " << dislocation[i].type << " ";
	     }
	 }
       
       logfile << endl;
       
       
       // Check if the list of broken obstacles is consistent with the obstacles marked as dead (not alive):
       for(int i=0; i<broken_obs.size(); i++)
	 {
	   if(obs[broken_obs[i]].alive != false)
	     {
	       cout << "Obstacle " << broken_obs[i] << " should be marked broken!!" << endl;
	       return 0;
	     }
	 }
       
       if (max_y > 0.980)
	 {
	   cout << "The simulation ended correctly: CRSS=" << CRSS << endl;
	   break;
	 }
    }
  
  return 0;  
}
