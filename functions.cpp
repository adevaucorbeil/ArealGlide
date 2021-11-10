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
  std::ifstream inputFile(input.c_str());

  double min_angle=M_PI;

  // Add three obstacles onto which the dislocation is going to be pinned at the start:
  // The three first points have to be on the buttom and perfectly aligned in order to
  // start with a straight dislocation line:

  obstacle dummy;
  double x;
  double y;
  double angle;

  dummy.x = 0.2;
  dummy.y = 0;
  dummy.angle = min_angle;
  obs.push_back(dummy);

  dummy.x = 0.5;
  dummy.y = 0;
  dummy.angle = min_angle;
  obs.push_back(dummy);

  dummy.x = 0.75;
  dummy.y = 0;
  dummy.angle = min_angle;
  obs.push_back(dummy);

  string line;

  if (inputFile.is_open())
    {
      while (1)
	{
	  getline(inputFile,line);
	  if (inputFile.good() == false) break;

	  std::stringstream stream(line);

	  // Get the obstacle coordinates from the line:

	  stream >> x;
	  stream >> y;
	  stream >> angle;

	  dummy.x=x;
	  dummy.y=y;
	  dummy.angle=angle;
	  if (angle<min_angle) min_angle = angle;
	  obs.push_back(dummy);
	}
      inputFile.close();

      for(int j=0;j<3;j++)
	{
	  obs[j].angle=max(min_angle,M_PI*170/(double)180);
	}
      
      // for(int j=1;j<obs.size();j++)
      //  	{
      //  	  std::cout << j << "x=" << obs[j].x << " y=" << obs[j].y << endl;
      // 	}

    }
  else cout << "Unable to open the input file " << input.c_str() << endl;

  // Add one obstacle at the top to avoid premature end of the simulation:  
  dummy.x = 0;
  dummy.y = 0.99;
  dummy.angle = M_PI;
  obs.push_back(dummy);
}

double sweptarea(vector<segment> dislocation,vector<obstacle> obs)
{
  double area=0;
  int i;

  for(i=0;i<dislocation.size();i++)
    {
      if(dislocation[i].type==0)
	{
	  int id_A,id_B;
	  id_A = dislocation[i].A;
	  id_B = dislocation[i].B;
	  area += 0.5*(obs[id_B].x + obs[id_A].x)*(obs[id_B].y - obs[id_A].y) + obs[id_A].y*(obs[id_B].x - obs[id_A].x) ;
	}

      if(dislocation[i].type==1)
	{
	  int id_A,id_B;
	  id_A = dislocation[i].A;
	  id_B = dislocation[i].B;
	  area += 0.5*(obs[id_B].x+1 + obs[id_A].x)*(obs[id_B].y - obs[id_A].y) + obs[id_A].y*(obs[id_B].x+1 - obs[id_A].x) ;
	}    
 
      if(dislocation[i].type==2)
	{
	  int id_A,id_B;
	  id_A = dislocation[i].A;
	  id_B = dislocation[i].B;
	  area += 0.5*(obs[id_B].x-1 + obs[id_A].x)*(obs[id_B].y - obs[id_A].y) + obs[id_A].y*(obs[id_B].x-1 - obs[id_A].x) ;
	}   
    }

  return area;
}


void stress_meet(segment &seg, vector<obstacle> &obs)
{
  double x_center;
  double y_center;

  double Ax;
  double Ay=obs[seg.A].y;

  double Bx;
  double By=obs[seg.B].y;
  
  if(seg.type==1)
    {
      Ax = obs[seg.A].x;
      Bx = obs[seg.B].x +1;
    }
  else if(seg.type==2)
    {
      Ax = obs[seg.A].x;
      Bx = obs[seg.B].x - 1;
    }
  else
    {
      Ax = obs[seg.A].x;
      Bx = obs[seg.B].x;
    }

  // cout << "Ax = " << Ax <<" Bx = " << Bx << endl;
  // cout << "Ay = " << Ay <<" By = " << By << endl;

  double distAB2 = pow(Ax-Bx,2)+pow(Ay-By,2);
  double distAB = sqrt(distAB2);

  double num_dens = 1/sqrt(obs.size());

  int obs_to_meet=-1;
  double Rmax=0;

  int type_obstacle=0;
  // double node_x;
  // double node_y;

  //Coordinates of the center of the segment AB:
  x_center = 0.5*(Ax + Bx);
  y_center = 0.5*(Ay + By);

  // For all obstacles that are alive, find out if they could be meet by the segment seg and under which stress:
  for(int i=0;i<obs.size();i++)
    {
      for(int j=-1;j<2;j++)
	{
	  double x = obs[i].x + j; 
	  double y = obs[i].y;
	  double distCenter2;
	  double R;
	  
	  if((obs[i].alive==true) && (i!=seg.A) && (i!=seg.B))
	    {
	      distCenter2 = pow(x - x_center,2) + pow(y - y_center,2);
	      if (distCenter2 < 0.25*distAB2)
		{
		    {
		      double area = 0.5*fabs((Ax-x)*(By-Ay) - (Ax-Bx)*(y-Ay));
		      double distBX = sqrt(pow(x-Bx,2)+pow(y-By,2));
		      double distAX = sqrt(pow(x-Ax,2)+pow(y-Ay,2));
		      //Find the radius of the circle going through A, B and the obstacle (X):
		      R = distAB*distBX*distAX/(4*area);
		      if(R>Rmax)
			{
			  Rmax = R;
			  obs_to_meet = i;
			  type_obstacle = j;
			  // node_x = x;
			  // node_y = y;
			}
		    }
		}
	    }
	}
    }

  // Translate the distance in terms of stress:

  seg.tau_meet = num_dens/(2*Rmax);
  seg.meet_obs = obs_to_meet;
  seg.loc_obs_to_meet = type_obstacle;
  // cout << "Coordinates of the node that would be met: " << node_x << "\t" << node_y << endl;
  // cout << "Node that'd be met: " << obs_to_meet << endl;
  // cout << "Original coordinates of that obstacle: " << obs[obs_to_meet].x << "\t" <<  obs[obs_to_meet].y << endl;
}


void stress_orowan(segment &seg,  vector<obstacle> &obs)
{
  double num_dens = 1/sqrt(obs.size());
  double distAB;

  double xA;
  double yA=obs[seg.A].y;
  double xB;
  double yB=obs[seg.B].y;

  
  xA = obs[seg.A].x + 1*((int) (seg.type == 2));
  xB = obs[seg.B].x + 1*((int) (seg.type == 1));

  distAB = sqrt(pow(xA - xB,2) + pow(yA - yB,2));
  // cout << "distAB =" << distAB << endl;
  
  seg.tau_orowan = num_dens/(distAB);
}

void stress_break(segment seg1, segment seg2, vector<obstacle> &obs)
{
  double num_dens = 1/sqrt(obs.size());

  // Compute the two vectors v1 and v2 defined as:
  //                seg1          seg2
  //          * <----v1----  * ----v2----> *
  
  double v1[2];
  double v2[2];

  // cout << "Before correction: " << endl;
  
  // cout << "v1 = [" << obs[seg1.A].x - obs[seg1.B].x << ", " <<  obs[seg1.A].y-obs[seg1.B].y << "]" << endl;
  // cout << "v2 = [" << obs[seg2.B].x - obs[seg2.A].x << ", " << obs[seg2.B].y-obs[seg2.A].y << "]" << endl;
  
  // If i==0, the left segment is the last segment of the dislocation vector.
  // If the type of segment is 1, the abscissa of the point B must be increased by 1.
  // If the type of segment is 2, the abscissa of the point A must be increased by 1.

  v1[0] = obs[seg1.A].x - (obs[seg1.B].x + 1*((int) (seg1.type == 1)) - 1*((int) (seg1.type == 2)));
  v1[1] = obs[seg1.A].y - obs[seg1.B].y;
  
  v2[0] = obs[seg2.B].x + 1*((int) (seg2.type == 1)) - 1*((int) (seg2.type == 2)) - obs[seg2.A].x;
  v2[1] = obs[seg2.B].y - obs[seg2.A].y; 
  
  // cout << "v1 = [" <<v1[1] << ", " << v1[2] << "]" << endl;
  // cout << "v2 = [" <<v2[1] << ", " << v2[2] << "]" << endl;


  // Determine the stress needed to break the obstacle which connects the two segments:
  double R;
  double L1;
  double L2;
  double alpha;
  double v1v2sin_alpha;
  double f;
  double df;
  double err = 1;
  
  L1 = sqrt(pow(v1[0],2)+pow(v1[1],2));
  L2 = sqrt(pow(v2[0],2)+pow(v2[1],2));
  
  alpha = acos((v1[0]*v2[0]+v1[1]*v2[1])/(L1*L2));

  v1v2sin_alpha = v1[0]*v2[1] - v1[1]*v2[0];
  
  if(v1v2sin_alpha < 0) alpha = 2*M_PI - alpha;

  obs[seg2.A].alpha = alpha;

  // The function to solve is monotonous decreasing from R=1/2*max(|v1|,|v2|) to R=+inf.
  // A solution exists iff the function is positive for R=1/2*max(|v1|,|v2|):
  
  R = 1.001*0.5*max(L1,L2);
  // cout << "Guess R=" << R << endl;

  f = asin(L1/(2*R)) + asin(L2/(2*R)) + obs[seg2.A].angle + alpha - 2*M_PI;
  // cout << "Phi = " << obs[seg2.A].angle  << endl;

  if(f < 0)
    {
      obs[seg2.A].tau_break = 1e10;
      // cout << "V1 = [" << v1[1] << " " << v1[2] << "]" << endl;
      // cout << "V2 = [" << v2[1] << " " << v2[2] << "]" << endl;
      // cout << "R = " << R << endl;
      // cout << "L1 = " << L1 << endl;
      // cout << "L2 = " << L2 << endl;
      // cout << "alpha = " << alpha << endl;
      // cout <<  "Breaking angle = " << obs[seg2.A].angle << endl;
    }
  else
    {
      while(err > 0.001)
	{
	  f =  asin(L1/(2*R)) + asin(L2/(2*R)) + obs[seg2.A].angle + alpha - 2*M_PI;
	  df = -L1/R*1/sqrt(4*pow(R,2)-pow(L1,2)) - L2/R*1/sqrt(4*pow(R,2)-pow(L2,2));

	  R -= f/df;
	  err = fabs(f/df)/R;
	}
      obs[seg2.A].tau_break = num_dens/(2*R);
    }

  // cout << "Tau_break of obstacle "<< seg2.A <<"=" <<  obs[seg2.A].tau_break << endl;

}


int break_node(vector<segment> &dislocation, vector<obstacle> &obs, int segment1, int segment2)
{
  
  
  // New node A:
  int A = dislocation[segment1].A;
  double pointA[2]={obs[A].x,obs[A].y};
  
  // New node B:
  int B = dislocation[segment2].B;
  double pointB[2]={obs[B].x,obs[B].y};

  // Broken node:
  int P = dislocation[segment2].A;
  double pointP[2]={obs[P].x,obs[P].y};  
  obs[P].alive=false; // Marks the broken node as dead.

#ifdef DEBUG
  cout << "Enter break node" << endl;
  cout << "A=" << A << endl;
  cout << "Ax=" << pointA[0] << "; Ay=" << pointA[1] << endl;
  cout << "B=" << B << endl;
  cout << "P=" << P << endl;
#endif
  
  if (dislocation[segment1].B != dislocation[segment2].A)
    {
      for(int i=0;i<dislocation.size();i++)
	{
	  cout << i << "\t" << dislocation[i].A << " " << dislocation[i].B << " " << dislocation[i].type << endl;
	}
      cout << "Error: the two segments to remove are not connected to the broken node" << endl;
      return -1;
    }

  // The two tau_breaks and alphas of the two pinned obstacles of that segment have to be reinitialized to -1:
  obs[A].tau_break = -1;
  obs[B].tau_break = -1;
  obs[A].alpha = -1;
  obs[B].alpha = -1;


  if((A==B)&&(B==P))
    {
      // The dislocation is pinned by a single obstacle that is breaking. One must find the next pinning point:
      int point_to_add=-1;
      double min_dist=1;
      for(int i=0; i<obs.size(); i++)
	{
	  if((obs[i].alive==true)&&(obs[i].y>obs[P].y))
	    {
	      double dist = obs[i].y - obs[P].y;
	      if (dist<min_dist)
		{
		  point_to_add = i;
		  min_dist = dist;
		}
	    }
	}

      if(point_to_add==-1)
	{
	  cout << "Error, cannot find a point to pin the dislocation\n";
	  return -1;
	}
      else
	{
	  //Clear the dislocation:
	  dislocation.clear();
	  segment dummy_seg(point_to_add,point_to_add,1);
	  dislocation.push_back(dummy_seg);
	}
    }
  else
    {
      
      // Types:
      int seg1type = dislocation[segment1].type;
      int seg2type = dislocation[segment2].type;
      
      // What type is the new segment:
      int segnewtype = mod(seg1type + seg2type,3);
      
      // cout << "segnewtype=" << segnewtype << endl;
      pointB[0]=pointB[0]+1*(segnewtype==1)-1*(segnewtype==2);
      pointP[0]=pointP[0]+1*(seg1type==1)-1*(seg1type==2);
      // cout << "Bx=" << pointB[0] << "; By=" << pointB[1] << endl;
      // cout << "Px=" << pointP[0] << "; Py=" << pointP[1] << endl;
      
      // Use the rolling circle technique with straight lines to connect the points A and B:
      double AP[2]={pointP[0]-pointA[0],pointP[1]-pointA[1]};
      double AB[2]={pointA[0]-pointB[0],pointA[1]-pointB[1]};
      
      double theta_AP_AB = angle(AP,AB);
      
      vector<segment> segments_to_add;
      
      int m=0;
      while(1)
	{
	  m++;
	  // cout << "m=" << m << endl;
	  double alpha;
	  int type;
	  segment dummy_seg;
	  int new_seg=0;
	  
	  double pointS[2];
	  double pointT[2];
	  
	  int sum_type;
	  int T;
	  
	  if(segments_to_add.size()==0)
	    {
	      pointS[0]=pointA[0];
	      pointS[1]=pointA[1];
	      
	      T=P;
	      pointT[0]=pointP[0];
	      pointT[1]=pointP[1];
	      
	    }
	  else
	    {
	      sum_type=0;
	      for(int i=0;i<segments_to_add.size();i++)
		{
		  sum_type+=segments_to_add[i].type;
		}
	      int sum_prev_type = mod(sum_type - segments_to_add.back().type,3);
	      sum_type = mod(sum_type,3);
	      pointS[0]=obs[segments_to_add.back().A].x+1*(sum_prev_type==1)-1*(sum_prev_type==2);
	      pointS[1]=obs[segments_to_add.back().A].y;
	      
	      T=segments_to_add.back().B;
	      
	      pointT[0]=obs[segments_to_add.back().B].x+1*(sum_type==1)-1*(sum_type==2);
	      pointT[1]=obs[segments_to_add.back().B].y;
	      
	      // cout << "Tx=" << pointT[0] << "; Ty=" << pointT[1] << endl;
	    }
	  
	  double ST[2]={pointT[0]-pointS[0],pointT[1]-pointS[1]};
	  double TB[2]={pointB[0]-pointT[0],pointB[1]-pointT[1]};
	  double min_alpha=angle(ST,TB);
	  // cout << "min_alpha=" << min_alpha << endl;
	  // cout << "T=" << T << endl;
	  for(int i=0;i<obs.size();i++)
	    {
	      if((obs[i].alive==true) && (i!=A) && (i!=T))
		{
		  for(int j=-1;j<2;j++)
		    {
		      double pointI[2]={obs[i].x+j,obs[i].y};
		      
		      if(inside_triangle(pointI,pointA,pointP,pointB,false)==1)
			{
			  double AI[2] = {pointI[0]-pointA[0],pointI[1]-pointA[1]};
			  
			  if(segments_to_add.size()==0)
			    {
			      alpha=angle(AP,AI);
			      if(j==-1)
				{
				  type = 2;
				}
			      else
				{
				  type = j;
				}			  
			    }
			  else
			    {
			      double TI[2]={pointI[0]-pointT[0],pointI[1]-pointT[1]};
			      alpha=angle(ST,TI);
			      if(sum_type==0)
				{
				  type = j*(j!=-1)+2*(j==-1);
				}
			      else if(sum_type==1)
				{
				  if(j==-1)
				    {
				      cout << "Error incompatible types\n";
				      return -1;
				    }
				  type = 0*(j==1)+2*(j==0);
				}
			      else
				{
				  if(j==1)
				    {
				      cout << "Error incompatible types\n";
				      return -1;
				    }
				  
				  type = 1*(j==0)+0*(j==-1);
				}
			    }
			  
			  if(alpha<min_alpha)
			    {
			      new_seg=1;
			      
			      min_alpha=alpha; 
			      if(segments_to_add.size()==0)
				{
				  dummy_seg.A=A;
				  dummy_seg.B=i;
				  dummy_seg.type=type;
				}
			      else
				{
				  dummy_seg.A=segments_to_add.back().B;
				  dummy_seg.B=i;
				  dummy_seg.type=type;
				}
			    }
			}
		    }
		}
	    }
	  
	  if (new_seg==0)
	    {
	      if(segments_to_add.size()==0)
		{
		  dummy_seg.A=A;
		  dummy_seg.type=segnewtype;
		}
	      else
		{
		  dummy_seg.A=segments_to_add.back().B;
		  dummy_seg.type=mod(segnewtype-sum_type,3);
		}
	      dummy_seg.B=B;
	    }
	  
	  // cout << "Dummy_seg:" << dummy_seg.A << " " << dummy_seg.B << " " << dummy_seg.type << endl;
	  segments_to_add.push_back(dummy_seg);
	  
	  if(segments_to_add.back().B==B) break;
	  if(segments_to_add.size()==0)
	    {
	      cout << "ERROR no segments to add\n";
	      return -1;
	    }
	}

#ifdef DEBUG
      cout << "Segment to add:" << endl;
      for(int i=0;i<segments_to_add.size();i++)
      	{
      	  cout << i << "\t" << segments_to_add[i].A << " " << segments_to_add[i].B << " " << segments_to_add[i].type << endl;
      	}
#endif

      //Erase broken segments and add the segments_to_add to the dislocation list:
      if(segment1 == dislocation.size()-1)
	{
	  dislocation.pop_back();
	  dislocation.erase(dislocation.begin());
	}
      else
	{
	  dislocation.erase(dislocation.begin()+segment2);
	  dislocation.erase(dislocation.begin()+segment1);
	}  
      
      if (segment2==0)
	{
	  dislocation.insert(dislocation.end(),segments_to_add.begin(),segments_to_add.end());
	}
      else
	{
	  dislocation.insert(dislocation.begin()+segment1,segments_to_add.begin(),segments_to_add.end());
	}
    }
    
  return 0;
}


int meet_node(vector<segment> &dislocation, vector<obstacle> &obs, int seg_meeting, int node_to_meet)
{
  // cout << "Meet the node " << node_to_meet << " in " << seg_meeting << endl;
  segment seg1(dislocation[seg_meeting].A,node_to_meet);
  segment seg2(node_to_meet,dislocation[seg_meeting].B);
  // cout << "seg1:" << endl;
  // cout << dislocation[seg_meeting].A << "\t" << node_to_meet << "\t" << endl;
  // cout << "seg2:" << endl;
  // cout << node_to_meet << "\t" << dislocation[seg_meeting].B <<endl;
  
  // cout << dislocation[seg_meeting].loc_obs_to_meet << endl;
  // cout << "Segment type: " << dislocation[seg_meeting].type << endl;
  // cout << "Node to meet type: " << dislocation[seg_meeting].loc_obs_to_meet << endl;


  // Reset the taus and the alphas of the three obstacles:

  obs[seg1.A].tau_break = -1;
  obs[seg1.B].tau_break = -1;
  obs[seg2.B].tau_break = -1;

  obs[seg1.A].alpha = -1;
  obs[seg1.B].alpha = -1;
  obs[seg2.B].alpha = -1;


  //Changing type of segments:
  if (dislocation[seg_meeting].loc_obs_to_meet == -1)
    {
      if(dislocation[seg_meeting].type==0)
	{
	  seg1.type = 2;
	  seg2.type = 1;
	}
      else if(dislocation[seg_meeting].type==1)
	{
	  cout << "Error: type of the dislocation and type of the point to meet incompatible" << endl;
	  return -1;
	}
      else if(dislocation[seg_meeting].type==2)
	{
	  seg1.type = 2;
	  seg2.type = 0;	   
	}
    }
  else if (dislocation[seg_meeting].loc_obs_to_meet == 1)
    {
      if(dislocation[seg_meeting].type==0)
	{
	  seg1.type = 1;
	  seg2.type = 2;	  
	}
      else if(dislocation[seg_meeting].type==1)
	{
	  seg1.type = 1;
	  seg2.type = 0;
	}
      else if(dislocation[seg_meeting].type==2)
	{
	  seg1.type = 0;
	  seg2.type = 2;
	}
      else
	{
	  cout << "Error: type of the dislocation and type of the point to meet incompatible" << endl;
	  return -1;
	}
    }
  else if (dislocation[seg_meeting].loc_obs_to_meet == 0)
    {
      if(dislocation[seg_meeting].type==0)
	{
	  seg1.type = 0;
	  seg2.type = 0;
	}
      else if(dislocation[seg_meeting].type==1)
	{
	  seg1.type = 0;
	  seg2.type = 1;
	}
      else if(dislocation[seg_meeting].type==2)
	{
	  seg1.type = 0;
	  seg2.type = 2;
	}
      else
	{
	  cout << "Error: type of the dislocation and type of the point to meet incompatible" << endl;
	  return -1;
	}
    }

if (seg_meeting == dislocation.size()-1)
  {
    dislocation.pop_back();
    dislocation.push_back(seg1);
    dislocation.push_back(seg2);
  }
 else
   {
     dislocation.erase(dislocation.begin()+seg_meeting);
     dislocation.insert(dislocation.begin()+seg_meeting,seg2);
     dislocation.insert(dislocation.begin()+seg_meeting,seg1);
   }
 return 0;
}

double dotprod(double v[],double w[])
{
  double result = v[0]*w[0]+v[1]*w[1];
  return result;
}

double crossprod(double v[],double w[])
{
  double result = v[0]*w[1]-v[1]*w[0];
  return result;
}

double angle(double v[],double w[])
{
  // Return the angle in radian between vector v and w:
  // cout << "v[0]=" << v[0] << " v[1]" << v[1] << endl;
  // cout << "w[0]=" << w[0] << " w[1]" << w[1] << endl;
  
  double vdotw = dotprod(v,w);
  //cout << "vdotw" << vdotw << endl;
  double norm_v = sqrt(pow(v[0],2)+pow(v[1],2));
  //cout << "norm_v" << norm_v << endl;
  double norm_w = sqrt(pow(w[0],2)+pow(w[1],2));
  //cout << "norm_w" << norm_w << endl;

  double cos_theta = vdotw/(norm_v*norm_w);
  //cout << "cos_theta=" << cos_theta << endl;
  double theta = acos(cos_theta);

  if(crossprod(v,w)<0) theta = 2*M_PI - theta;
  // cout << "angle(v,w)=" << theta << endl;
  return theta;
}

int inside_triangle(double X[], double A[], double B[], double C[], bool debug)
{
  if(debug==true) cout << "Enters inside_triangle" << endl;
  if(debug==true) cout << "A" << "=[" << A[0] <<  "," << A[1] << "]" << endl;
  if(debug==true) cout << "B" << "=[" << B[0] <<  "," << B[1] << "]" << endl;
  if(debug==true) cout << "C" << "=[" << C[0] <<  "," << C[1] << "]" << endl;
  if(debug==true) cout << "X" << "=[" << X[0] <<  "," << X[1] << "]" << endl;

  double AB[2];
  AB[0] = B[0]-A[0];
  AB[1] = B[1]-A[1];

  double AC[2];
  AC[0] = C[0]-A[0];
  AC[1] = C[1]-A[1];

  double AX[2];
  AX[0] = X[0]-A[0];
  AX[1] = X[1]-A[1];

  double BC[2];
  BC[0] = C[0]-B[0];
  BC[1] = C[1]-B[1];

  double BA[2];
  BA[0] = A[0]-B[0];
  BA[1] = A[1]-B[1];

  double BX[2];
  BX[0] = X[0]-B[0];
  BX[1] = X[1]-B[1];

  double CA[2];
  CA[0] = A[0]-C[0];
  CA[1] = A[1]-C[1];

  double CB[2];
  CB[0] = B[0]-C[0];
  CB[1] = B[1]-C[1];

  double CX[2];
  CX[0] = X[0]-C[0];
  CX[1] = X[1]-C[1];

  double prod1;
  prod1 = crossprod(AB,AC);
  int sign_prod1 = sign(prod1);
  // if(debug==true) cout << "AB.AC=" << prod1 << endl;
  
  AB[0] = B[0]-A[0];
  AB[1] = B[1]-A[1];
  double prod2;
  prod2 = crossprod(AB,AX);
  int sign_prod2 = sign(prod2);
  if(debug==true) cout << "AB.AC=" << prod1 << endl;
  if(debug==true) cout << "AB.AX=" << prod2 << endl;

  if(debug==true) cout << "sign(AB.AC)=" << sign_prod1 << endl;
  if(debug==true) cout << "sign(AB.AX)=" << sign_prod2<< endl;

  if(sign_prod1 != sign_prod2) return 0;

  double prod3;
  prod3 = crossprod(BC,BA);
  if(debug==true) cout << "BC.BA=" << prod3 << endl;
  double prod4;
  prod4 = crossprod(BC,BX);
  if(debug==true) cout << "BC.BX=" << prod4 << endl;

  if(sign(prod3) != sign(prod4)) return 0;

  double prod5;
  prod5 = crossprod(CA,CB);
  if(debug==true) cout << "CA.CB=" << prod5 << endl;
  double prod6;
  prod6 = crossprod(CA,CX);
  if(debug==true) cout << "CA.CX=" << prod6 << endl;

  if(sign(prod5) != sign(prod6)) return 0;  

  return 1;
}

int sign(double x)
{
  if (x>0) return 1;
  else if (x<0) return -1;
  else return 0;
}

int clean_dislocation(vector<segment> &dislocation, vector<obstacle> &obs)
{
  // Removes loops:
  // Look for identical pinned obstacles:

  int dislo_size = dislocation.size();

  for(int i=0; i<dislo_size-1; i++)
    {
      for(int j=i+1; j<dislo_size;j++)
	{
	  if (dislocation[i].A == dislocation[j].A)
	    {

	      //What is the loop to remove? If inside the loop there is not the same number of segments of type 1 and 2, then it's not the one to delete.

	      //Check the number of segments of type 1 and type 2 in between i and j:
	      int ntype1seg=0;
	      int ntype2seg=0;

	      for(int k=i;k<j;k++)
		{
		  if (dislocation[k].type == 1) ntype1seg++;
		  if (dislocation[k].type == 2) ntype2seg++;
		}
	      // cout << "ntype1seg = " << ntype1seg << endl;
	      // cout << "ntype2seg = " << ntype2seg << endl;

	      if (ntype1seg == ntype2seg)
		{
		  vector<segment> loop;
		  loop.insert(loop.begin(),dislocation.begin()+i,dislocation.begin()+j);
		  dislocation.erase(dislocation.begin()+i,dislocation.begin()+j);
		  kill_obstacles_inside_loop(loop,obs);
		  return 1;
		}
	      else
		{
		  // cout << "i=" << i;
		  // cout << " j=" << j << endl;

		  vector<segment> loop;
		  loop.insert(loop.begin(),dislocation.begin()+j,dislocation.end());
		  loop.insert(loop.end(),dislocation.begin(), dislocation.begin()+i);

		  dislocation.erase(dislocation.begin()+j,dislocation.end());
		  dislocation.erase(dislocation.begin(), dislocation.begin()+i);
		  kill_obstacles_inside_loop(loop,obs);
		  
		  //Check if the number of segments of type 1 and 2 are well different, if not, there is an issue:
		  int ntype1seg=0;
		  int ntype2seg=0;
		  
		  for(int k=0;k<dislocation.size();k++)
		    {
		      if (dislocation[k].type == 1) ntype1seg++;
		      if (dislocation[k].type == 2) ntype2seg++;
		    }

		  
		  // cout << "Dislocation:" << endl;
		  // for(int i=0;i<dislocation.size();i++)
		  //   {
		  //     cout << i << "\t" << dislocation[i].A << " " << dislocation[i].B << " " << dislocation[i].type << endl;
		  //   }

		  // cout << "ntype1seg=" << ntype1seg << endl;
		  // cout << "ntype2seg=" << ntype2seg << endl;
		  
		  if (ntype1seg != ntype2seg) return 1;
		  else return -1;
		}
	    }
	}
    }
  return 0;
}

//Check if the point I is in the loop:
int  inside_loop(vector<segment> loop, vector<obstacle> &obs, int I)
{
  // Create the list of the corners:
  vector<double> polyX;
  vector<double> polyY;
  
  int sum_type=0;
  for(int i=0; i<loop.size(); i++)
    {
      sum_type = mod(sum_type + loop[i].type,3);
      
      double x=obs[loop[i].B].x;
      double y=obs[loop[i].B].y;

      if(sum_type==2) x=x-1;
      else if (sum_type==1) x+=1;
      polyX.push_back(x);
      polyY.push_back(y);
    }

  if(polyX.back()!=obs[loop[0].A].x)
    {
      cout << "Error, the loop is not closed" << endl;
      for(int i=0; i<loop.size(); i++)
	{
	  cout << loop[i].A << " " << loop[i].B << " " << loop[i].type << endl;
	}
    }
  

  int polySides = polyX.size();
  int i, j=polySides-1 ;
  bool oddNodes=false;
  for(int k=-1;k<2;k++)
    {
      double x=obs[I].x+k;
      double y=obs[I].y;
      
      for (i=0; i<polySides; i++) {
	if (polyY[i]<y && polyY[j]>=y
	    ||  polyY[j]<y && polyY[i]>=y) {
	  if (polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x) {
	    oddNodes=!oddNodes; }}
	j=i; }
      
      if (oddNodes==true) 
	{
	  //cout << I << " is inside the loop\n";
	  return 1;
	}
    }
  return 0;
}


int kill_obstacles_inside_loop(vector<segment> loop, vector<obstacle> &obs)
{
  // cout << "Loop:\n";
  // for(int i=0;i<loop.size();i++)
  //   {
  //     cout << loop[i].A << " " << loop[i].B << " " << loop[i].type << endl;
  //   }
  // Marks as dead all the points inside the loop:
  for(int i=0;i<obs.size(); i++)
    {
      if(obs[i].alive==true)
	{
	  //Check if the point is inside the loop:
	  if(inside_loop(loop,obs,i))
	    {
	      // cout << "Obstacle " << i << "is inside the loop and marked as dead\n";
	      obs[i].alive=false;
	    }
	}
    }
  return 0;
}

int mod(int a, int b)
{
  int result = a % b;
  if(result<0)
    {
      result+=b;
    }
  return result;
}
