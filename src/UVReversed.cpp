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

#include "UVReversed.h"
#include "UVCommon.h"

#include <iostream>
#include <cstdlib>
#include <assert.h>

//#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;
#define er ;

using namespace std;

#define MERR_CHK(stat,msg) if ( !stat ) { MGlobal::displayError(msg); } // cerr << msg << endl; }

UVReversed::UVReversed()
{
}

UVReversed::~UVReversed()
{
}

void* UVReversed::creator()
{
   return new UVReversed;
}

bool UVReversed::isUndoable() const
{
   return false;
}

MStatus UVReversed::doIt(const MArgList& args)
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
	MItMeshPolygon itmeshp(obj, &stat);er
	MItMeshPolygon itmeshp2(path.node(), &stat);er
	
	MSelectionList selection;
	
	bool inversionFound = false;
	
	MIntArray reversedUVs;
	
	for (itmeshp.reset(); itmeshp.isDone() == false; itmeshp.next())
	{
		MVector normal;
		stat = itmeshp.getNormal(normal, MSpace::kWorld);
		cout << "normal " << normal.y << endl;
		if (normal.y > 0)
		{
			inversionFound = true;
			int prev = 0;
			itmeshp2.setIndex(itmeshp.index(), prev);
			if (itmeshp2.hasUVs())
			{
				// get number of verts
				MIntArray vertices;
				stat = itmeshp2.getVertices(vertices);er
				// for each vert
				for (size_t i = 0; i < vertices.length(); i++)
				{
					// get UV
					int index;
					stat = itmeshp2.getUVIndex(i,index);
					// add UV to list
					reversedUVs.append(index);
				}
			}
		}
	}

	setResult(inversionFound);
	
	if (inversionFound)
	{
		MString str = "select -add ";
		for (size_t i = 0; i < reversedUVs.length(); i++)
		{
			str += meshName + ".map[" + reversedUVs[i] + "] ";
		}
		cout << str << endl;
		MGlobal::executeCommand(str);
	}
	
	stat = MGlobal::deleteNode( obj );er

	return stat;
}

// UNDO THE COMMAND
MStatus UVReversed::undoIt()
{
	return MStatus();
}
