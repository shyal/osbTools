#ifndef _UV_COMMON_H_
#define _UV_COMMON_H_

#include <maya/MObject.h>
#include <maya/MObjectArray.h>

class MFnMesh;

//! This class provides common functionality for UV classes in the codebase
class UVCommon
{
	public:
							//! Default public constructor
							UVCommon();
							
							//! Default public destructor
							~UVCommon();
							
	/*! Given an MFnMesh, create a new MFnMesh representation of the UVs
	 *  The resulting mesh created by this function is the exact
	 *  polygon representation of the UVs as seen in the UV editor
	 *  turning UVs into polygons enables us to perform work on UVs
	 *  while using algorithms reserved to polys
	 */
	static MObject 			getUVMesh(MFnMesh& mesh);
	
	/*! Given an MFnMesh, extract all UV Shells into an object array
	 *  This function has to exist because the UV set of a polygon object
	 *  can be formed of more than a single shell.
	 */
	static MObjectArray 	getUVShells(MFnMesh& mesh);
};

#endif
