#ifndef __MAYACAPSERVER_HPP_INCLUDED__
#define __MAYACAPSERVER_HPP_INCLUDED__

#include <string>
#include <vector>

#include <mocapserver.h>
#include <mocapserial.h>
#include <XnOpenNI.h>

namespace MayaCapServer{
	
	//----------------------------------
	// startMocServer
	//----------------------------------
	unsigned int __stdcall	startMocServer	( void* arg );
	
	//----------------------------------
	// C_MayaCapServer
	//----------------------------------
	class C_MayaCapServer{
	 public:
		//Inner class
		struct CapChanVec{
			CapChannel	pos;
			CapChannel	ori;
		};

	 public:
		//Creators
		C_MayaCapServer	();

	 public:
		//Manipulators
		int				invoke					(	void );
		void			setProgramName			(	const std::string&	programName );		//Ex. MayaOpenNIServer
		void			setServerAddress		(	const std::string&	serverAddress );	//Ex. localhost:9999

		int				clientLoop				(	int					client_fd );
		int				createChannels			(	int					client_fd );
		void			sendDatas				(	int					client_fd );
		void			readAndWriteSkelton		(	char				skelId );

	 protected:
		std::string					programName_;
		std::string					serverAddress_;

		CapChanVec					capChanSkelPostions_[ XN_SKEL_RIGHT_FOOT ];
		CapChannel					capChanSwitchWave_;
		CapChannel					capChanHand_;
		
		bool						isChannelCreated_;

	};

	//----------------------------------
	// C_MayaCommandClient
	//----------------------------------
	class C_MayaCommandClient{
	 public:
		//Creators
		C_MayaCommandClient();
		virtual ~C_MayaCommandClient();
		
	 public:
		//Manipulators
		void			open					(	void );
		void			sendCommand				(	const std::string&	cmd );

	 protected:
		//Members
		std::string				programName_;
		int						openId_;
	};
	
	
}

#endif
