#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MItSelectionList.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MObject.h>
#include <maya/MSelectionList.h>
#include <maya/MArgList.h>
#include <maya/MFloatPoint.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnSpotLight.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MItMeshVertex.h>
#include <maya/MBoundingBox.h>
#include <maya/MItMeshEdge.h>

#include "holesBorder.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <assert.h>

#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;

using namespace std;

/*                              ______________
                         ,===:'.,            `-._
                                `:.`---.__         `-._
                                  `:.     `--.         `.
                                    \.        `.         `.
                            (,,(,    \.         `.   ____,-`.,
                         (,'     `/   \.   ,--.___`.'
                     ,  ,'  ,--.  `,   \.;'         `
                      `{D, {    \  :    \;
                        V,,'    /  /    //
                        j;;    /  ,' ,-//.    ,---.      ,
                        \;'   /  ,' /  _  \  /  _  \   ,'/
                              \   `'  / \  `'  / \  `.' /
                               `.___,'   `.__,'   `.__,'


Right so this check's aim is to fix holes within polygonal geometry by adding an extra edge
loop within a specified threshold of said hole. As a result, the hole keeps its shape when the
geometry is smoothed or converted to subDs.

Now maya can already do a lot of the preliminary work by selecting the edges which are on the
boundary, with:

    polySelectConstraint -m 3 -t 0x8000 -w 1;
    polySelectConstraint -dis;

It's at this point that this plugin command gets invoked. So our input is just the assumption
that the current selection holds boundary edges. These could represent one or more holes, on
more than one object.

The first step is therefore to run an algorithm to sort this set of edges into individual sets,
with each set representing one hole, basically turning S into discreet sets of ordered edges.

We do this by running a recursive algorithm which picks the first edge within our input set S
and *walks* along that edge to the next contiguous edge which is on a boundary. Each time such
an edge is encoutered it is removed from S and added to its own respective ordered set. The
algorithm continues recursing until it hits an edge which is within our new ordered set at
which point we have walked around the entire hole.

At this point it returns to the first edge in S and starts recursing once again. The algorithm
continues doing this until S is empty.

std::find being linear my estimate is that this algorithm also runs in linear time according
to the number of edges in S.

*/

holesBorder::holesBorder()
{
}

holesBorder::~holesBorder()
{
}

void* holesBorder::creator()
{
   return new holesBorder;
}

bool holesBorder::isUndoable() const
{
   return false;
}

// here we assume that we have a bunch of border edges selected
// in one mesh only
MStatus holesBorder::doIt(const MArgList& args)
{
	// Even though we have edges selected, getActiveSelectionList only returns
	// objects, not components
	MSelectionList list;
	MStatus stat = MGlobal::getActiveSelectionList(list);er
	
	MString returnStr;

	// for now we'll only work with one object at a time.. else
	// it will get too messy

	for (size_t i = 0; i < list.length(); i++)
	{
		// This is the threshold
		// if we are below this threshold then do not insert loops
		// although this threshold is in world space which sucks
		float threshold = 0.1;

		MDagPath path;
		MObject	component;
		stat = list.getDagPath(i,path,component);er
		MFnMesh mesh(path, &stat);er
		MString name = path.fullPathName(&stat);er
		MItMeshEdge ite(path, component, &stat);er
		MItMeshVertex itv(path, MObject::kNullObj, &stat);er
		int prevIndex = 0;
		
		std::list<uint> allBorderEdges;
		vector<vector<uint> > borders;
		vector<vector<RingEdgeInfo> > rings;
		
		while (!ite.isDone())
		{
			allBorderEdges.push_back(ite.index(&stat));er
			ite.next();
		}
		
		std::list<uint>::iterator itel = allBorderEdges.begin();

		uint holeNumber = 0;

		int counter = 0;
	
		// sort all the edges into descreet buckets of contiguous edgess
		while (itel != allBorderEdges.end() && counter++ < 500)
		{
			// so we have our first edge
			uint edge = *itel;
		
			// let's pick a vertex fairly randomly
			int vertexList[2];
			stat = mesh.getEdgeVertices(edge, vertexList);er

			// set it as the start index of the vertex iterator
			stat = itv.setIndex(vertexList[1], prevIndex);er

			rings.push_back(vector<RingEdgeInfo>());
			borders.push_back(vector<uint>());
			func(edge, vertexList[1], mesh, ite, itv, holeNumber++, borders, rings, allBorderEdges, itel);
		}

		bool splitDone = false;

		for (size_t i = 0; i < borders.size() && splitDone == false; i++)
		{
			vector<double> edgeLength(rings[i].size());
			for (size_t j = 0; j < rings[i].size(); j++)
			{
				ite.setIndex(rings[i][j].m_edge, prevIndex);
				stat = ite.getLength(edgeLength[j], MSpace::kWorld);er
			}
			
			double maxDist = *max_element(edgeLength.begin(), edgeLength.end());
						
			if (fabs(maxDist - threshold) > 0.05)
			{
				stringstream ss;
				ss << "polySplit ";

				for (size_t j = 0; j < rings[i].size(); j++)
				{
					int vertexList[2];
					stat = mesh.getEdgeVertices(rings[i][j].m_edge, vertexList);er
				
					float f = 0.1/edgeLength[j];
					if (f > 1 || f < 0) f = 0.5;
					uint boundaryVertex = rings[i][j].m_boundaryVertex;
					uint internalVertex = rings[i][j].m_internalVertex;
					
					float inv = ((boundaryVertex == vertexList[0]) ? f : 1-f);
					
					assert(inv >= 0 && inv <= 1);
					
					ss << " -ep " << rings[i][j].m_edge << " " << inv;
				}

				int vertexList[2];
				stat = mesh.getEdgeVertices(rings[i][0].m_edge, vertexList);er
				float f = 0.1/edgeLength[0];
				if (f > 1 || f < 0) f = 0.5;
				float inv = rings[i][0].m_boundaryVertex == vertexList[0] ? f : 1-f;
				assert(inv >= 0 && inv <= 1);
				ss << " -ep " << rings[i][0].m_edge << " " << inv << " " << path.fullPathName().asChar() << "; ";
			
				returnStr += MString(ss.str().c_str());
								
				splitDone = true;
			}
		}
	}
	MPxCommand::setResult(returnStr);
	return MS::kSuccess;
}

// Recursive function for sorting polygonal edges into
// discreet sets of connected edges
MStatus holesBorder::func(	uint edge,
							uint vertex,
							const MFnMesh& mesh,
							MItMeshEdge& ite,
							MItMeshVertex& itv,
							uint holeNumber,
							vector<vector<uint> >& borders,
							vector<vector<RingEdgeInfo> >& rings,
							std::list<uint>& allBorderEdges,
							std::list<uint>::iterator& itel
							)
{
	MStatus stat;
	int prevIndex = 0;
	stat = itv.setIndex(vertex, prevIndex);

	// get edges connected to current vertex
	MIntArray edgesConnectedToVertex;
	stat = itv.getConnectedEdges(edgesConnectedToVertex);
	
	// for each connected edge
	for (size_t j = 0; j < edgesConnectedToVertex.length(); j++)
	{
		// that is not the current edge
		if ((uint)edgesConnectedToVertex[j] != edge)
		{
			// get vertices of this connected edge
			int vertexList[2];
			stat = mesh.getEdgeVertices(edgesConnectedToVertex[j], vertexList);er
			stat = ite.setIndex(edgesConnectedToVertex[j], prevIndex);er

			// if that particular edge is on the boundary
			if (ite.onBoundary(&stat))
			{
				er
				// add that edge to its appropriate bucket
				borders[holeNumber].push_back(edge);

				// remove from the pool
				allBorderEdges.remove(edge);

				itel = allBorderEdges.begin();
				
				// look for this edge in the bucket.. 
				vector<uint>::iterator fit = find(borders[holeNumber].begin(), borders[holeNumber].end(), edgesConnectedToVertex[j]);
				
				// if it's in there in means we've gone all
				// around the loop so we can stop
				if (fit == borders[holeNumber].end())
					// else recurse
					func((uint)edgesConnectedToVertex[j], vertex == vertexList[0] ? (uint)vertexList[1] : (uint)vertexList[0], mesh, ite, itv, holeNumber, borders, rings, allBorderEdges, itel);
			}
			else
			{
				// if the edge is not on a boundary it means it's one of the edges 
				// that will need splitting.. so store its information
				RingEdgeInfo info = {
										// Edge
										(uint)edgesConnectedToVertex[j], 
										// Boundary Vertex
										(uint)vertex, 
										// Internal Vertex
										(uint)(vertex == vertexList[0] ? vertexList[1] : vertexList[0])
									};
				rings[holeNumber].push_back(info);
			}
		}
	}
	return MS::kSuccess;
}

MStatus holesBorder::undoIt()
{
	return MStatus();
}
