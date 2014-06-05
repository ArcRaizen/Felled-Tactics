#ifndef TRAVELNODE_S
#define TRAVELNODE_S
#include "Position.h"

struct TravelNode
{
	Position p;
	int f, g, h;
	int pathNum;
	TravelNode* parent;
	TravelNode(){f = 0; g = 0; h = 0; pathNum = 0; parent = 0;}
	TravelNode(TravelNode* par, Position _p, int _g, int _h) 
	{ 
		parent = par;
		p = _p; 
		g = _g + (par != NULL ? par->g : 0); 
		h = _h; 
		f = g + h;
		pathNum = (par != NULL ? par->pathNum + 1 : 0);
	}
};
#endif