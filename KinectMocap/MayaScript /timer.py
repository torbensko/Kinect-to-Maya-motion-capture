
# Originally based on the work of Rod Green (http://www.rodgreen.com/?p=191)
# and subsequently modified by Torben Sko (me@torbensko.com)
# 
# Please also note that my code is provided AS IS with NO WARRANTY OF ANY KIND, 
# INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A 
# PARTICULAR PURPOSE.



######################
#	imports
######################

import sys
import time
import threading
import maya.mel as mel
import maya.utils as utils
import maya.cmds as cmds
import string

######################
#	classes
######################

class TimerObj(threading.Thread):
	def __init__(self, runTime, command):
		self.runTime = runTime
		self.command = command
		self.doRun = True
		threading.Thread.__init__(self)
	def run(self):
		while(self.doRun):
			time.sleep(1.0/self.runTime)
			utils.executeDeferred(mel.eval, prepMelCommand(self.command)) #executeInMainThreadWithResult
	def restart(self):
		self.doRun = True
	def stop(self):
		self.doRun = False

######################
#	functions
######################
def prepMelCommand(commandString):
	return cmds.encodeString(commandString).replace("\\\"","\"")

timerObj = False

def startTimerObj(runTime, command):
	global timerObj
	if timerObj != False:
		timerObj.stop()
	timerObj = TimerObj(runTime, command)
	timerObj.start()

def stopTimerObj():
	global timerObj
	timerObj.stop()