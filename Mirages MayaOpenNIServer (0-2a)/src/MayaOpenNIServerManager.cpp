#include "locale.h"
#include <fstream>

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "MayaOpenNIServerManager.hpp"
#include "WindowGUI.hpp"
#include "MayaCapServer.hpp"

namespace MayaOpenNIServer{
	
	//----------------------------------
	// S_CaptureData
	//----------------------------------
	void S_CaptureData::clear( void ){

		for( int i = 0; i < MAX_SKEL_INDEX; ++i ){
			for( unsigned int j = 0; j < 3; ++j ){
				skelParams_[i].pos_[j] = 0.0f;
			}
			for( unsigned int j = 0; j < 9; ++j ){
				skelParams_[i].oriMat_[j] = 0.0f;
				skelParams_[i].inhOriMat_[j] = 0.0f;
			}
		}
	}
	
	//--------------------------------------------------------
	// S_Behavior
	//--------------------------------------------------------
	S_Behavior::S_Behavior():
		portNumber_( 9999 ),
		cmdPortNumber_( 9998 ),
		smoothing_( 0.1f ),
		handTrackingGestureId_( kGestureNone )
	{
		posOffset_[0] = 0.0;
		posOffset_[1] = 0.0;
		posOffset_[2] = 0.0;

		glWindowSizeX_ = 640;
		glWindowSizeY_ = 480;
		serverAddress_ = "localhost";
		for( unsigned int i = 0; i < kGestureNone; ++i ){
			useGesture_[i] = false;
		}

	}

	
	//--------------------------------------------------------
	// C_MayaOpenNIServerManager
	//--------------------------------------------------------
	C_MayaOpenNIServerManager::C_MayaOpenNIServerManager():
			requireRestartServer_( false ),
			requireStopServer_( false ),
			guiThreadId_( 0 ),
			serverThreadId_( 0 ),
			clientConnected_( false )
	{
		char* skeNames[] = {
			"SKEL_HEAD",
			"SKEL_NECK",
			"SKEL_TORSO",
			"SKEL_WAIST",

			"SKEL_LEFT_COLLAR",
			"SKEL_LEFT_SHOULDER",
			"SKEL_LEFT_ELBOW",
			"SKEL_LEFT_WRIST",
			"SKEL_LEFT_HAND",
			"SKEL_LEFT_FINGERTIP",

			"SKEL_RIGHT_COLLAR",
			"SKEL_RIGHT_SHOULDER",
			"SKEL_RIGHT_ELBOW",
			"SKEL_RIGHT_WRIST",
			"SKEL_RIGHT_HAND",
			"SKEL_RIGHT_FINGERTIP",

			"SKEL_LEFT_HIP",
			"SKEL_LEFT_KNEE",
			"SKEL_LEFT_ANKLE",
			"SKEL_LEFT_FOOT",

			"SKEL_RIGHT_HIP",
			"SKEL_RIGHT_KNEE",
			"SKEL_RIGHT_ANKLE",
			"SKEL_RIGHT_FOOT"
		};

		for( unsigned int i = 0; i < 24; ++i ){
			skelNames_.push_back( skeNames[i] );
		}

	}
	
	C_MayaOpenNIServerManager::~C_MayaOpenNIServerManager(){
		writeSetting();
	}
	
	C_MayaOpenNIServerManager*	C_MayaOpenNIServerManager::getManager( void ){
		static C_MayaOpenNIServerManager	manager;
		return &manager;
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::init( void ){
		getManager()->readSetting();
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::final( void ){

	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::updateCaptureData	(	void ){

		if( !xnManager_.isInit() ){
			captureData_.clear();
		}else{
			if ( !xnManager_.isPaused() ){
				// Read next available data
				xnManager_.getContext().WaitAndUpdateAll();

				if( xnManager_.getUserGenerator().GetNumberOfUsers() > 0 ){
					XnUserID	userIds[10];
					XnUInt16	nUsers = 10;
					xnManager_.getUserGenerator().GetUsers( userIds, nUsers );

					XnUserID userId = userIds[0];

					for( int i = 0; i < MAX_SKEL_INDEX; ++i ){
						XnSkeletonJointTransformation	jointTrs;
						xnManager_.getUserGenerator().GetSkeletonCap().GetSkeletonJoint( userId, (XnSkeletonJoint)(i + 1), jointTrs );

						captureData_.skelParams_[i].oriPos_[0] = jointTrs.position.position.X;
						captureData_.skelParams_[i].oriPos_[1] = jointTrs.position.position.Y;
						captureData_.skelParams_[i].oriPos_[2] = jointTrs.position.position.Z;
						
						captureData_.skelParams_[i].pos_[0] = jointTrs.position.position.X + static_cast< float >( getBehavior().posOffset_[0] );
						captureData_.skelParams_[i].pos_[1] = jointTrs.position.position.Y + static_cast< float >( getBehavior().posOffset_[1] );
						captureData_.skelParams_[i].pos_[2] = jointTrs.position.position.Z + static_cast< float >( getBehavior().posOffset_[2] );

						for( int j = 0; j < 9; ++j ){
							captureData_.skelParams_[i].oriMat_[j] = jointTrs.orientation.orientation.elements[j];
							captureData_.skelParams_[i].inhOriMat_[j] = jointTrs.orientation.orientation.elements[j];
						}
					}

					computeInhelitOrientation();
				}
			}
		}
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::computeInhelitOrientation	(	void ){

		C_Matrix33	matCenter( captureData_.skelParams_[ XN_SKEL_TORSO - 1 ].oriMat_ );
		matCenter.inverse();
		
		C_Matrix33	matLeftShl( captureData_.skelParams_[ XN_SKEL_LEFT_SHOULDER - 1 ].oriMat_ );
		
		matLeftShl = matLeftShl * matCenter;
		matLeftShl.getAsArray( captureData_.skelParams_[ XN_SKEL_LEFT_SHOULDER - 1 ].inhOriMat_ );
		
		C_Matrix33	matRightShl( captureData_.skelParams_[ XN_SKEL_RIGHT_SHOULDER - 1 ].oriMat_ );
		matRightShl = matRightShl * matCenter;
		matRightShl.getAsArray( captureData_.skelParams_[ XN_SKEL_RIGHT_SHOULDER - 1 ].inhOriMat_ );
		
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	const S_CaptureData& C_MayaOpenNIServerManager::getCurrentCaptureData	(	void ) const{
		return captureData_;
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	S_CaptureData& C_MayaOpenNIServerManager::getCurrentCaptureData	(	void ){
		return captureData_;
	}

	//- - - - - - - - - - - - - - - - - -
	//
	const S_Behavior& C_MayaOpenNIServerManager::getBehavior		(	void ) const{
		return behavior_;
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	S_Behavior& C_MayaOpenNIServerManager::getBehavior		(	void ){
		return behavior_;
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::waitForThredsEnd(	void ){

		HANDLE	handles[10];
		handles[0] = reinterpret_cast<HANDLE>( guiThreadId_ );
		handles[1] = reinterpret_cast<HANDLE>( serverThreadId_ );

		WaitForMultipleObjects( 2, handles, TRUE, INFINITE );

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::killThreads(	void ){

		xnManager_.getContext().Shutdown();
		exit(0);

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::startGUIThread		(	void ){
		setGUIThreadID( _beginthreadex( 0, 0, UI::makeUI, 0, 0, 0 ) );
		
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::startServerThread	(	void ){
		setStopServer( false );
		setRestartServer( false );
		setClientConnected( false );
		
		setServerThreadID( _beginthreadex( 0, 0, MayaCapServer::startMocServer, 0, 0, 0 ) );
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::stopServer	(	void ){
		setStopServer( true );
		WaitForSingleObject( (HANDLE)serverThreadId_, INFINITE );
		setStopServer( false );
		serverThreadId_ = 0;
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::startUserTracker	(	void ){
		xnManager_.initUserTracer();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::killGUIThread		(	void ){
		if( guiThreadId_ ){
			_endthreadex( guiThreadId_ );
			guiThreadId_ = 0;
		}
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::killServerThread	(	void ){
		if( serverThreadId_ ){
			_endthreadex( serverThreadId_ );
			serverThreadId_ = 0;
		}
	}

	//- - - - - - - - - - - - - - - - - -
	//
	bool C_MayaOpenNIServerManager::isGUIStarted		(	void ){
		if( guiThreadId_ ){
			DWORD	code;
			GetExitCodeThread( (HANDLE)guiThreadId_, &code );
			if( code == STILL_ACTIVE ){
				return true;
			}
		}

		return false;

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	bool C_MayaOpenNIServerManager::isServerStarted		(	void ){
		if( serverThreadId_ ){
			DWORD	code;
			GetExitCodeThread( (HANDLE)serverThreadId_, &code );
			if( code == STILL_ACTIVE ){
				return true;
			}
			
		}
		
		return false;
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::writeSetting( void ){
		
		char	buff[2000];
		
		WritePrivateProfileString( "Setting", "Host", behavior_.serverAddress_.c_str() , "./setting.ini" );
		
		_itoa_s( behavior_.portNumber_, buff, 10, 10 );
		WritePrivateProfileString( "Setting", "Port", buff, "./setting.ini" );
		_itoa_s( behavior_.cmdPortNumber_, buff, 10, 10 );
		WritePrivateProfileString( "Setting", "CmdPort", buff, "./setting.ini" );
		_itoa_s( behavior_.handTrackingGestureId_, buff, 10, 10 );
		WritePrivateProfileString( "Setting", "TrackingGesture", buff, "./setting.ini" );

		_gcvt_s( buff, 100, behavior_.smoothing_, 7 );
		WritePrivateProfileString( "Setting", "Smoothing", buff , "./setting.ini" );
		
		_gcvt_s( buff, 100, behavior_.posOffset_[0], 7 );
		WritePrivateProfileString( "Setting", "OffsetX", buff , "./setting.ini" );
		_gcvt_s( buff, 100, behavior_.posOffset_[1], 7 );
		WritePrivateProfileString( "Setting", "OffsetY", buff , "./setting.ini" );
		_gcvt_s( buff, 100, behavior_.posOffset_[2], 7 );
		WritePrivateProfileString( "Setting", "OffsetZ", buff , "./setting.ini" );

		for( unsigned int i = 0 ; i < 6; ++i ){
			_itoa_s( behavior_.useGesture_[i], buff, 10, 10 );
			char gesStr[] = "UseGesture0";
			gesStr[ 10 ] = '0' + i;
			WritePrivateProfileString( "Setting", gesStr, buff, "./setting.ini" );
		}
		
		writeMelCommand( std::string( "./GestureCommand_Wave.mel" ), behavior_.gestureCommands_[0] );
		writeMelCommand( std::string( "./GestureCommand_Click.mel" ), behavior_.gestureCommands_[1] );
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::writeMelCommand( const std::string&	filePath,
													const std::string&	cmdStr )
	{
		std::ofstream	ofs;
		ofs.open( filePath.c_str(), std::ios::out );

		if( !ofs.is_open() ){
			return;
		}

		ofs << cmdStr;
		ofs.close();

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::readSetting( void ){

		char	buff[2000];
		if( GetPrivateProfileString( "Setting", "Host", "", buff, 2000, "./setting.ini" ) > 0 ){
			behavior_.serverAddress_ = buff;
		}
		
		if( GetPrivateProfileString( "Setting", "Port", "", buff, 2000, "./setting.ini" ) > 0 ){
			behavior_.portNumber_ = atoi( buff );
		}

		if( GetPrivateProfileString( "Setting", "CmdPort", "", buff, 2000, "./setting.ini" ) > 0 ){
			behavior_.cmdPortNumber_ = atoi( buff );
		}

		if( GetPrivateProfileString( "Setting", "TrackingGesture", "", buff, 2000, "./setting.ini" ) > 0 ){

			int id = static_cast< E_GestureId >( atoi( buff ) );
			if( id < static_cast< int >( kGestureNone ) ){
				behavior_.handTrackingGestureId_ = static_cast< E_GestureId >( id );
			}
		}
		
		if( GetPrivateProfileString( "Setting", "Smoothing", "", buff, 2000, "./setting.ini" ) > 0 ){
			behavior_.smoothing_ = static_cast<float>( atof( buff ) );
		}
		
		if( GetPrivateProfileString( "Setting", "OffsetX", "", buff, 2000, "./setting.ini" ) > 0 ){
			behavior_.posOffset_[0] = atof( buff );
		}
		
		if( GetPrivateProfileString( "Setting", "OffsetY", "", buff, 2000, "./setting.ini" ) > 0 ){
			behavior_.posOffset_[1] = atof( buff );
		}
		
		if( GetPrivateProfileString( "Setting", "OffsetZ", "", buff, 2000, "./setting.ini" ) > 0 ){
			behavior_.posOffset_[2] = atof( buff );
		}
		
		for( unsigned int i = 0 ; i < 6; ++i ){
			char gesStr[] = "UseGesture0";
			gesStr[ 10 ] = '0' + i;
			if( GetPrivateProfileString( "Setting", gesStr, "", buff, 2000, "./setting.ini" ) > 0 ){
				behavior_.useGesture_[i] = atoi( buff ) == 0 ? false: true;
			}
		}

		readMelCommand( &behavior_.gestureCommands_[0], std::string( "./GestureCommand_Wave.mel" ) );
		readMelCommand( &behavior_.gestureCommands_[1], std::string( "./GestureCommand_Click.mel" ) );
		
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::readMelCommand( std::string*		retCmdStr,
													const std::string&	filePath )
	{
		std::ifstream	ifs;
		ifs.open( filePath.c_str(), std::ios::in );

		if( !ifs.is_open() ){
			return;
		}

		while( !ifs.eof() ){
			retCmdStr->push_back( ifs.get() );
		}
		retCmdStr->erase( retCmdStr->size() - 1, 1 );	//remove LF

		ifs.close();

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::callGestureCmd( E_GestureId gestureId ){

		if( gestureId == kGestureNone ){
			return;
		}
		
		if( !behavior_.useGesture_[ gestureId ] ){
			return;
		}

		if( behavior_.gestureCommands_[ gestureId ].size() <= 0 ){
			return;
		}
		

		MayaCapServer::C_MayaCommandClient	client;
		try{
			client.open();
			client.sendCommand( behavior_.gestureCommands_[ gestureId ] );
		}catch( const std::exception& e ){
			printf( "%s\n", e.what() );
			return;
		}
	}

}
