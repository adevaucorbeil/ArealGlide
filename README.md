# ArealGlide

ArealGlide is a tool that simulates the glide of a single dislocation through a point-like obstacles distributed on the glide plane.

As input it takes the location of the obstacles, as well as their respective breaking angles.
It outputs the position of the dislocation at each step, and the final normalized critical resolved shear stress. 

############################ COMPILATION ###############################

In order to compile the Areal Glide Model, one needs GCC. If you are
using Windows, you will need to use Cygwin, and compile the code from
a Cygwin terminal.

To compile, simply run: g++ main.cpp -o areal

This should create an executable file called areal.


################################ USE ###################################

The Areal Glide model uses a single input file. This file is a simple
text file containing three columns of numbers (divided by a single space)

Each line of the input file corresponds to a single obstacle. Thus, the
number of lines corresponds to the number of obstacles in the box.

The first column contains the normalized x position of the obstacle
(between 0 and 1). The second column contains the normalized y position
of the obstacle (between 0 and 1). The third column contains the
breaking angle of the obstacle expressed in radians.

If the input file is called input.xy0, to run the model, simply run:
./areal -i test.xy0 -o test.out

As the simulation is running, no output will appear on the terminal,
however, it will create a file (called test.out here or whatever you
name it) which contains the list of obstacles pinning the dislocation
at each steps, and the normalized shear stress associated with the
passage to this very step. This file could be used to visualize the
glide of the dislocation step by step during post-processing.

At the end of the simulation, the model will output a single line
in the standard output (terminal) similar to the following example:
The simulation ended correctly: CRSS=0.000764182

It indicates that the simulation was successful, and also the value
of the normalized critical shear stress obtained (here 0.000764182)

One could also ask this output to be stored in a file as follows:
./areal -i test.xy0 -o test.out >> test.log

Thus, once the simulation is complete, no output will appear on the
standard output (terminal), but the file test.log will be created
and will contain the line:
The simulation ended correctly: CRSS=0.000764182



####################### GENERATOR OF INPUT FILES #######################

The folder Obstacle_Generators/Random contains two generators of input
files for the areal glide model.

The first one, contained in
Obstacle_Generators/Random/Single_or_multiple_sets can create
distributions of 1 or multiple sets of obstacles, each set having the
same breaking angle.
In order to compile this generator, type:
g++ main.cpp -o random

It will create an executable called random. To use it, do:
./random -n number_of_obstacles -o input.xy0

and follow the instructions.
In return, it will create an input file called input.xy0 containing
number_of_obstacles obstacles.


The second one, contained in
Obstacle_Generators/Random/Strength_distribution can create a single
set of obstacles whose strength follows a log-normal distribution.

To compile this generator, you will need a version of GCC which supports
the TR1 standard. typically GCC-3.2.3 and later versions. Compile it
as follows:
g++ main.cpp -o random_with_strength_distribution

And run it as follows:
./random_with_strength_distribution -n number_of_obstacles -o input.xy0

and follow the instructions.
In return, it will create an input file called input.xy0 containing
number_of_obstacles obstacles.

########################### POST-PROCESSING ############################

If one wants to see step by step the glide of the dislocation through
the array of obstacles, one could use the MATLAB code located in the
Visualization folder.

The main function is contained in extract_stable_configuration.m

It is used the following way:
stepleap=1000 # Number of steps to leap during randering (1 to visualize all steps)
doplot=1 # if =1, the evolution will be plotted in MATLAB. If =0 no visual output occurs in MATLAB
dosave=1 # if =1, the visualization will be saved in jpeg files. =0 to not save
extract_stable_configuration('../test',stepleap,doplot,dosave)

It is important that the input file and the output file differ only by
their extension, i.e: test.xy0 and test.out, or thing.xy0 and thing.out,
for example.
