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
#include <maya/MItDag.h>
#include <maya/MGlobal.h>
#include <maya/MFnTransform.h>

#include "Renaming.h"
#include "UVCommon.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <assert.h>

#include <boost/algorithm/string.hpp>
using namespace boost;
using namespace std;

#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;

Operation::Operation(const MString& newName, const MString& oldName, const MDagPath& path):
	m_newName(newName),
	m_oldName(oldName),
	m_path(path)
{

}

Operation::Operation()
{
}

void Operation::doIt()
{
	MFnDependencyNode node(m_path.node());
	node.setName(m_newName);
}

void Operation::undoIt()
{
	MFnDependencyNode node(m_path.node());
	node.setName(m_oldName);
}

//***************************************************************************************************************

TypeRename::TypeRename()
{
}

TypeRename::~TypeRename()
{
}

void* TypeRename::creator()
{
   return new TypeRename;
}

bool TypeRename::isUndoable() const
{
   return true;
}

// This command adds _GRP on groups and _GEO on geometry.

MStatus TypeRename::doIt(const MArgList& args)
{
	MStatus stat;

	bool dogrp = args.asString(0, &stat) == "grp";er;
	bool doply = args.asString(1, &stat) == "ply";er;
	bool dogeo = args.asString(1, &stat) == "geo";er;
	bool doall = args.asString(2, &stat) == "all";er;
	bool force = args.asBool(3, &stat) == true;er;

	MSelectionList list;
	
	m_operations.resize(0);

	MGlobal::getActiveSelectionList(list);

	{
		MItDag itdag(MItDag::kDepthFirst, MFn::kTransform, &stat);er

		MItSelectionList iter(list, MFn::kTransform);

		// For every group in the scene (depth first)
		while (dogrp && (doall ? !itdag.isDone() : !iter.isDone()))
		{
			MDagPath path;
			doall ? stat = itdag.getPath(path) : stat = iter.getDagPath(path);er
		
			MObject obj = path.node(&stat);er

			unsigned int childCount = path.childCount(&stat);
		
			bool transformIsGroup = childCount > 1; 

			// if we only have one child and it happens to be a transform
			// then let's consider this a group too
			if (childCount == 1)
			{
				MObject child = path.child(0, &stat);er
				MFnTransform trans(child, &stat);
				if (stat == MS::kSuccess)
				transformIsGroup = true;
			}

			if (transformIsGroup)
			{
				MFnDependencyNode node(path.node(), &stat);er
			
				// get the name of the node
				string name = node.name().asChar();

				// remove all those strings
				erase_all(name, "_GRP");
				erase_all(name, "_GEO");
				erase_all(name, "_PLY");
			
				// name it as a group since it has more than one child
				Operation op((name + "_GRP").c_str(), node.name(), path);
				m_operations.push_back(op);
			}
			
			if (doall)
			{
				stat = itdag.next();
				er
			}
			else
			{
				stat = iter.next();
				er
			}
		}
	}

	{
		MItDag itdag = MItDag(MItDag::kDepthFirst, MFn::kMesh, &stat);er
		MItSelectionList sel(list, MFn::kMesh);

		int counter = 0;

		// Iterate over Meshes
		while ((doply || dogeo) && (doall ? !itdag.isDone() : !sel.isDone()))
		{
			MDagPath path;
			stat = doall ? itdag.getPath(path) : sel.getDagPath(path);			
			path.pop();

			MObject transformObj = path.transform(&stat);er

			MFnDependencyNode transformOfMeshNode(transformObj, &stat);er
			MFnDependencyNode meshNode(path.node(), &stat);er

			MString name = transformOfMeshNode.name();
			std::string namestr = name.asChar();

			bool isPly = std::string::npos != namestr.find("_PLY");
			bool isGeo = std::string::npos != namestr.find("_GEO");

			if (isGeo || isPly)
			{
				if (force)
				{
					string str(name.asChar());
					erase_all(str, "_GEO");
					erase_all(str, "_PLY");
					name = str.c_str();
				}
				else
				{
					doall ? itdag.next() : sel.next();
					continue;
				}
			}
		
			MString newName;
		
			if (doply)
				newName = (name + "_PLY");
			else if (dogeo)
				newName = (name + "_GEO");
			
			Operation op(newName, transformOfMeshNode.name(), path);
			
			m_operations.push_back(op);
			
			doall ? itdag.next() : sel.next();
		}
	}
	
	return redoIt();
}

// UNDO THE COMMAND
MStatus TypeRename::undoIt()
{
	for (size_t i = 0; i < m_operations.size(); i++)
		m_operations[i].undoIt();
	return MStatus();
}

// REDO THE COMMAND
MStatus TypeRename::redoIt()
{
	for (size_t i = 0; i < m_operations.size(); i++)
		m_operations[i].doIt();
	return MStatus();
}
