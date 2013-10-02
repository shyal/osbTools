#ifndef _UVReversed_H_
#define _UVReversed_H_

#include <maya/MArgList.h>
#include <maya/MStatus.h>

class UVReversed : public MPxCommand
{
	public:
								//! Default public constructor
								UVReversed();
								
								//! Default public destructor
		virtual 				~UVReversed();

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
