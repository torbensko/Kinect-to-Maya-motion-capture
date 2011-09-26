MayaOpenNIServer
Preview Alpha 1.1

***********About this program***********
Motion capture server for Maya with OpeNI/Kinect.

***********Requirement***********
+WindowsXP or laterÅ@32bit(64bit did not tested by my self, but some user reported work fine)
+Kinect hardware
+OpenNI
+NITE
http://www.openni.org/
+Kinect driver
https://github.com/avin2/SensorKinect

***********Forders***********
-bin
Program and DLL, and setting files

-MayaScript
Test script for maya

-src
source

***********How to use***********
1. Server setup
Run MayaOpenNIServer.exe
If you can see captured depthmap in MayaOpenNIServer GUI, Push "Start server" button after do calibration.
Note1: Open firewall for MayaOpenNIServer.exe or prot number 9999( default ).
Note2: Please refer OpenNI Documents how to calibrate your pose.

2. Maya
Run mel command:
defineDataServer -d KinectCapture -s "localhost:9999";
then you can connect server.

or you can use SampleScript.
Run mayaKinectCaptureTest, push "Make Nulls", then push "Connect";

confirm you can see  "KinectCapture" in 
Window->Animation Editors->Device Editor.


***********Libraly information***********
ÅEOpenNI
ÅEFLTK2.0

***********Licensec***********
Under LGPL3.0 in this release.


***********Author***********
Yasutoshi Mori
Twitter: @yasutoshi_mori
MirageWrks@gmail.com


