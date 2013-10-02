#ifndef _HOLES_BORDER_H_
#define _HOLES_BORDER_H_

#include <maya/MArgList.h>
#include <maya/MStatus.h>
#include <maya/MPxCommand.h>
#include <maya/MFloatPoint.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>

#include <list>
#include <vector>

typedef unsigned int uint;

//! Find holes in a mesh and create a split around said hole
class holesBorder : public MPxCommand
{

	struct RingEdgeInfo
	{
		uint	m_edge;
		uint	m_boundaryVertex;
		uint	m_internalVertex;
	};

	public:
								//! Default public constructor
								holesBorder();
								
								//! Default public destructor
		virtual 				~holesBorder();
		
		//! Node creator
		static void* 			creator();
		
		//! Specifies whether the action is undable
		bool 					isUndoable() const;
		
		//! Performs the node's duty
		MStatus 				doIt(const MArgList&);
		
		//! Undoes what was done in doIt
		MStatus 				undoIt();
		
		
		/*! recursive function which does the main part of the algorithm's work
		*/
		MStatus					func(	uint edge, 
										uint vertex,
										const MFnMesh&,
										MItMeshEdge&,
										MItMeshVertex& itv,
										uint hole,
										std::vector<std::vector<uint> >& borders,
										std::vector<std::vector<RingEdgeInfo> >& rings,
										std::list<uint>& allBorderEdges,
										std::list<uint>::iterator& itel);
		

};

#endif
