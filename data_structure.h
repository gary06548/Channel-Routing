#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <iostream>
#include <vector>

using namespace std;

//------- define data structure of graph node -------
namespace data_structure
{
	// horizontal edges in left edge algorithm
	struct Edge_Link 
	{
		int start, end;         // start <--------> end
		int id;                 // pin id of this net
		int track;              // the track to which the edge belongs
		bool type;              // 0: space edge, 1: solid edge
		struct Edge_Link *next; // next edge
	};
	
	typedef Edge_Link* Edgeptr; // name Edge_Link* as Edgeptr
	
	// data structure of VCG
	struct Node 
	{
		int tail;          // tail id
		int head;          // head id
		bool state;        // shows that this node is teaversed or not
		struct Node *psor; // presuccessor
		struct Node *ssor; // successor
	};
	
	typedef Node* Nodeptr; // name Node* as Nodeptr
	
	struct VCG
	{
		Nodeptr sptr; // successor
		Nodeptr pptr; // presuccessor
		vector <Edgeptr> eptrs; // horizontal edges
	};
	
	struct Pin_Location
	{
		bool top; // 1: top pin, 0: bottom pin
		int col;  // the position (column) of this pin
	};
	
};

#endif




