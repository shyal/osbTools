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
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MItMeshVertex.h>
#include <maya/MBoundingBox.h>
#include <maya/MItMeshEdge.h>

#include "UVSeams.h"
#include "UVCommon.h"

#include <iostream>
#include <cstdlib>
#include <assert.h>

//#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;
#define er ;

using namespace std;

#define MERR_CHK(stat,msg) if ( !stat ) { MGlobal::displayError(msg); } // cerr << msg << endl; }

UVSeams::UVSeams()
{
}

UVSeams::~UVSeams()
{
}

void* UVSeams::creator()
{
   return new UVSeams;
}

bool UVSeams::isUndoable() const
{
   return false;
}

MStatus UVSeams::doIt(const MArgList& args)
{
	MStatus stat;

	MString meshName = args.asString(0, &stat);er
	MSelectionList list;
	list.add(meshName);
	MDagPath path;
	stat = list.getDagPath(0, path);er
	MFnMesh fnMesh(path, &stat);er

	MObject obj = UVCommon::getUVMesh(fnMesh);
	
	// go over every vertex
	MItMeshVertex itmeshv(obj, &stat);er
	MItMeshVertex itmeshv2(obj, &stat);er
	MItMeshEdge itmeshe(obj, &stat);er
	
	MSelectionList selection;
	
	bool seamFound = false;
	
	MIntArray seamVerts;
	
	for (itmeshv.reset(); itmeshv.isDone() == false; itmeshv.next())
	{
		MIntArray edgeList;
		// for each vertex get ever edge
		stat = itmeshv.getConnectedEdges(edgeList);
		
		int vertexIndex = itmeshv.index();
				
		// if neighbouring edge has dot prod of circa 1 that's most likely a seam
		for (size_t i = 0; i < edgeList.length(); i++)
		{
			int e1 = edgeList[i];
			int e2 = edgeList[i < edgeList.length()-1 ? i + 1 : 0];
			
			int oppVertex1;
			int oppVertex2;

			int prevIndex = 0;

			stat = itmeshv.getOppositeVertex(oppVertex1, e1);er

			MPoint e1v0 = itmeshv.position(MSpace::kObject,&stat);er
			
			stat = itmeshv2.setIndex(oppVertex1, prevIndex);er
			
			MPoint e1v1 = itmeshv2.position(MSpace::kObject,&stat);er

			MVector e1e(e1v1 - e1v0);
			
			stat = itmeshv.getOppositeVertex(oppVertex2, e2);er
		
			stat = itmeshv2.setIndex(oppVertex2, prevIndex);er
			
			MPoint e2v1 = itmeshv2.position(MSpace::kObject,&stat);er
			
			MVector e2e(e2v1 - e1v0);

			if ((e1e.normal() * e2e.normal() > 0.999) && fabs(e1e.length() - e2e.length()) < 0.001)
			{
				seamVerts.append(vertexIndex);
				seamVerts.append(oppVertex1);
				seamVerts.append(oppVertex2);
				seamFound = true;
			}
			if (edgeList.length() == 2)
				break;
		}
	}

	setResult(seamFound);
	
	if (seamFound)
	{
		MString str = "select -add ";
		for (size_t i = 0; i < seamVerts.length(); i++)
		{
			str += meshName + ".map[" + seamVerts[i] + "] ";
		}
		MGlobal::executeCommand(str);
	}
	
	
	stat = MGlobal::deleteNode( obj );er

	return stat;
}

// UNDO THE COMMAND
MStatus UVSeams::undoIt()
{
	return MStatus();
}
