#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <algorithm>
#include "data_structure.h"

using namespace std;
using namespace data_structure;

//------------------  create graph  ------------------
class Graph 
{
private:
	vector < vector <string> > input; // input data from input file
	int col_size;                     // column size of pins from input file
	
	// transfer string to integer number
	int StoI(string str)
	{
		int num = 0;
			
		for(unsigned int i=0; i<str.length(); i++)
			num = num*10 + (int)str[i] - 48;
		
		return num;
	}
	
	// create a new node in VCG
	Nodeptr NewNode(int Tail, int Head)
	{
		Nodeptr newnode = new Node;
	
		newnode->tail  = Tail;
		newnode->head  = Head;
		newnode->state = false; // haven't been traversed
		newnode->psor  = NULL;
		newnode->ssor  = NULL;
		
		return newnode;	
	}
	
	// create a new edge 
	Edgeptr NewEdge(int start, int end, int id, bool type)
	{
		Edgeptr output = new Edge_Link;
		
		output->start = start; // start point
		output->end   = end;   // end point
		output->id    = id;    // -1: space edge, others: solid edge
		output->track = -1;    // haven't been assigned to track -> initial value is invalid
		output->type  = type;  // 0: space edge, 1: solid edge
		output->next  = NULL;  // no next edge yet
		
		return output;
	}
	
	// use for finding all horizontal edges in these nets
	Edgeptr find_edge(int id)
	{
		int start_pt = -1; // invalid value
		
		// find the foremost position of this id
		for(int col = 0; col < input[0].size(); col++)
		{
			if( StoI(input[0][col]) == id || StoI(input[1][col]) == id )
			{
				start_pt = col;
				break;
			}
		}
		
		// find the final position of this id
		for(int col = input[0].size()-1; col>= 0; col--)
		{
			if( StoI(input[0][col]) == id || StoI(input[1][col]) == id)
			{
				return NewEdge(start_pt, col, id, true);
			}
		}
	}

	// compare left x coordinate, return true if edge_1's x is smaller then edge_2, otherwise return false 
	static bool compare_x(Edgeptr e1, Edgeptr e2)
	{
		return e1->start < e2->start;
	}
	
	// find all unconstrained edges (doesn't include free edges)
	vector <Edgeptr> unconstrain_edges(vector <VCG> vcg)
	{
		vector <Edgeptr> output;
		
		for(unsigned int i=1; i<vcg.size(); i++)
		{
			// this pin exists and has no presuccessor and has successor
			if(vcg[i].pptr->state)  // && vcg[i].sptr->ssor != NULL
			{
				Nodeptr ptmp = vcg[i].pptr->psor;
				bool store_flag = true;
				
				while(ptmp != NULL)
				{
					if( !(ptmp->state) )
					{
						store_flag = false;
						break;
					}
					ptmp = ptmp->psor;
				}
				
				if(store_flag) output.push_back(vcg[i].eptrs[0]);
			}
		}
		
		// sort free_edges and constrain_edges, small to big
		sort(output.begin(), output.end(), compare_x); 
		
		return output;
	}

	// create a VCG
	void create_VCG(vector <VCG> &vcg)
	{
		vector <Nodeptr> successor, presuccessor;
		
		//------------ create nodes into successor vector ---------------	
		// find maximum id in these pins
		
		int max_id = 0; // maximum id of pins
		
		for(unsigned int row=0; row<2; row++)
		{
			for(unsigned int col=0; col<input[0].size(); col++)
			{
				int value = StoI(input[row][col]);
				if(value > max_id) max_id = value;
			}
		}
		
		for(int i=0; i<=max_id; i++)
		{
			successor.push_back( NewNode(i, -1) );    // -1 -> NULL
			presuccessor.push_back( NewNode(-1, i) ); // -1 -> NULL
		}
		
		// construct nodes of successor
		for(unsigned int col=0; col<input[0].size(); col++)
		{
			presuccessor[ StoI(input[0][col]) ]->state = true; // means that this pin exist
			presuccessor[ StoI(input[1][col]) ]->state = true; // means that this pin exist
			
			// if the number = 0 -> skip creating node
			if(input[0][col] == "0" || input[1][col] == "0" || input[0][col] == input[1][col]) continue;
			
			Nodeptr temp_ssor = successor[ StoI(input[0][col]) ];
			
			while(temp_ssor->ssor != NULL) temp_ssor = temp_ssor->ssor;
			
			temp_ssor->ssor = NewNode( StoI(input[0][col]) , StoI(input[1][col]) );
		}
		
		//-------- construct nodes' connection of presuccessor vector -----------
		for(int i=0; i<presuccessor.size(); i++)  
		{
			Nodeptr temp_psor = presuccessor[i];
			
			for(int j=0; j<successor.size(); j++)
			{
				Nodeptr temp_ssor = successor[j]->ssor;
				
				while(temp_ssor != NULL)
				{
					if(temp_ssor->head == presuccessor[i]->head)
					{
						temp_psor->psor = temp_ssor;
						temp_psor = temp_ssor;
						//break; // node maybe connect to same successor 2 times or more
					}
					
					temp_ssor = temp_ssor->ssor;
				}
			}
		}
		
		// store successor and presuccessor into vcg
		for(unsigned int i=0; i<successor.size(); i++)
		{
			VCG tmp_vcg;
			tmp_vcg.sptr = successor[i];
			tmp_vcg.pptr = presuccessor[i];
			
			if(presuccessor[i]->state) tmp_vcg.eptrs.push_back( find_edge( presuccessor[i]->head ) );
			
			vcg.push_back(tmp_vcg);
		}
	}
	
public:
	// constructor -> initialize private members	
	Graph(vector < vector <string> > input_data)
	{
		input = input_data;
		col_size = input[0].size();
	}

	// the algorithm of routing
	void left_edge(vector <VCG> &vcg, int &track)
	{
		vector <Edgeptr> constrain_edges; // all unassigned edges (doesn't include free_edges)
		vector <Edgeptr> tracks;          // the vector used to access all edges of each track (row)
		
		// create vcg by input file
		create_VCG(vcg);
		
		constrain_edges = unconstrain_edges(vcg);
		
		// assign constrain_edges into track
		while(!constrain_edges.empty())
		{
			track++;
			Edgeptr previous_edge = NULL;
			int previous_end = -1;
			
			for(int i=0; i<constrain_edges.size(); i++)
			{
				int start_pt = constrain_edges[i]->start;
				
				if(start_pt < previous_end) continue;
				
				Edgeptr space_edge = NULL;
				
				constrain_edges[i]->track = track;
				
				if(start_pt != previous_end + 1) // not adjacent edges -> create space edge
				{
					space_edge = NewEdge(previous_end + 1, start_pt - 1, -1, false);
					space_edge->track = track;
					space_edge->next = constrain_edges[i];
				}
				
				// if previous_edge == NULL, means that first enter -> update the data of tracks
				if(previous_edge == NULL) 
				{
					if(space_edge != NULL) tracks.push_back(space_edge);
					else tracks.push_back(constrain_edges[i]);
				}
				else 
				{
					if(space_edge != NULL) previous_edge->next = space_edge;
					else previous_edge->next = constrain_edges[i];
				}
				
				
				previous_end = constrain_edges[i]->end;
				previous_edge = constrain_edges[i];
				
				//--------------- update VCG !!! -----------------
				Nodeptr stmp = vcg[ constrain_edges[i]->id ].sptr->ssor;
				vcg[ constrain_edges[i]->id ].pptr->state = false; // has been assigned
				
				while(stmp != NULL)
				{
					stmp->state = true;
					stmp = stmp->ssor;
				}
						
				constrain_edges.erase(constrain_edges.begin() + i--); // "+i" first then "-1"
			}
			
			// if it is final round, create additional space edge to fill the track
			if(previous_end != col_size - 1) 
			{
				Edgeptr fill_space = NewEdge(previous_end + 1, col_size - 1, -1, false);
				fill_space->track = track;
				previous_edge->next = fill_space;
			}
			
			constrain_edges = unconstrain_edges(vcg); // find all unconstrained edges
		} 
	}

};

#endif




