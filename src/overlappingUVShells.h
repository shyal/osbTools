#ifndef _OVERLAPPING_UV_SHELLS_H_
#define _OVERLAPPING_UV_SHELLS_H_

#include <maya/MArgList.h>
#include <maya/MStatus.h>

//! This class represents the plugin to check whether UVs are overlapping
class OverlappingUVShells : public MPxCommand
{
	public:
								//! Default public constructor
								OverlappingUVShells();

								//! Default public destructor
		virtual 				~OverlappingUVShells();

		//! Node creator
		static void* 			creator();

		//! Specifies whether the action is undable
		bool 					isUndoable() const;

		//! Performs the node's duty
		MStatus 				doIt(const MArgList&);

		//! Undoes what was done in doIt
		MStatus 				undoIt();
	
};

#endif
