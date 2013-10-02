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

#include "intersect.h"

#include <iostream>
#include <cstdlib>
#include <sstream>

#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;

using namespace std;

#ifndef INTERSECTCMD
#define INTERSECTCMD

#define MERR_CHK(stat,msg) if ( !stat ) { MGlobal::displayError(msg); } // cerr << msg << endl; }

#endif

checkNormalDirection::checkNormalDirection()
{
}

checkNormalDirection::~checkNormalDirection()
{
}

void* checkNormalDirection::creator()
{
   return new checkNormalDirection;
}

bool checkNormalDirection::isUndoable() const
{
   return false;
}

MStatus checkNormalDirection::doIt(const MArgList& args)
{
	float pa, pb;
	NormalDirection normalDira, normalDirb;
	
	methodA(normalDira, pa, args);
	methodB(normalDirb, pb, args);
	
	if (pa >= pb && normalDira == eOUTWARDS)
	{
		setResult("outwards");
	}
	else if (pa >= pb && normalDira == eINWARDS)
	{
		setResult("inwards");
	}	
	if (pa < pb && normalDirb == eOUTWARDS)
	{
		setResult("outwards");
	}
	else if (pa < pb && normalDirb == eINWARDS)
	{
		setResult("inwards");
	}
	return MS::kSuccess;
}

// UNDO THE COMMAND
MStatus checkNormalDirection::undoIt()
{
	return MStatus();
}

MStatus checkNormalDirection::methodB(NormalDirection& normalDir, float& p, const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;

	if (args.length() < 2) 
	{

		stringstream ss;
		ss << "Need name of the mesh and the number of faces that you want to inspect." << endl;
		ss << "num args given " << args.length() << endl;

		MGlobal::displayError(MString(string(ss.str()).c_str()));
		return MStatus::kFailure;
	}
	
	MString meshName = args.asString(0, &stat);er
	int numFacesToInspect = args.asInt(1, &stat);er
	
	MSelectionList list;
	list.add(meshName);
	MDagPath path;
	stat = list.getDagPath(0, path);er
	MFnMesh fnMesh(path, &stat);er

	MMeshIsectAccelParams mmAccelParams = fnMesh.autoUniformGridParams();

	int numCollisions = 0;

	int numfaces = fnMesh.numPolygons(&stat);er

	MItMeshPolygon itmesh(path, MObject::kNullObj, &stat);er
	
	int numPairIntersections = 0;
	
	MFloatVector normal;
	MVector vec;
	MFloatPoint center;
	
	int skipFactor = numfaces/numFacesToInspect;
	size_t i = 0;
	
	int numChecksPerformed = 0;
	
	for (itmesh.reset(); itmesh.isDone() == false; itmesh.next(), i++)
	{
		if (skipFactor > 1 && i % skipFactor != 0)
			continue;
		stat = itmesh.getNormal(vec,MSpace::kObject);
		normal = vec;
		center = MFloatPoint(itmesh.center(MSpace::kObject, &stat));er
		// nudge starting position so we don't self intersect
		center += normal*0.01;
		stat = intersectVectorWithMesh(numCollisions, center, normal, fnMesh, mmAccelParams);
		if (numCollisions%2 == 0) numPairIntersections++;
		numChecksPerformed++;
	}
		
	float dirp = numPairIntersections/(float)numChecksPerformed;
	
	if (dirp >= .5)
	{
		p = (numPairIntersections/(float)numChecksPerformed-0.5)*2.0;
		normalDir = eOUTWARDS;
	}
	else
	{
		p = ((numChecksPerformed-numPairIntersections)/(float)numChecksPerformed-0.5)*2.0;
		normalDir = eINWARDS;
	}
	
	return stat;	
}

MStatus checkNormalDirection::methodA(NormalDirection& normalDir, float& p, const MArgList& args)
{
	MStatus stat = MStatus::kSuccess;

	if (args.length() < 2) 
	{
		MGlobal::displayError("Need name of the mesh and the number of faces that you want to inspect");
		return MStatus::kFailure;
	}
	
	MString meshName = args.asString(0, &stat);er
	int numFacesToInspect = args.asInt(1, &stat);er
		
	MSelectionList list;
	list.add(meshName);
	MDagPath path;
	stat = list.getDagPath(0, path);er
	MFnMesh fnMesh(path, &stat);er

	MMeshIsectAccelParams mmAccelParams = fnMesh.autoUniformGridParams();

	int numfaces = fnMesh.numPolygons(&stat);er
	
	if (numfaces <= 2)
	{
		setResult("NA");
		return MS::kFailure;
	}

	MItMeshPolygon itmesh(path, MObject::kNullObj, &stat);er
	MItMeshPolygon itmesh2(path, MObject::kNullObj, &stat);er
	MItMeshVertex itmeshvtx(path, MObject::kNullObj, &stat);er
	MBoundingBox bbox;

	for (itmeshvtx.reset(); itmeshvtx.isDone() == false; itmeshvtx.next())
		bbox.expand(itmeshvtx.position(MSpace::kWorld));
		
	MVector normal;
	MVector vec;
	MFloatPoint center;
	MFloatPoint rayPos;
		
	int numChecksPerformed = 0;
	
	srand(0);
		
	int outwardCount = 0;
	
	size_t counterc = 0;
	
	while (numChecksPerformed < numFacesToInspect)
	{
		if (counterc++ >= 5000)
			break;
		
		bool collided = false;
		
		center.x = bbox.center().x;
		center.y = bbox.center().y;
		center.z = bbox.center().z;
		
		int counter = 0;
		// generate random point as the origine of the ray, piont that is definitely outside the bounding box
		do
		{
			rayPos.x = bbox.center().x+((rand()/(float)RAND_MAX)-0.5)*bbox.width()*2;
			rayPos.y = bbox.center().y+((rand()/(float)RAND_MAX)-0.5)*bbox.height()*2;
			rayPos.z = bbox.center().z+((rand()/(float)RAND_MAX)-0.5)*bbox.depth()*2;
		}while(bbox.contains(rayPos) && counter++ < 500);
		
		MFloatVector ray = center-rayPos;

//		drawVector(rayPos, ray);

		MFloatPoint collisionPos;

		float fHitRayParam, fHitBary1, fHitBary2;
		int nHitFace, nHitTriangle;

		// a large positive number is used here for the maxParam parameter
		collided = fnMesh.closestIntersection(rayPos, ray, NULL, NULL, false, MSpace::kWorld, (float)1, false, &mmAccelParams, collisionPos, &fHitRayParam, &nHitFace, &nHitTriangle, &fHitBary1, &fHitBary2, (float)1e-6, &stat);
		
		// it *should* collide, but if for some mysterious reason
		// it doesn't then it's no big deal... this is the maya API after all
		if (!collided)
			continue;

//		polySphere(collisionPos);

		int pv = nHitFace > 0 ? nHitFace-1 : 1;
		itmesh2.setIndex(nHitFace, pv);
		itmesh2.getNormal(normal);
		
		// get dot product with ray
		
		ray.normalize();
		normal.normalize();
	
		double dot = normal * ray;
	
		if (dot < 0)
			outwardCount++;
		
		numChecksPerformed++;
	}
	
	if (outwardCount > numChecksPerformed / 2)
	{
		p = (outwardCount/(float)numChecksPerformed-0.5)*2.0;
		normalDir = eOUTWARDS;
	}
	else
	{
		p = ((numChecksPerformed-outwardCount)/(float)numChecksPerformed-0.5)*2.0;
		normalDir = eINWARDS;
	}

	return stat;
}

MStatus checkNormalDirection::intersectVectorWithMesh(int& nNumberHitPoints, const MFloatPoint& pos, const MFloatVector& dir, MFnMesh& fnMesh, MMeshIsectAccelParams& mmAccelParams)
{
	MStatus stat = MStatus::kSuccess;

	nNumberHitPoints = 0;

	static MFloatPoint hitPoint;

	static float fHitRayParam, fHitBary1, fHitBary2;
	static int nHitFace, nHitTriangle;

	// a large positive number is used here for the maxParam parameter
	static bool bAnyIntersection;
	bAnyIntersection = fnMesh.anyIntersection(pos, dir, NULL, NULL, false, MSpace::kObject, (float)9999, false, &mmAccelParams, hitPoint, &fHitRayParam, &nHitFace, &nHitTriangle, &fHitBary1, &fHitBary2, (float)1e-6, &stat);
	
	if (! bAnyIntersection) 
	{
		return stat;
	}

	static MFloatPointArray hitPoints;
	static MFloatArray faHitRayParams;
	static MIntArray iaHitFaces;
	static MIntArray iaHitTriangles;
	static MFloatArray faHitBary1;
	static MFloatArray faHitBary2;

	static bool bAllIntersections;
	bAllIntersections = fnMesh.allIntersections(pos, dir, NULL, NULL, false, MSpace::kObject, 9999, false, NULL, false, hitPoints, &faHitRayParams, &iaHitFaces, &iaHitTriangles, &faHitBary1, &faHitBary2, 0.000001f, &stat);
	
	if (! bAllIntersections)
	{
		return stat;
	}
	
	// check how many intersections are found
	nNumberHitPoints = hitPoints.length();

	return stat;
}
