// "When I was 5 years old my mother always told me that happiness was the key to life. 
// When I went to school, they asked me what I wanted to be when i grew up. I wrote down "Happy". 
// They told me i didnt understand the assignment, and I told them they didnt understand life." - John Lennon

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
#include <maya/MPointArray.h>

#include "UVCommon.h"

#include "overlappingUVShells.h"

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <vector>
#include <set>

#include "string.h"

#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;
#define err if (stat != MS::kSuccess) {cout << "trolol " << __FILE__ << ":" << __LINE__ << endl; return MS::kFailure;}
//#define er ;

using namespace std;

#define MERR_CHK(stat,msg) if ( !stat ) { MGlobal::displayError(msg); } // cerr << msg << endl; }

OverlappingUVShells::OverlappingUVShells()
{
}

OverlappingUVShells::~OverlappingUVShells()
{
}

void* OverlappingUVShells::creator()
{
   return new OverlappingUVShells;
}

bool OverlappingUVShells::isUndoable() const
{
   return false;
}

int tri_tri_overlap_test_3d(double p1[3], double q1[3], double r1[3], 
			    double p2[3], double q2[3], double r2[3]);


int coplanar_tri_tri3d(double  p1[3], double  q1[3], double  r1[3],
		       double  p2[3], double  q2[3], double  r2[3],
		       double  N1[3], double  N2[3]);


int tri_tri_overlap_test_2d(double p1[2], double q1[2], double r1[2], 
			    double p2[2], double q2[2], double r2[2]);


int tri_tri_intersection_test_3d(double p1[3], double q1[3], double r1[3], 
				 double p2[3], double q2[3], double r2[3],
				 int * coplanar, 
				 double source[3],double target[3]);


struct ltstr
{
  bool operator()(const MString& s1, const MString& s2) const
  {
    return strcmp(s1.asChar(), s2.asChar()) < 0;
  }
};

MStatus OverlappingUVShells::doIt(const MArgList& args)
{
	MStatus stat;

	MString meshName = args.asString(0, &stat);err
	MSelectionList list;
	list.add(meshName);
	MDagPath path;
	stat = list.getDagPath(0, path);err
	MFnMesh fnMesh(path, &stat);err

	MObjectArray objs = UVCommon::getUVShells(fnMesh);

	cout << "number of objects " << objs.length() << endl;

	vector<MBoundingBox> boxes(objs.length());
	
	set<MString, ltstr> overlappingUVs;

	MItMeshPolygon originalMeshItp(path, MObject::kNullObj, &stat);err

	for (size_t i = 0; i < objs.length(); i++)
	{
		MItMeshVertex itmeshv(objs[i], &stat);
		while(itmeshv.isDone(&stat) == false)
		{
			err;
			boxes[i].expand(itmeshv.position(MSpace::kObject, &stat));err
			itmeshv.next();
		}
	}
	
	MString cmd = "select -add ";
	MString tmp = "";
	bool polyOverlapped = false;

	for (size_t i = 0; i < boxes.size(); i++)
	{
		for (size_t j = i; j < boxes.size(); j++)
		{
			if (i != j)
			{
//				cout << "testing " << i << " and " << j << endl;
				if (boxes[i].intersects(boxes[j], 0.001))
				{
					MItMeshPolygon itmeshpi(objs[i], &stat);err
					MItMeshPolygon itmeshpj(objs[j], &stat);err
					MFnDagNode dagi(objs[i], &stat);err
					MString dagistr = dagi.fullPathName(&stat);err
					MFnDagNode dagj(objs[j], &stat);err
					MString dagjstr = dagj.fullPathName(&stat);err
					while(itmeshpi.isDone(&stat) == false)
					{
						bool ti = itmeshpi.hasValidTriangulation(&stat);err
						if (ti)
						{
							polyOverlapped = false;
							MIntArray vertices;
							stat = itmeshpi.getVertices(vertices);
							MIntArray UVsi(vertices.length());
							for (size_t k = 0; k < vertices.length(); k++)
							{
								itmeshpi.getUVIndex(k, UVsi[k]);
								tmp += dagistr + ".map[" + UVsi[k] + "] ";
							}
							
							int triCounti;
							stat = itmeshpi.numTriangles(triCounti);er
							for (int tci = 0; tci < triCounti; tci++)
							{
								MPointArray trii;
								MIntArray vertexListi;
								stat = itmeshpi.getTriangle(tci, trii, vertexListi, MSpace::kWorld);
								while(itmeshpj.isDone(&stat) == false)
								{
									bool tj = itmeshpj.hasValidTriangulation(&stat);err
									if (tj)
									{
										int triCountj;
										stat = itmeshpj.numTriangles(triCountj);er
										for (int tcj = 0; tcj < triCountj; tcj++)
										{
											MPointArray trij;
											MIntArray vertexListj;
											stat = itmeshpj.getTriangle(tcj, trij, vertexListj, MSpace::kWorld);
											
											double p1[2] = {trii[0].x, trii[0].z};
											double q1[2] = {trii[1].x, trii[1].z};
											double r1[2] = {trii[2].x, trii[2].z};
											
											cout << "p1 x " << p1[0] << "y " << p1[1] << endl; 
											
											double p2[2] = {trij[0].x, trij[0].z};
											double q2[2] = {trij[1].x, trij[1].z};
											double r2[2] = {trij[2].x, trij[2].z};
											
											cout << "p2 x " << p2[0] << "y " << p2[1] << endl; 
											
											if ( tri_tri_overlap_test_2d(p1, q1, r1, p2, q2, r2))
											{
//												cout << "TRIANGLES OVERLAP" << endl;
												polyOverlapped = true;
												goto end;
											}
										}
									}
									itmeshpj.next();
								}
							}
							if (polyOverlapped)
							{
								cmd += tmp;
							}
						}
						itmeshpi.next();
					}
//					cout << i << " intersects " << j << endl;
				}
			}
		}
	}
	
end:
	
	for (size_t i = 0; i < objs.length(); i++)
		stat = MGlobal::deleteNode( objs[i] );err

	setResult(polyOverlapped);

	return stat;
}


// UNDO THE COMMAND
MStatus OverlappingUVShells::undoIt()
{
	return MStatus();
}
