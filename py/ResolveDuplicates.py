import maya.OpenMayaMPx as OpenMayaMPx
import maya.OpenMaya as OpenMaya
from sys import *
import re
from string import * 
from types import *
import maya.cmds as cmds
import NewtonRenaming

from unittest import *

reload(NewtonRenaming)

from NewtonRenaming import *

# Objects in maya are composed of three elements according to our naming scheme:
# <shortname>_<numbering>_<type>
#
# the numbering has a padding of 3 by default, but can get bigger (say if group has 1000 objects
# or more
#
# the type is either GRP GEO or PLY

# Objects which have the same shortname are considered to be duplicates
class Duplicate():

	# number of times this shortname appears
	count = 0
	
	# dag paths to these objects
	paths = []
	
	# initialise local vars properly
	def __init__(self):
		self.count = 1
		self.paths = []


# Atomic rename operation, can be done and undone
class RenameOperation():
	
	# new name of the object
	newName = ""
	
	# old name of the object (for undo)
	oldName = ""
	
	# this is for numering / number padding purposes
	numbering = 0
	
	# Maya MFnDependencyNode we want to rename
	node = OpenMaya.MFnDependencyNode()
	
	# Maya DagPath we want to rename
	path = OpenMaya.MDagPath()
	
	def __init__(self, shortName, numbering, path):
	
		self.numbering = numbering
		self.node = OpenMaya.MFnDependencyNode(path.node())
		self.oldName = self.node.name()
		self.path = path
		
		ns = str(numbering)
		ns = ns.zfill(3)
	
		self.newName = shortName + "_" + ns
		
		if hasType(self.oldName):
			self.newName = self.newName + "_" + getType(self.oldName)
	
	def redo(self):
		self.node.setName(self.newName)
	
	def undo(self):
		self.node.setName(self.oldName)


# Main class for resolving name clashes in the scene
class ResolveDuplicates(OpenMayaMPx.MPxCommand):

	# holds operations that need to be performed
	# in redo / undo
	operations = []

	def __init__(self):
		OpenMayaMPx.MPxCommand.__init__(self)

	def doIt(self, args):
	
		# this dictionary keeps track of how many times each object
		# has been encountered, i.e the key is a short name
		# the value is the number of times that short name was
		# encountered
		groupCounts = {}
		geoCounts = {}
		
		# clear operations on new call
		# to doIt
		self.operations = []
				
		# iterate over each and every tranform in the scene
		iterObj = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kTransform)

		while not iterObj.isDone():
		
			# get object's dag path
			dPath = OpenMaya.MDagPath()
			iterObj.getPath(dPath)
			
			# get leaf's name
			transformName = getLeafName(dPath)

			# skip out cameras
			if (transformName != "persp" and transformName != "top" and transformName != "side" and transformName != "front"):

				# get name minus numbering or type
				shortName = getShortName(transformName)

				utilx = OpenMaya.MScriptUtil()
				ptr = utilx.asUintPtr()
					
				dPath.numberOfShapesDirectlyBelow(ptr)
				
				numShapes = OpenMaya.MScriptUtil.getUint (ptr)
				
				isGeo = True if numShapes >= 1 else False

				if (isGeo):
				
					# how many times has this short name been encountered?
					count = groupCounts[shortName].count if shortName in groupCounts.keys() else None

					# if never, then set it to 1
					if (count is None):
						dup = Duplicate()
						dup.count = 1
						dup.paths.append(dPath)
						groupCounts[shortName] = dup
					else:
						# else increment it
						groupCounts[shortName].count += 1
						groupCounts[shortName].paths.append(dPath)
						
				else:
					# how many times has this short name been encountered?
					count = geoCounts[shortName].count if shortName in geoCounts.keys() else None

					# if never, then set it to 1
					if (count is None):
						dup = Duplicate()
						dup.count = 1
						dup.paths.append(dPath)
						geoCounts[shortName] = dup
					else:
						# else increment it
						geoCounts[shortName].count += 1
						geoCounts[shortName].paths.append(dPath)

			iterObj.next()

		self.resolveNames(groupCounts)
		self.resolveNames(geoCounts)
		
		self.redoIt()

	def resolveNames(self, counts):
		# loop over each short name's duplicates
		for it in counts.items():
			# if a short name has been encountered more than once
			if (it[1].count >= 1):
				i = 1
				# for each object with the same short name
				for dPath in it[1].paths:
					names = split(dPath.fullPathName(), "|")
					objName = names[-1]
					shortName = getShortName(objName)
					op = RenameOperation(shortName, i, dPath)
					i = i + 1
					self.operations += [op]

		
	def redoIt(self):

		for op in self.operations:
			op.redo()
	
	def undoIt(self):

		for op in self.operations:
			op.undo()

	def isUndoable(self):
		return True

def cmdCreator():
	return OpenMayaMPx.asMPxPtr( ResolveDuplicates() )

def initializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject, "Author", "1.0", "Any")
	try:
		mplugin.registerCommand( "newtonResolveDuplicates", cmdCreator)
	except:
		stderr.stdout.write( "Failed to register command: %s\n" % "newtonResolveDuplicates" )
		raise

def uninitializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject)
	try:
		mplugin.deregisterCommand( "newtonResolveDuplicates" )
	except:
		stderr.stdout.write( "Failed to unregister command: %s\n" % "newtonResolveDuplicates" )
		raise
