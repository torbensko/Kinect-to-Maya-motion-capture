
# This code is provided under a Creative Commons Attribution license 
# http://creativecommons.org/licenses/by/3.0/
# As such you are free to use the code for any purpose as long as you remember 
# to mention our names (Torben Sko and Beau Vardos) at some point.
# 
# Please also note that my code is provided AS IS with NO WARRANTY OF ANY KIND, 
# INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A 
# PARTICULAR PURPOSE.


import maya.cmds as cmds

#	"SKEL_HEAD",
#	"SKEL_NECK",
#	"SKEL_TORSO",				// "SKEL_WAIST", "SKEL_LEFT_COLLAR",
#	
#	"SKEL_LEFT_SHOULDER",
#	"SKEL_LEFT_ELBOW",			// "SKEL_LEFT_WRIST",
#	"SKEL_LEFT_HAND",			// "SKEL_LEFT_FINGERTIP", "SKEL_RIGHT_COLLAR",
#	
#	"SKEL_RIGHT_SHOULDER",
#	"SKEL_RIGHT_ELBOW",			// "SKEL_RIGHT_WRIST",
#	"SKEL_RIGHT_HAND",			// "SKEL_RIGHT_FINGERTIP",
#
#	"SKEL_LEFT_HIP",
#	"SKEL_LEFT_KNEE",			// "SKEL_LEFT_ANKLE",
#	"SKEL_LEFT_FOOT",
#
#	"SKEL_RIGHT_HIP",
#	"SKEL_RIGHT_KNEE",			// "SKEL_RIGHT_ANKLE",
#	"SKEL_RIGHT_FOOT"

def joint( side, lowerJoint, upperJoint, useSphere = 0, sharedUpper = 0, sharedLower = 0, show = 1, heightScale = 1 ):
	name = lowerJoint + "_" + upperJoint
	
	upperName = 'SKEL_'
	if sharedUpper == 0:
		upperName += side + "_"
	upperName += upperJoint
	
	lowerName = 'SKEL_'
	if sharedLower == 0:
		lowerName += side + "_"
	lowerName += lowerJoint

	print name
	cmds.spaceLocator( name = '%s_%s' % (side,name) )
	cmds.pointConstraint( lowerName, '%s_%s' % (side,name) )
	cmds.pointConstraint( upperName, '%s_%s' % (side,name) )
	cmds.aimConstraint( upperName, '%s_%s' % (side,name) )
	if useSphere:
		cmds.sphere( name = '%s_%s_C' % (side,name), radius=1 )
	else:
		cmds.cylinder( name = '%s_%s_C' % (side,name), radius=0.5, heightRatio = 6 * heightScale )
	
	cmds.setAttr( '%s_%s_C.doubleSided' % (side,name), 0 )
	if show == 0:
		cmds.setAttr( '%s_%s_C.primaryVisibility' % (side,name), 0 )
		
	#cmds.rotate( 0, 0, 90, '%s_FOREARM_C' % (side) )
	#cmds.makeIdentity( '%s_FOREARM_C' % (side), apply = 1, rotate = 1 )
	cmds.select('%s_%s' % (side,name), '%s_%s_C' % (side,name))	
	cmds.parentConstraint()
	return

def rigCharacter(show = 1):	
	sides = [ 'LEFT', 'RIGHT' ]
	locators = [ 'SKEL_HEAD', 'SKEL_NECK', 'SKEL_TORSO', 'SKEL_LEFT_SHOULDER', 'SKEL_LEFT_ELBOW', 'SKEL_LEFT_HAND', 'SKEL_RIGHT_SHOULDER', 'SKEL_RIGHT_ELBOW', 'SKEL_RIGHT_HAND', 'SKEL_LEFT_HIP', 'SKEL_LEFT_KNEE', 'SKEL_LEFT_FOOT', 'SKEL_RIGHT_HIP', 'SKEL_RIGHT_KNEE', 'SKEL_RIGHT_FOOT' ]


	# select all locators
	cmds.select( locators )
	print locators[0:5]
	
	for i in locators:
		#i = str(i[0:-5])
		translate = cmds.getAttr('%s.translate' % (i) )
		rotate = cmds.getAttr('%s.rotate' % (i) )
		print '...%s: ' % (i)
		for j in translate:
			print 'translate: ', j
			cmds.select( d=True )
			#cmds.joint( name = '%s_JOINT' % (i), p=j )
			cmds.sphere( name = '%s_JOINT' % (i), radius=0.75 )
			if show == 0:
				cmds.setAttr( '%s_JOINT.primaryVisibility' % (i), 0 )
			#if hide > 0:
			#	cmds.setAttr( '%s_JOINT' % (i), 0 )
			print i
			cmds.select('%s' % (i), '%s_JOINT' % (i))	
			cmds.parentConstraint() #pointConstraint
	
	for i in sides:
		joint(i, 'HAND', 'ELBOW', show = show)
		joint(i, 'ELBOW', 'SHOULDER', show = show)
		joint(i, 'FOOT', 'KNEE', heightScale = 1.7, show = show)
		joint(i, 'KNEE', 'HIP', heightScale = 1.7, show = show)
		joint(i, 'HIP', 'TORSO', sharedUpper = 1, show = show)
	
		cmds.scale( 1.2, 1.2, 1.2, 'SKEL_%s_SHOULDER_JOINT' % (i) )
		cmds.scale( 1.2, 1.2, 1.2, 'SKEL_%s_HIP_JOINT' % (i) )
		cmds.scale( 1.2, 1.2, 1.2, 'SKEL_%s_HAND_JOINT' % (i) )
		cmds.scale( 1.5, 1.5, 1.5, 'SKEL_%s_FOOT_JOINT' % (i) )
	
	joint('MIDDLE', 'TORSO', 'NECK', sharedUpper=1, sharedLower=1, show = show)
	joint('MIDDLE', 'LEFT_HIP', 'RIGHT_HIP', sharedUpper=1, sharedLower=1, show = show)
	
	cmds.scale( 1.8, 1.8, 1.8, 'SKEL_HEAD_JOINT' )
	cmds.scale( 1.5, 1.5, 1.5, 'SKEL_NECK_JOINT' )
	cmds.scale( 1.1, 1.1, 1.1, 'SKEL_TORSO_JOINT' )
	
	return
