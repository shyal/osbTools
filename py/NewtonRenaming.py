import maya.OpenMayaMPx as OpenMayaMPx
import maya.OpenMaya as OpenMaya
from sys import *
import re
from string import * 
from types import *
import maya.cmds as cmds

def hasPadding(name):
	m = re.search('_(\d+)', name)
	return True if m != None and m.group(0) != "" else False

def hasType(name):
	m = re.search('(PLY|GEO|GRP)', name)
	return True if m != None and m.group(0) != "" else False
	
def getType(name):
	m = re.search('(PLY|GEO|GRP)', name)
	return m.group(0) if m != None and m.group(0) != "" else ""

def getLeafName(aFullPath):
	names = split(aFullPath.fullPathName(), "|")
	objName = names[-1]
	return objName
	
def setNumberPadding(name, number):
	if hasPadding(name):
		ns = str(number)
		ns = ns.zfill(3)
		name = re.sub(r'_(\d+)', ns, name)
	return name
	
def removeTrailingNumbers(name):
	name = re.sub(r'\d*$', '', name)
	return name

def getShortName(name):
	name = re.sub(r'_(PLY|GEO|GRP)', '', name)
	name = re.sub(r'_(\d+)', '', name)
	name = re.sub(r'_*$', '', name)
	return name

def setShortName(newName, dPath):
	dep = OpenMaya.MFnDependencyNode(dPath.node())
	name = dep.name()
	
	shortName = getShortName(name)

	name = re.sub(shortName, newName, name)
	dep.setName(name)

#unit tests

sn = getShortName("taj_roof_001_PLY")
if sn != "taj_roof":
	print "error in getShortName -> taj_roof_001_PLY should return taj_roof but returned: " + sn + "\n"
	raise AssertionError

sn = getShortName("taj_roof_PLY")
if sn != "taj_roof":
	print "error in getShortName -> taj_roof_PLY should return taj_roof but returned: " + sn + "\n"
	raise AssertionError
	
sn = getShortName("taj1_roof_PLY")
if sn != "taj1_roof":
	print "error in getShortName -> taj_roof_PLY should return taj1_roof but returned: " + sn + "\n"
	raise AssertionError
	
sn = getShortName("taj1roof_PLY")
if sn != "taj1roof":
	print "error in getShortName -> taj_roof_PLY should return taj1roof but returned: " + sn + "\n"
	raise AssertionError
	
sn = getShortName("taj1_PLY")
if sn != "taj1":
	print "error in getShortName -> taj_roof_PLY should return taj1 but returned: " + sn + "\n"
	raise AssertionError

sn = getShortName("taj_roof_")
if sn != "taj_roof":
	print "error in getShortName -> taj_roof_PLY should return taj_roof but returned: " + sn + "\n"
	raise AssertionError

