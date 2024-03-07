#ifndef FILE_PROCESS_H
#define FILE_PROCESS_H

#include <fstream>
#include <vector>
#include "data_structure.h"

using namespace std;
using namespace data_structure;

//------- read input file and write output file -------
class File_Process 
{
private:
	// transfer sentence to substrings
	vector <string> sentence_to_substrings(string input) // Ex: save "Hello World" as "Hello" and "World"
	{
		vector <string> output;
		int start_pos = 0;
	
		for(int i=0; i<input.length(); i++) 
		{
			if(input[i] == ' ')
			{
				if(input[start_pos] != ' ' && start_pos != i) output.push_back( input.substr(start_pos, i-start_pos) );
				start_pos = i+1;
			}
			else if(i == input.length()-1) // final character -> save word and break the loop
			{
				output.push_back( input.substr(start_pos, i+1-start_pos) );
				break;
			}
		}
	
		return output;
	}

	// transfer string to integer number
	int StoI(string num)
	{
		int out = 0;
		
		for(unsigned int i=0; i<num.length(); i++)
			out = out*10 + (int)num[i] - 48;
		
		return out;
	}

	vector < vector <string> > input_segmentation;
	
public:
	// read input file
	void in_file(vector < vector <string> > &lvs, char *argv)
	{
		ifstream infile(argv, ios::in);
		string in;
		vector <string> substring_data;
	
		if(!infile) // if file not exist -> exit
		{
			cerr<<"File could not be opened"<<endl;
			exit(1);
		}
		else
		{
			while(!infile.eof()) // read this file until reading every line in this file
			{
				getline(infile,in);
				substring_data = sentence_to_substrings(in); // seprate tis line into words
				lvs.push_back(substring_data);
			}
		}
		
		input_segmentation = lvs;
	}
	
	// write output file
	void out_file(vector <VCG> vcg, int track_max, char *argv)
	{
		ofstream outfile (argv, ios::out);
		
		vector < vector <Pin_Location> > pins( vcg.size() ); // initialize its size = vcg's size
		track_max++;
		
		// locate all pins' position
		for(unsigned int i=0; i<2; i++)
		{
			for(unsigned int j=0; j<input_segmentation[i].size(); j++)
			{
				if(input_segmentation[i][j] == "0") continue;
				
				Pin_Location location;
				
				location.col = j;
				location.top = i == 0; // if i = 0 -> top pins, otherwise -> bottom pins
				
				pins[ StoI(input_segmentation[i][j]) ].push_back(location);
			}
		}
		
		for(unsigned int i=1; i<vcg.size(); i++)
		{
			if(vcg[i].eptrs.size() == 0) continue;
			
			outfile<<".begin "<<i<<endl; 	
			
			//-----------------  print edges of this net --------------------
			for(unsigned int j=0; j<vcg[i].eptrs.size(); j++)
			{
				Edgeptr cur_edge = vcg[i].eptrs[j];
				
				// print horizontal edges
				outfile<<".H "<<cur_edge->start<<" "<<track_max - cur_edge->track<<" "<<cur_edge->end<<endl;
				
				// print vertical edges
				for(unsigned int k=0; k<pins[i].size(); k++)
				{
					if(pins[i][k].col > cur_edge->start && pins[i][k].col < cur_edge->end)
					{
						if(pins[i][k].top)
							outfile<<".V "<<pins[i][k].col<<" "<<track_max - cur_edge->track<<" "<<track_max<<endl;
						else
							outfile<<".V "<<pins[i][k].col<<" 0 "<<track_max - cur_edge->track<<endl;
						
						pins[i].erase(pins[i].begin() + k--);
					}
					else if(pins[i][k].col == cur_edge->start)
					{
						if(j != 0) // not the first edge
						{
							if(pins[i][k].top)
								outfile<<".V "<<pins[i][k].col<<" "<<track_max - cur_edge->track<<" "<<track_max<<endl;
						}
						else // must connect together
						{
							if(pins[i][k].top)
								outfile<<".V "<<pins[i][k].col<<" "<<track_max - cur_edge->track<<" "<<track_max<<endl;
							else
								outfile<<".V "<<pins[i][k].col<<" 0 "<<track_max - cur_edge->track<<endl;
						}
					}
					else if(pins[i][k].col == cur_edge->end)
					{
						if(j != vcg[i].eptrs.size() - 1) // not the final edge
						{
							if(!pins[i][k].top)
								outfile<<".V "<<pins[i][k].col<<" 0 "<<track_max - cur_edge->track<<endl;
						}
						else // must connect together
						{
							if(pins[i][k].top)
								outfile<<".V "<<pins[i][k].col<<" "<<track_max - cur_edge->track<<" "<<track_max<<endl;
							else
								outfile<<".V "<<pins[i][k].col<<" 0 "<<track_max - cur_edge->track<<endl;
						}
					}
					
				}
				
				// print the vertical edges between adjacent horizontal edges
				if(j > 0)
				{
					Edgeptr pre_edge = vcg[i].eptrs[j-1];
					
					int big_track = track_max - pre_edge->track;
					int sma_track = track_max - cur_edge->track;
					
					if(big_track < sma_track) swap(sma_track, big_track);
					else if(big_track == sma_track) continue;
					
					outfile<<".V "<<cur_edge->start<<" "<<sma_track<<" "<<big_track<<endl;
				}
			}
			
			outfile<<".end"<<endl; 
		}
		
		return;
	}
	
};

#endif




