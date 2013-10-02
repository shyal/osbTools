#ifndef _Renaming_H_
#define _Renaming_H_
#include <maya/MArgList.h>
#include <maya/MStatus.h>
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MFnDependencyNode.h>
#include <map>
#include <vector>

/*! This class represents an atomic rename operation
 * it does not perform any work other than the renaming itself
*/
struct Operation
{
	/*! Public constructor
	 * initialise this class by passing the objects new name, its old name,
	 * and dag path towards the object you want to rename.
	 */
	Operation(const MString& newName, const MString& oldName, const MDagPath&);
	
	//! Default public destructor
	Operation();
	
	//! Performs the node's duty
	void					doIt();
	
	//! Undo what was done in doit
	void					undoIt();
	
	//! The new name for the object
	MString 				m_newName;
	
	//! The old name for the object
	MString 				m_oldName;
	
	//! The path towards the object you want to rename
	MDagPath				m_path;
	
};

/*! This class takes care of renaming objects based on their type
 * There are 3 types of objects, GRP which contain transforms, PLY and GEO
 * which are transforms of polygons. Whether an object is PLY or GEO is
 * specified by the end user.
*/
class TypeRename : public MPxCommand
{
	public:
	
		//!Default public constructor
								TypeRename();
		
		//!Default public destructor
		virtual 				~TypeRename();
		
		//! Node creator
		static void* 			creator();
		
		//! Specifies whether the action is undable
		bool 					isUndoable() const;
		
		//! Performs the node's duty
		MStatus 				doIt(const MArgList&);
		
		//! Undoes what was done in doIt
		MStatus 				undoIt();
		
		//! Redoes what was undone, this function usually performs the actual work
		MStatus 				redoIt();
		
	private:
		//! These are the rename operations which need to be performed
		std::vector<Operation>	m_operations;
};

#endif
