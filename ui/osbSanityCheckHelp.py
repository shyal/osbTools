import os, sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import maya.cmds as cmds
import maya.mel as mel

from pfUtils.v1_2.qt.dialog import PfQDialog

import osbTools.v1_0
reload(osbTools)
from osbTools.v1_0 import osbSanityCheckHelp_UI

global w

class win(PfQDialog):
	def __init__(self, parent=None, modal=False):
		#call parent's init
		PfQDialog.__init__(self, parent, modal=modal)
		
		#setup main UI from QtDesigner file
		self.ui = osbSanityCheckHelp_UI.Ui_Dialog()
		self.ui.setupUi( self )
		self.setObjectName('osbSanityCheck')
		self.ui.textEdit.insertHtml("\
<html>\
<body bgcolor=\"silver\"> \
\
<img src=\"/home/shyalb/Desktop/w3schools.jpg\" width=\"104\" height=\"142\" />\
The sanity checker is a simple API allowing you to easily add your<br>\
own checks.<br>\
<br>\
It is currently installed in:<br>\
<br>\
<br>\
<br>\
In that same folder you will find a file called \"userChecks.mel\" you can either put your own checks in userChecks.mel directly, or put them in seperate mel files and source those files from userChecks.mel<br>\
<br>\
Your checks should start with:<br>\
<br>\
source \"osbSanityCheckerAPI.mel\"; <br>\
<br>\
The registration command is:<br>\
<br>\
osbAddSanityCheck(\"exampleCheckName\",<br>\
    \"Example Category Name\",<br>\
    \"Example label - this name appears in the UI\",<br>\
    \"Example Tooltip - this help shows up when hovering over the check in the UI\",<br>\
    \"Perform\");<br>\
<br>\
The last string can either be \"Perform\" or \"Perform,Select,fix\". A \"Perform\" check will do all that is required in one function. For this check you'll need to declare and define a global function that has the same name as the first argument, so:<br>\
<br>\
global proc exampleCheckName() <br>\
{<br>\
    // checking code here<br>\
}<br>\
<br>\
The function signature contains no arguments and the function has no return value. Within the function, you can assume that the correct objects that you want to check are currently selected.<br>\
<br>\
The second kind of check is \"Perform,Select,fix\". For this type of check, the objects / components which remain selected when exampleCheckName() returns are the objects that will get added to the selection set which flags them as being problematic.<br>\
<br>\
global proc exampleCheckNameFix() <br>\
{<br>\
    // fixing code here<br>\
}<br>\
<br>\
Once again, in this function, you can assume that the problematic objects / components that you want to fix are currently selected.<br>\
<br>\
Your best option to get up to running is to take a look at existing checks inside the 'checks' directory.<br>\
<br>\
If the checks that you add are used a lot please feel free to add them to the main tool and check your changes into the newton repos in bitbucket.<br>\
Please also note that existing checks can be hidding by using edit > Hide selected checks, go ahead and hide checks that you do not use, and implement your own checks to make this tool your own.\
\
</body>\
</html>\
")
	
def main():
	global w
	w = win()
	w.show()
