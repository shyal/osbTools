#ifndef _UVSEAMS_H_
#define _UVSEAMS_H_

#include <maya/MArgList.h>
#include <maya/MStatus.h>

/*! Find seams in the default UV set of the given mesh.
  * Seams are when two UV edges are perfectly aligned (with a dot product of =~ 1)
  * and share a vertex
  */
class UVSeams : public MPxCommand
{
	public:
								//! Default public constructor
								UVSeams();
								
								//! Default public destructor
		virtual 				~UVSeams();
		
		
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
