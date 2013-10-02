// "There is nothing noble in being superior to your fellow man; true nobility is being superior to your former self."
// -Ernest Hemingway

#include "UVCommon.h"

#include <maya/MStatus.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFloatPointArray.h>
#include <maya/MVector.h>
#include <vector>
#include "assert.h"

using namespace std;


#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;

UVCommon::UVCommon()
{
	
}

UVCommon::~UVCommon()
{
	
}

// This function creates a mesh representation of the UVs
MObject UVCommon::getUVMesh(MFnMesh& fnMesh)
{
	MStatus stat;
	int numUVs = fnMesh.numUVs(&stat);er

	MFloatArray uArray, vArray;

	stat = fnMesh.getUVs(uArray, vArray);

	MFloatPointArray vertices(uArray.length());

	for (size_t i = 0; i < uArray.length(); i++)
		vertices[i] = MFloatPoint(uArray[i], 0, vArray[i]);
	
	MObject fnobj = fnMesh.object(&stat);er

	MItMeshPolygon itmesh(fnobj, &stat);er

	size_t numUVFaces = 0;

	MIntArray polygonCounts;

	MIntArray polygonConnects;

	size_t i = 0;
	for (itmesh.reset(); itmesh.isDone() == false; itmesh.next(), i++)
	{
		int polygonId = itmesh.index(&stat);er
		unsigned int vertexCount = itmesh.polygonVertexCount(&stat);er
		if (itmesh.hasUVs(&stat))
		{
			er
			numUVFaces++;
			polygonCounts.append(0);
			for (size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
			{
				polygonCounts[i]++;
				int uvId;
				stat = fnMesh.getPolygonUVid(polygonId, vertexIndex, uvId);er
				polygonConnects.append(uvId);
			}
		}
	}

	MFnMesh uvMesh;

	MObject retObj = uvMesh.create(numUVs, numUVFaces, vertices, polygonCounts, polygonConnects, MObject::kNullObj, &stat);er

	return retObj;
}

// This function creates a different mesh for each UV shell
// do not read unless you want subjected to an intense
// headache followed by a severe brain hemorrhage
MObjectArray UVCommon::getUVShells(MFnMesh& fnMesh)
{
	MStatus stat;
	MIntArray uvShellIds;
	unsigned int nbUvShells;
	
	stat = fnMesh.getUvShellsIds(uvShellIds, nbUvShells);er
	
	MIntArray nbUvPerShell(nbUvShells);
	MIntArray UvsRemapped(uvShellIds.length());
	
	for (size_t j = 0; j < uvShellIds.length(); j++)
		UvsRemapped[j] = nbUvPerShell[uvShellIds[j]]++;

	MObjectArray arr(nbUvShells);

	MFloatArray uArray, vArray;

	stat = fnMesh.getUVs(uArray, vArray);

	MObject fnobj = fnMesh.object(&stat);er

	MItMeshPolygon itmesh(fnobj, &stat);er

	vector<size_t> numUVFaces(nbUvShells);
	vector<MIntArray> polygonCounts(nbUvShells);
	vector<MIntArray> polygonConnects(nbUvShells);
	vector<MFloatPointArray> vertices(nbUvShells);
	
	for (size_t i = 0; i < uArray.length(); i++)
		vertices[uvShellIds[i]].append(MFloatPoint(uArray[i], 0, vArray[i]));

	for (itmesh.reset(); itmesh.isDone() == false; itmesh.next())
	{
		int polygonId = itmesh.index(&stat);er
		unsigned int vertexCount = itmesh.polygonVertexCount(&stat);er
		assert(vertexCount);
		if (itmesh.hasUVs(&stat))
		{
			er
			double area;
			itmesh.getUVArea(area);
			int uvId;
			stat = fnMesh.getPolygonUVid(polygonId, 0, uvId);er
			numUVFaces[uvShellIds[uvId]]++;
			polygonCounts[uvShellIds[uvId]].append(0);
			for (size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
			{
				polygonCounts[uvShellIds[uvId]][polygonCounts[uvShellIds[uvId]].length()-1]++;
				stat = fnMesh.getPolygonUVid(polygonId, vertexIndex, uvId);er
				polygonConnects[uvShellIds[uvId]].append(UvsRemapped[uvId]);
			}
		}
	}

	MFnMesh uvMesh;

	for (size_t j = 0; j < nbUvShells; j++)
		arr[j] = uvMesh.create(nbUvPerShell[j], numUVFaces[j], vertices[j], polygonCounts[j], polygonConnects[j], MObject::kNullObj, &stat);er
	
	return arr;
}































