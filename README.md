# Overview

The Sanity Checking Tool performs sanity checks at the modeling phase. It acts as a user interface, is very easily extensible (see Adding User Checks) and can be called programmatically through other tools (such as publishing tools) to automatically check the sanity of assets before publish.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/SanityChecker.png)

#### Dependencies

osbTools contains MEL, Python and C++. It is also dependent on boost 1.47.0.

#### Invoking

From Maya command line:

```
source "osbSanityChecker.mel";
```

#### Check types

There are various types of checks:

* Perform: This is a one step check which performs its duties in a single click.

* Check, Select: This type of check is for information purposes only, it will detect a defect in the and enable the user to select the problematic components.

* Check, Select, Fix: This is a 3 step check which enables the user to run a test, select the problematic components, and fix them.

#### Scope

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityRunOnAllGeo.png)

When '''edit > Run on all geo''' is checked, the checks will be run on the whole scene. When it is unchecked the checks will run on the selected geometry only.

#### Modularity

Ticking the check-boxes to the left of the checks, then click '''edit > hide checked items''' will hide selected items from the window. This enables to only work with UV tests for example. This combined with the ability to add your own checks means users can use this as a framework to add their own checks and make the tool their own.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityModularity.png)

To unhide hidden checks, simply click '''edit > reset'''.

## Geo Checks

### Open edges / Border edge ###

This check looks for open borders in polygon shells and the fix closes them. This is a default Maya test.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityOpenFaces.png)

### Add borders to holes

This check looks for open borders in polygon shells and adds an edge loop to their border.


![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanitySplitBorders.png)


### Floating vertex/zero edge length

This checks for edges whose two vertices are less than 1e-05 units apart.


![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityZeroLengthEdge.png)


### No more than four sided polys

Makes sure that no polygonal faces have more than 4 edges. The Fix is performed by triangulating offending faces. This is a default maya test.


![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityFourSidedPolies.png)


### No non maniforld faces

Fixes non-manifold geometry, this is default maya functionality. For more information on non-manifold geometry please refer to the [
maya doc](http://download.autodesk.com/global/docs/maya2013/en_us/index.html?url=files/Polygons_overview_Twomanifold_vs._nonmanifold_polygonal_geometry.htm,topicNumber=d30e132600).

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityNonManifold.png)

### No lamina faces

This is a default maya test. From the maya documentation:

"Lamina faces share all of their edges. When you select Lamina Faces for removal, Maya removes faces that share all edges. By removing these types of faces, you can avoid unnecessary processing time, especially when you export the model to a game console. For example, suppose you performed Edit Mesh > Duplicate Face with the Separate Duplicate Faces option turned off. You would have two faces on top of each other. If you later merge the vertices of the two faces, they would share the same edges. You can remove the extra face using Cleanup with Lamina faces turned on."

### Delete shaders assign lambert1 shader

This check deletes all shaders in the scene and applies the default lambert material to all polygonal geometry

### Give warning if reference geo is in the scene

This checks whether there are any maya references left in the scene.

### Vallence check: No more than 11 edges going into one point

his check makes sure there are no more than 11 edges meeting atone single vertex.If there are more then the fix will remove every second edge.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityVallence.png)

### Check polygon normals facing the right direction

This checks whether the normals for the object are facing 'outwards'. It uses a combination of two statistical methods to do so, and selects the method with the highest level of confidence. Please note that this check assumes that normals are conformed before running.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityNormalDirection.png)

### Delete all history

This check deletes history on all nodes.

## Transform Checks

### Freeze transforms

This check freezes all transform nodes in the scene.

### Place top group node pivot to world 0

This places the top level group node's pivot at world (0,0,0). This check will not work if there is more than 1 top group transform.


![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityTopGroupPivot.png)


### no geo below -0.005 in Y

This check ensures that no geometry's bounding box is below -0.005

## Naming Conventions

### Check that all geo and group nodes have a number padding

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityNumberPadding.png)


This dialog offers renaming features for the entire hierarchy or for selection.

#### Inherit Parent's Name

[[File:osbSanityInherit.png]]

This feature simply renames geometry to its immediate parents' name.


![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityPadding.png)

This feature is a little more complex, first of all it makes sure that every geometry name in the scene is unique. This is useful as a lot of maya scripts fail when there are more than one object having the same name. It also numbers each piece of geometry according to its position within the hierarchy.

### Check that all geo and group nodes end in _GEO/_PLY/_GRP

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityNodeEndings.png)

Checking '''_GRP''' will add the '_GRP' suffix to the end of all groups.

Checking '''_PLY''' or '''_GEO''' will add either of those suffixes at the end of geo nodes. However it will only do so on geo nodes which don't currently have a suffix. If you want to override the existing suffix you'll have to first tick '''force _PLY|_GEO'''.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityGRPGEO.png)

## UV Checking

### Check there are no open UV seams


![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityUVSeams.png)

This check detects seams (cuts) in UVs which are not otherwise visible. For example if you select edges on a model and use the scissors tool to cut them, this is a perfect example of a seam which this check will detect.

### Check no UV edges are overlapping a tile border

Check that no UVs are overlapping a tile border. This is important as all the UVs per object need to fit within one grid tile.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityUVOverlappingTileBorder.png)

### Check no UV edges are overlapping each other

Check no UVs/UV shells are overlapping each other. UV Shells should not overlap since they'll inherit the same texture pixels if they do. This check makes sure no shells within the same object overlap.

'''n.b.''' this check seems to currently be broken.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityUVOverlap.png)

### Check no UVs are reversed/inverted (like poly normals

This checks that no UVs are facing down, to make sure textures don't show up inverted.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityReversedUVs.png)

### (Check no UVs are in -1 tiles in Y and X

Make sure UVs are only in the positive areas of the UV grid.

![](https://raw.githubusercontent.com/shyal/osbTools/master/docs/images/OsbSanityUVsInRightQuarter.png)

## Scene Check 
### Delete layers/unimportant DAG nodes

This check deletes the defaultLightSet, the defaultObjectSet, displayLayers, defaultLayers, defaultRenderLayers, brushs ikRPsolvers, ikSCsolvers, ikSplineSolvers, DisplayLayers and RenderLayers

## Adding User Checks

The sanity checker is a simple API allowing you to easily add your own checks.

You can either put your own checks in osbUserChecks.mel directly, or put them in seperate mel files and source those files from osbUserChecks.mel

Your checks should start with:

```
source "osbSanityCheckerAPI.mel";
```


The registration command is:

```
osbAddSanityCheck("exampleCheckName",
    "Example Category Name",
    "Example label - this name appears in the UI",
    "Example Tooltip - this help shows up when hovering over the check in the UI",
    "Perform");
```

The last string can either be "Perform" or "Perform,Select,fix". A "Perform" check will do all that is required in one function. For this check you'll need to declare and define a global function that has the same name as the first argument, so:

```
global proc exampleCheckName() 
{
    // checking code here
}
```

The function signature contains no arguments and the function has no return value. Within the function, you can assume that the correct objects that you want to check are currently selected.

The second kind of check is "Perform,Select,fix". For this type of check, the objects / components which remain selected when exampleCheckName() returns are the objects that will get added to the selection set which flags them as being problematic.

```
global proc exampleCheckNameFix() 
{
    // fixing code here
}
```

Once again, in this function, you can assume that the problematic objects / components that you want to fix are currently selected.

Your best option to get up to running is to take a look at existing checks inside the 'checks' directory.

If the checks that you add are used a lot please feel free to add them to the main tool and check your changes into the newton repos in bitbucket. Please also note that existing checks can be hidding by using edit > Hide selected checks, go ahead and hide checks that you do not use, and implement your own checks to make this tool your own.
