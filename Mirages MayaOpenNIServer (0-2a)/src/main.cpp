/*
  Maya OpenNI Server
  
  Copyright (C) 2010 Yasutoshi Mori (Mirage)
  $Id: main.cpp,v 1.5 2010/12/30 20:51:52 Mirage Exp $
*/


//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "XnManager.hpp"
#include "MayaOpenNIServerManager.hpp"
#include "WindowGUI.hpp"
#include "MayaCapServer.hpp"


void	exitHandle( void );
int		main( int, char** );


//---------------------------------------------------------------------------
// main
//---------------------------------------------------------------------------
int main( int, char** )
{
	using namespace MayaOpenNIServer;

	atexit( exitHandle );
	
	//Maya MocServer Setup
	try{
		C_MayaOpenNIServerManager::init();

		//UI
		C_MayaOpenNIServerManager::getManager()->startGUIThread();

		C_MayaOpenNIServerManager::getManager()->startUserTracker();

		C_MayaOpenNIServerManager::getManager()->waitForGUIThreadEnd();
		
	}catch( std::exception& e ){
		MessageBox( NULL, e.what(), "MayaOpenNIServer Error", MB_OK | MB_SETFOREGROUND );
		exit( -1 );
	}

}

void	exitHandle( void ){
	
	using namespace MayaOpenNIServer;
	
	C_MayaOpenNIServerManager::final();
	
}
