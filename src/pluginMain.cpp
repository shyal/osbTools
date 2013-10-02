
#include <maya/MFnPlugin.h>
#include "intersect.h"
#include "UVSeams.h"
#include "overlappingUVShells.h"
#include "UVReversed.h"
#include "Renaming.h"
#include "holesBorder.h"

#define er if (stat != MS::kSuccess) cout << "trolol " << __FILE__ << ":" << __LINE__ << endl;

MStatus initializePlugin( MObject obj )
{
    MFnPlugin plugin( obj, "Oliver Shyal Beardsley", "V1.0.0", "osb" );

    MStatus stat;
	stat = plugin.registerCommand("checkNormalDirection", checkNormalDirection::creator);er
	stat = plugin.registerCommand("UVSeams", UVSeams::creator);er
	stat = plugin.registerCommand("OverlappingUVShells", OverlappingUVShells::creator);er
	stat = plugin.registerCommand("UVReversed", UVReversed::creator);er
	stat = plugin.registerCommand("osbTypeRename", TypeRename::creator);er
	stat = plugin.registerCommand("holesBorder", holesBorder::creator);er
    return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj)
{
        MFnPlugin plugin( obj );

        MStatus stat;
		
		stat = plugin.deregisterCommand("holesBorder");er
		stat = plugin.deregisterCommand("osbTypeRename");er
		stat = plugin.deregisterCommand("UVReversed");er
		stat = plugin.deregisterCommand("OverlappingUVShells");er
		stat = plugin.deregisterCommand("UVSeams");er
		stat = plugin.deregisterCommand("checkNormalDirection");er
        return MS::kSuccess;
}
