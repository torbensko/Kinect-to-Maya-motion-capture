#if !defined( __MAYAOPENNISERVERMANAGER_HPP_INCLUDED__ )
#define __MAYAOPENNISERVERMANAGER_HPP_INCLUDED__

#include <string>
#include <vector>
#include <process.h>
#include <Windows.h>

#include "XnManager.hpp"
#include "Matrix33.hpp"

namespace MayaOpenNIServer{

	static const int			MAX_SKEL_INDEX = 24;

	enum E_GestureId{
		kGestureWave = 0,
		kGestureClick,
		kGesture2,
		kGesture3,
		kGesture4,
		kGesture5,
		kGesture6,
		kGesture7,
		kGesture8,
		kGesture9,
		kGestureNone,
	};

	static const char* GESTURE_NAMES[10] = {
		"Wave",
		"Click",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		""
	};
	
	//--------------------------------------------------------
	// S_CaptureData
	//
	//--------------------------------------------------------
	struct	S_CaptureData{
		//Members
		struct S_SkelParams{
			float	oriPos_[3];
			float	pos_[3];
			float	oriMat_[9];
			float	inhOriMat_[9];
		};

		S_SkelParams	skelParams_[MAX_SKEL_INDEX];

		float	handPosition_[3];
		
	 public:
		//Manipulators
		void	clear();

	};

	//--------------------------------------------------------
	// S_Behavior
	//
	//--------------------------------------------------------
	struct	S_Behavior{
	 public:
		//Creators
		S_Behavior();
		
	 public:
		//Members
		int									portNumber_;
		int									cmdPortNumber_;
		std::string							serverAddress_;
		std::string							gestureCommands_[10];
		bool								useGesture_[ kGestureNone ];
		E_GestureId							handTrackingGestureId_;
		float								smoothing_;
		
		int			glWindowSizeX_;
		int			glWindowSizeY_;

		double		posOffset_[3];

	};
	//--------------------------------------------------------
	// C_MayaOpenNIServerManager
	//
	//--------------------------------------------------------
	class C_MayaOpenNIServerManager{
	 public:
		//Typedefs
		
	 public:
		//Creators
		virtual ~C_MayaOpenNIServerManager();

	 private:
		C_MayaOpenNIServerManager();

	 public:
		//Static Manipulators
		static C_MayaOpenNIServerManager*	getManager			(	void );
		
		static void							init				(	void );
		static void							final				(	void );

	 public:
		//Manipulators
		inline const std::string&					getSkeltonName		(	int	id ) const;
		inline const std::vector< std::string >&	getSkeltonNames		(	void ) const;
		
		void								updateCaptureData			(	void );
		void								computeInhelitOrientation	(	void );
		const S_CaptureData&				getCurrentCaptureData		(	void ) const;
		S_CaptureData&						getCurrentCaptureData		(	void );

		const S_Behavior&					getBehavior					(	void ) const;
		S_Behavior&							getBehavior					(	void );

		void								startGUIThread			(	void );
		void								startServerThread		(	void );
		void								stopServer				(	void );
		void								startUserTracker		(	void );
		
		inline uintptr_t					getGUIThreadID			(	void ) const;
		inline uintptr_t					getServerThreadID		(	void ) const;
		inline void							setGUIThreadID			(	uintptr_t	id );
		inline void							setServerThreadID		(	uintptr_t	id );
		inline void							waitForGUIThreadEnd		(	void );
		void								waitForThredsEnd		(	void );
		void								killGUIThread			(	void );
		void								killServerThread		(	void );
		void								killThreads				(	void );

		bool								isGUIStarted			(	void );
		bool								isServerStarted			(	void );

		inline bool							setClientConnected		(	bool flag );
		inline bool							isClientConnected		(	void );

		inline bool							isRequireRestartServer	(	void );
		inline void							setRestartServer		(	bool	flag );
		
		inline bool							isRequireStopServer		(	void );
		inline void							setStopServer			(	bool	flag );


		inline C_XnManager&					getXnManager			(	void );
		inline const C_XnManager&			getXnManager			(	void ) const;

		inline unsigned int					getGestureFlag			(	void ) const;
		inline void							setGestureFlag			(	unsigned int );
		inline void							orGestureFlag			(	unsigned int );
		inline void							xorGestureFlag			(	unsigned int );

		void								writeSetting			(	void );
		void								writeMelCommand			(	const std::string&	filePath,
																		const std::string&	cmdStr );
		
		void								readSetting				(	void );
		void								readMelCommand			(	std::string*		retCmdStr,
																		const std::string&	filePath );

		void								callGestureCmd			(	E_GestureId	gestureId );

	 public:
		//Typedefs
		static const unsigned int		GESTURE_WAVE = 1;
		static const unsigned int		GESTURE_CLICK = 1 << 1;
		static const unsigned int		GESTURE_NONE = 1 << 2;


	 protected:
		//Members
		C_XnManager							xnManager_;
		
		S_CaptureData						captureData_;
		S_Behavior							behavior_;
		uintptr_t							guiThreadId_;
		uintptr_t							serverThreadId_;
		
		bool								requireRestartServer_;
		bool								requireStopServer_;
		bool								clientConnected_;
		
		std::vector< std::string >			skelNames_;
		

		unsigned int						gestureBit_;

	};
}

#include "MayaOpenNIServerManager.inl"

#endif
