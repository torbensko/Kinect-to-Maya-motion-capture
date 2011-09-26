
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <errno.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>
#include <io.h>

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "MayaCapServer.hpp"
#include "MayaOpenNIServerManager.hpp"

namespace MayaCapServer{

	using namespace MayaOpenNIServer;

	//----------------------------------
	// startMocServer
	//----------------------------------
	unsigned int __stdcall	startMocServer	(	void* arg ){
		
		C_MayaCapServer	server;

		server.setProgramName( "MayaOpenNIServer.exe" );
		server.invoke();
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setClientConnected( false );
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setRestartServer( false );
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setStopServer( false );

		return 0;
	}
	
	//----------------------------------
	// C_MayaCapServer
	//----------------------------------

	//- - - - - - - - - - - - - - - - - - - -
	//
	C_MayaCapServer::C_MayaCapServer():
		isChannelCreated_( false )
	{
		
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	int C_MayaCapServer::invoke( void ){

		int clientFd = 0;
		int status;
		
		while (1){

			std::string strServerAddr = MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().serverAddress_;
			char buf[10];
			_itoa_s( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().portNumber_, buf, 10, 10 );
			strServerAddr = strServerAddr + ":" +  buf;
			
			setServerAddress( strServerAddr );
			
			printf( "%s\n", strServerAddr.c_str() );

			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setRestartServer( false );
			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setStopServer( false );
			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setClientConnected( false );

			clientFd = CapServe( const_cast< char* >( serverAddress_.c_str() ) );
			if ( clientFd < 0 ){
				CapError( -1, CAP_SEV_FATAL, const_cast<char*>( programName_.c_str() ), NULL );
				break;
			}

			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setClientConnected( true );
			printf( "connected\n" );

			status = clientLoop( clientFd );

			if ( status < 0 ){
				CapError( -1, CAP_SEV_FATAL, const_cast< char* >( programName_.c_str() ), NULL );
			}

			closesocket( clientFd );
			clientFd = -1;

			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->setClientConnected( false );
			if( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->isRequireStopServer() ){
				break;
			}

		}

		return 0;

	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaCapServer::setProgramName( const std::string& programName ){
		programName_ = programName;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaCapServer::setServerAddress( const std::string& serverAddress ){
		serverAddress_ = serverAddress;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaCapServer::sendDatas( int clientFd ){
		for( unsigned int i = 0; i < 24; ++i ){
			readAndWriteSkelton( i );
		}

		CapSetData( capChanHand_, C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().handPosition_ );

#if 0
		float val;
		if( C_MayaOpenNIServerManager::GESTURE_WAVE & C_MayaOpenNIServerManager::getManager()->getGestureFlag() ){
			C_MayaOpenNIServerManager::getManager()->xorGestureFlag( C_MayaOpenNIServerManager::GESTURE_WAVE );
			val = 1.0f;
		}else{
			val = 0.0f;
		}
		
		CapSetData( capChanSwitchWave_, &val );
#endif
		
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaCapServer::readAndWriteSkelton( char skelId ){
	
		using namespace MayaOpenNIServer;

		const S_CaptureData&	capData = C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData();
		
		CapSetData( capChanSkelPostions_[skelId].pos, (void*)capData.skelParams_[skelId].pos_ );

		CapMatrix3	capMat;
		int eCnt = 0;
		
		capMat[0][0] = capData.skelParams_[skelId].oriMat_[0];
		capMat[1][0] = capData.skelParams_[skelId].oriMat_[1];
		capMat[2][0] = capData.skelParams_[skelId].oriMat_[2];
		         
		capMat[0][1] = capData.skelParams_[skelId].oriMat_[3];
		capMat[1][1] = capData.skelParams_[skelId].oriMat_[4];
		capMat[2][1] = capData.skelParams_[skelId].oriMat_[5];
		         
		capMat[0][2] = capData.skelParams_[skelId].oriMat_[6];
		capMat[1][2] = capData.skelParams_[skelId].oriMat_[7];
		capMat[2][2] = capData.skelParams_[skelId].oriMat_[8];
		
		CapQuaternion	capQu;
		CapMatrix32Quat( capMat, capQu );
		CapSetData( capChanSkelPostions_[skelId].ori, capQu );

	}
	

	//- - - - - - - - - - - - - - - - - - - -
	//
	int C_MayaCapServer::createChannels( int clientFd ){

		using namespace MayaOpenNIServer;
		
		for( unsigned int i = 0; i < C_MayaOpenNIServerManager::getManager()->getSkeltonNames().size(); ++i ){

			std::string	chanNamePos = C_MayaOpenNIServerManager::getManager()->getSkeltonName(i) + "_POS";
			std::string	chanNameOri = C_MayaOpenNIServerManager::getManager()->getSkeltonName(i) + "_ORI";
			
			capChanSkelPostions_[i].pos = CapCreateChannel( const_cast<char*>( chanNamePos.c_str() ), CAP_USAGE_POSITION, 3 );
			capChanSkelPostions_[i].ori = CapCreateChannel( const_cast<char*>( chanNameOri.c_str() ), CAP_USAGE_ORIENTATION, 4 );

		}
		
		capChanHand_ = CapCreateChannel( "HAND_POS", CAP_USAGE_POSITION, 3 );
		
#if 0
		capChanSwitchWave_ = CapCreateChannel( "GESTURE_WAVE", CAP_USAGE_UNKNOWN, 1 );
#endif
		return 0;
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	int C_MayaCapServer::clientLoop( int clientFd ){
		
		int status;
		CapCommand cmd;
		
		char ruser[64];
		char rhost[64];
		char realhost[64];
		
		fd_set rd_fds;
		struct timeval base_timeout, timeout;
		int recording = 0;

		base_timeout.tv_sec  = 1;
		base_timeout.tv_usec = 0;

		while (1){
			
			FD_ZERO( &rd_fds );
			FD_SET((unsigned int)clientFd, &rd_fds);
			timeout.tv_sec  = base_timeout.tv_sec;
			timeout.tv_usec = base_timeout.tv_usec;

			if( C_MayaOpenNIServerManager::getManager()->isRequireRestartServer() || C_MayaOpenNIServerManager::getManager()->isRequireStopServer() ){
				return 0;
			}
			
			status = select( FD_SETSIZE, &rd_fds, NULL, NULL, &timeout );
			if ( status < 0 ){
				
				CapError( clientFd, CAP_SEV_FATAL, const_cast< char* >( programName_.c_str() ), "select failed" );
				CapError( clientFd, CAP_SEV_FATAL, "select", NULL );
				return -1;
				
			}else if ( status == 0 ){
				if ( recording ){
					sendDatas( clientFd );
					continue;
				}else{
					return 0;
				}
			}else{
				cmd = CapGetCommand( clientFd );
				
				switch ( cmd ){
				 case CAP_CMD_QUIT:
					printf( "CMD_QUIT\n" );
					return 0;

				 case CAP_CMD_CLOSE:
					printf( "CMD_CLOSE\n" );
					return 0;
					
				 case CAP_CMD_AUTHORIZE:
					printf( "CMD_AUT\n" );
					status = CapGetAuthInfo(clientFd, ruser, rhost, realhost);
					if (status < 0){
						return -1;
					}
					status = CapAuthorize( clientFd, 1 );
					break;

				 case CAP_CMD_INIT:
					printf( "CMD_INIT\n" );
					status = CapInitialize( clientFd, const_cast< char* >( programName_.c_str() ) );
					break;

				 case CAP_CMD_VERSION:
					printf( "CMD_VER\n" );
					status = CapVersion( clientFd, const_cast< char* >( programName_.c_str() ), "1.0", "Kinect OpenNI capture server - v0.0" );
					break;

				 case CAP_CMD_INFO:
					printf( "CMD_INFO\n" );
					if ( !isChannelCreated_ ){
						status = createChannels( clientFd );
						if (status < 0){
							break;
						}
						isChannelCreated_ = true;
					}
					status = CapInfo( clientFd, 0.0, 0.0, 0.0, 512 * 1024, 1 );
					break;

				 case CAP_CMD_DATA:
					sendDatas( clientFd );
					status = CapData( clientFd );
					break;

				 case CAP_CMD_START_RECORD:
					printf( "START_RECORD\n" );
					recording = 1;
					break;

				 case CAP_CMD_STOP_RECORD:
					printf( "STOP_RECORD\n" );
					recording = 0;
					break;
					
				 default:
					status = CapError( clientFd, CAP_SEV_ERROR, const_cast< char* >( programName_.c_str() ), "Unknown server command." );
					break;
				}

				if (status < 0){
					return -1;
				}
			}
		}

		return 0;
	}

	//----------------------------------
	// C_MayaCommandClient
	//----------------------------------
	C_MayaCommandClient::C_MayaCommandClient( ):
		openId_( -1 )
	{
	}
	
	C_MayaCommandClient::~C_MayaCommandClient( ){
		if( openId_ >= 0 ){
			closesocket( openId_ );
		}

		WSACleanup();
	}
	
	void C_MayaCommandClient::open( void ){


		WSADATA	wsaData;
		WSAStartup( MAKEWORD( 2, 0 ), &wsaData );

		openId_ = socket( AF_INET, SOCK_STREAM, 0 );
		if( openId_ == INVALID_SOCKET ){
			throw std::exception( "socket Open Error" );
		}
		
		struct sockaddr_in	addr;
		addr.sin_family = AF_INET;
		
		LPSERVENT serv = getservbyname( "mayaCommand", "tcp" );
		if( serv == NULL ){
			addr.sin_port = htons( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().cmdPortNumber_ );
		}else{
			addr.sin_port = htons( serv->s_port );
		}

		std::string address = MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().serverAddress_;
		
		addr.sin_addr.s_addr = inet_addr( address.c_str() );
		if( addr.sin_addr.s_addr == 0xffffffff ){
			hostent*	host;
			host = gethostbyname( address.c_str() );
			if( host == NULL ){
				throw std::exception( "Invalid address" );
			}
			addr.sin_addr.s_addr = *( unsigned int* )host->h_addr_list[0];
		}

		if( connect( openId_, (SOCKADDR*)&addr, sizeof( addr ) ) == SOCKET_ERROR ){
			throw std::exception( "Commandport bind Error" );
		}

	}

	void C_MayaCommandClient::sendCommand( const std::string&	cmd ){

		if( openId_ < 0 ){
			printf( "openID:%d", openId_ );
			return;
		}
		
		send( openId_, cmd.c_str(), cmd.size(), 0 );

		char reply[5000];
		int red = recv( openId_, reply, 4096, 0 );

		if( red > 0 ){
			printf( "%s\n", reply );
		}
	}
	
}

