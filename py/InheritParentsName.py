import maya.OpenMayaMPx as OpenMayaMPx
import maya.OpenMaya as OpenMaya
from sys import *
import re
from string import * 
from types import *
import maya.cmds as cmds
import NewtonRenaming

reload(NewtonRenaming)

from NewtonRenaming import *

class RenameOperation():
	newName = ""
	oldName = ""
	node = OpenMaya.MFnDependencyNode()
	path = OpenMaya.MDagPath()
	
	def __init__(self, nn, p):
		self.newName = nn
		self.node = OpenMaya.MFnDependencyNode(p.node())
		self.oldName = self.node.name()
		self.path = p
	
	def redo(self):
		self.node.setName(self.newName)
	
	def undo(self):
		self.node.setName(self.oldName)
	
	def redoShortName(self):
		short = getShortName(self.newName)
		setShortName(short, self.path)
	
	def undoShortName(self):
		self.node.setName(self.oldName)

class InheritParentsName(OpenMayaMPx.MPxCommand):

	operations = []
	
	def __init__(self):
		OpenMayaMPx.MPxCommand.__init__(self)

	def doIt(self, args):
			
		iterObj = OpenMaya.MItDag(OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kMesh)
		
		idd = 0
		
		self.operations = []
		
		while not iterObj.isDone():
			dPath = OpenMaya.MDagPath( )
			iterObj.getPath( dPath )
			
			dPath.pop()

			transformName = ""

			names = split(dPath.fullPathName(), "|")
			parentName = names[-2]
			op = RenameOperation(parentName, dPath)

			self.operations += [op]

			iterObj.next()
		
		self.redoIt()

	def redoIt(self):
		print "redoing"
		for op in self.operations:
			op.redoShortName()
			
	def undoIt(self):
		print "undoing"
		for op in self.operations:
			op.undoShortName()
		
	def isUndoable(self):
		return True

def cmdCreator():
	return OpenMayaMPx.asMPxPtr( InheritParentsName() )

def initializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject, "Author", "1.0", "Any")
	try:
		mplugin.registerCommand( "newtonInheritParentsName", cmdCreator)
	except:
		print( "Failed to register command: %s\n" % "newtonInheritParentsName" )
		raise

def uninitializePlugin(mobject):
	mplugin = OpenMayaMPx.MFnPlugin(mobject)
	try:
		mplugin.deregisterCommand( "newtonInheritParentsName" )
	except:
		stderr.stdout.write( "Failed to unregister command: %s\n" % "newtonInheritParentsName" )
		raise
