#ifndef _INTERSECT_H_
#define _INTERSECT_H_

#include <maya/MArgList.h>
#include <maya/MStatus.h>
#include <maya/MPxCommand.h>
#include <maya/MFloatPoint.h>
#include <maya/MFnMesh.h>

//! This class represents the plugin to check whether an mesh is inside out or outside in
class checkNormalDirection : public MPxCommand
{
	public:
								//! Default public constructor
								checkNormalDirection();

								//! Default public destructor
		virtual 				~checkNormalDirection();

		//! Node creator
		static void* 			creator();

		//! Specifies whether the action is undable
		bool 					isUndoable() const;

		//! Performs the node's duty
		MStatus 				doIt(const MArgList&);

		//! Undoes what was done in doIt
		MStatus 				undoIt();
		
	private:

		//! This function encapsulates the fast intersection of a vector with a mesh
		MStatus 				intersectVectorWithMesh(
															int& nNumberHitPoints,
															const MFloatPoint& pos,
															const MFloatVector& dir,
															MFnMesh& fnMesh,
															MMeshIsectAccelParams& mmAccelParams
														);

		//! Enumerates whether a normal is facing inwards or outwards
		enum 					NormalDirection
		{
									eINWARDS,
									eOUTWARDS
		};

		//! Randomly shoots rays from outside the object towards its centre, computes direction of normals as a probability
		MStatus 				methodA(NormalDirection& normalDir, float& p, const MArgList& args);
		//! Shoots rays off each normal and counts how many times it penetrates the object, based on this computers the direction of normals as a probability
		MStatus 				methodB(NormalDirection& normalDir, float& p, const MArgList& args);
};

#endif
