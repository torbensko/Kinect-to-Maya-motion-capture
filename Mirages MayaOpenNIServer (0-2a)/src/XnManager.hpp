#if !defined( __XNMANAGER_INCLUDED__ )
#define __XNMANAGER_INCLUDED__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>


namespace MayaOpenNIServer{

	//--------------------------------------------------------
	// C_XnManager
	//
	//--------------------------------------------------------
	class C_XnManager{
	 public:
		//Creators
		C_XnManager();
		~C_XnManager();
		
	 public:
		//Manipulators
		void						initUserTracer	(	void );
		
		inline bool					isInit				(	void ) const{ return isInit_; };
		inline bool					isNeedPose			(	void ) const{ return isNeedPose_; };
		inline bool					isPaused			(	void ) const{ return isPaused_; };

		inline xn::Context&				getContext			(	void ) { return context_; };
		inline xn::DepthGenerator&		getDepthGenerator	(	void ) { return depthGenerator_; };
		inline xn::UserGenerator&		getUserGenerator	(	void ) { return userGenerator_; };
		inline xn::GestureGenerator&	getGesrureGenerator	(	void ) { return gestureGenerator_; };
		inline xn::HandsGenerator&		getHandsGenerator	(	void ) { return handsGenerator_; };

	 public:
		//Callbacks
		static void	__stdcall onNewUser		(	xn::UserGenerator&				generator,
													XnUserID					userId,
													void*						pThis );
		
		static void	__stdcall onLostUser	(	xn::UserGenerator&				generator,
												XnUserID						userId,
												void*							pThis );

		static void	__stdcall onPoseDetect	(	xn::PoseDetectionCapability&	cap,
												const XnChar*					strPos,
												XnUserID						userId,
												void*							pThis );
		
		static void	__stdcall onCalibStart	(	xn::SkeletonCapability&			cap,
												XnUserID						userId,
												void*							pThis );
		
		static void	__stdcall onCalibEnd	(	xn::SkeletonCapability&			cap,
												XnUserID						userId,
												XnBool							isSuccess,
												void*							pThis );
		
		static void	_stdcall onGestRecog	(	xn::GestureGenerator&			generator,
												const XnChar*					strGesture,
												const XnPoint3D*				pIDPos,
												const XnPoint3D*				pEndPos,
												void*							pThis );
		
		static void __stdcall onGestProg	(	xn::GestureGenerator&			generator,
												const XnChar*					strGesture,
												const XnPoint3D*				pPosition,
												XnFloat							fProgress,
												void*							pThis );
		
		static void __stdcall onHandCreate	(	xn::HandsGenerator				&generator,
												XnUserID						user,
												const XnPoint3D*				pPosition,
												XnFloat							fTime,
												void*							pThis );
		
		static void __stdcall onHandDestroy	(	xn::HandsGenerator				&generator,
												XnUserID						user,
												XnFloat							fTime,
												void*							pThis );

		static void __stdcall onHandUpdate	(	xn::HandsGenerator				&generator,
												XnUserID						user,
												const XnPoint3D*				pPosition,
												XnFloat							fTime,
												void*							pThis );

	 protected:
		//Members
		xn::Context				context_;
		xn::DepthGenerator		depthGenerator_;
		xn::UserGenerator		userGenerator_;
		xn::GestureGenerator	gestureGenerator_;
		xn::HandsGenerator		handsGenerator_;

		std::string			configPath_;
		XnChar				strPose_[20];

		bool				isNeedPose_;
		bool				isInit_;
		bool				isPaused_;

		XnCallbackHandle	hUserCB_;
		XnCallbackHandle	hCalibCB_;
		XnCallbackHandle	hPoseCB_;
		XnCallbackHandle	hGestCB_;
		XnCallbackHandle	hHandCB_;
		

	};
	
}

#endif