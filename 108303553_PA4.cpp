#include <iostream>
#include <vector>
#include <algorithm>
#include "data_structure.h"
#include "file_process.h"
#include "graph.h"

using namespace std;
using namespace data_structure;

int main(int argc, char *argv[])
{
	vector < vector <string> > input; // string segmentation from input file
	vector <VCG> vcg;                 // vertical constrain graph
	int track_max = 0;                // the number of tracks
	
	//char input_file[] = "case10.txt";
	//char output_file[] = "out10.txt";
	
	//-------  read iput file and store the data into a 2D string vector  --------
	File_Process file;
	file.in_file(input, argv[1]); // read input file
	
	//--------------------  use left edge algorithm to route  --------------------
	Graph graph(input);
	graph.left_edge(vcg, track_max);

	//---------------------------  write output file  ----------------------------
	file.out_file(vcg, track_max, argv[2]);
}













