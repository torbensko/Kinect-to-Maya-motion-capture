#include "MayaOpenNIServerManager.hpp"
#include "MayaCapServer.hpp"
#include "XnManager.hpp"

namespace MayaOpenNIServer{

	//--------------------------------------------------------
	// C_XnManager
	//--------------------------------------------------------
	C_XnManager::C_XnManager():
		isNeedPose_( false ),
		isInit_( false ),
		isPaused_( false )
	{
		configPath_ = "./SamplesConfig.xml";
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	C_XnManager::~C_XnManager(){
		context_.Shutdown();
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::initUserTracer( void ){

		struct SafeCheck{
			inline void operator()( XnStatus	status, const char* what = 0 ){
				if( status != XN_STATUS_OK ){
					throw std::exception( what );
				}
			}
		};

		try{

			isInit_ = false;

			//Init
			SafeCheck()( context_.InitFromXmlFile( configPath_.c_str() ), "Load XML file" );
			SafeCheck()( context_.FindExistingNode( XN_NODE_TYPE_DEPTH, depthGenerator_ ), "Depth Generator" );
			if( XN_STATUS_OK != context_.FindExistingNode( XN_NODE_TYPE_USER, userGenerator_ ), "FindExistingNode" ){
				SafeCheck()( userGenerator_.Create( context_ ), "create handGenerator" );
			}

			if( !userGenerator_.IsCapabilitySupported( XN_CAPABILITY_SKELETON ) ){
				throw std::exception();
			}

			userGenerator_.RegisterUserCallbacks( onNewUser, onLostUser, this, hUserCB_ );
			userGenerator_.GetSkeletonCap().RegisterCalibrationCallbacks( onCalibStart, onCalibEnd, this, hCalibCB_ );

			if( userGenerator_.GetSkeletonCap().NeedPoseForCalibration() ){
				isNeedPose_ = true;

				if( !userGenerator_.IsCapabilitySupported( XN_CAPABILITY_POSE_DETECTION ) ){
					throw std::exception();
				}

				userGenerator_.GetPoseDetectionCap().RegisterToPoseCallbacks( onPoseDetect, 0, this, hPoseCB_ );
				userGenerator_.GetSkeletonCap().GetCalibrationPose( strPose_ );
			}

			userGenerator_.GetSkeletonCap().SetSkeletonProfile( XN_SKEL_PROFILE_ALL );

			//Hands
			SafeCheck()( handsGenerator_.Create( context_ ), "create handGenerator" );
			handsGenerator_.RegisterHandCallbacks( onHandCreate, onHandUpdate, onHandDestroy, this, hHandCB_ );
			
			//Gesture
			SafeCheck()( gestureGenerator_.Create( context_ ), "create gestureGenerator" );
			gestureGenerator_.RegisterGestureCallbacks( onGestRecog, onGestProg, this, hGestCB_ );

			XnBoundingBox3D	bbox;
			bbox.LeftBottomNear.X = -10000.0f;
			bbox.LeftBottomNear.Y = -10000.0f;
			bbox.LeftBottomNear.Z = -10000.0f;
			bbox.RightTopFar.X = 10000.0f;
			bbox.RightTopFar.Y = 10000.0f;
			bbox.RightTopFar.Z = 10000.0f;
			gestureGenerator_.AddGesture( "Wave", &bbox );
			gestureGenerator_.AddGesture( "Click", &bbox );
			SafeCheck()( context_.StartGeneratingAll() );

			userGenerator_.GetSkeletonCap().SetSmoothing( static_cast< XnFloat >( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().smoothing_ ) );
			handsGenerator_.SetSmoothing( static_cast< XnFloat >( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().smoothing_ ) );

			isInit_ = true;
			
		}catch(...){
			throw;
		}
		
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onNewUser		(	xn::UserGenerator&	generator,
										XnUserID			userId,
										void*				pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );
		if( self->isNeedPose() ){
			self->getUserGenerator().GetPoseDetectionCap().StartPoseDetection( self->strPose_, userId );
		}else{
			self->getUserGenerator().GetSkeletonCap().RequestCalibration( userId, TRUE );
		}
		
	}
		
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onLostUser	(	xn::UserGenerator&	generator,
										XnUserID			userId,
										void*				pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );
		
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onPoseDetect	(	xn::PoseDetectionCapability&	cap,
										const XnChar*					strPos,
										XnUserID						userId,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );

		self->getUserGenerator().GetPoseDetectionCap().StopPoseDetection( userId );
		self->getUserGenerator().GetSkeletonCap().RequestCalibration( userId, TRUE );
		
	}
		
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onCalibStart	(	xn::SkeletonCapability&			cap,
										XnUserID						userId,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );
		
	}
		
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onCalibEnd		(	xn::SkeletonCapability&			cap,
										XnUserID						userId,
										XnBool							isSuccess,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );

		if( isSuccess ){
			self->getUserGenerator().GetSkeletonCap().StartTracking( userId );

		}else{
			if( self->isNeedPose() ){
				self->getUserGenerator().GetPoseDetectionCap().StartPoseDetection( self->strPose_, userId );
			}else{
				self->getUserGenerator().GetSkeletonCap().RequestCalibration( userId, TRUE );
			}
		}
		
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onGestRecog	(	xn::GestureGenerator&			generator,
										const XnChar*					strGesture,
										const XnPoint3D*				pIDPos,
										const XnPoint3D*				pEndPos,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );

		E_GestureId id = kGestureNone;
		
		if( strcmp( strGesture, GESTURE_NAMES[ kGestureWave ] ) == 0 ){
			id = kGestureWave;
		}else if( strcmp( strGesture, GESTURE_NAMES[ kGestureClick ] ) == 0 ){
			id = kGestureClick;
		}
		
		C_MayaOpenNIServerManager::getManager()->callGestureCmd( id );
		
		if( C_MayaOpenNIServerManager::getManager()->getBehavior().handTrackingGestureId_ != kGestureNone &&
			C_MayaOpenNIServerManager::getManager()->getBehavior().handTrackingGestureId_ == id )
		{
			self->handsGenerator_.StopTrackingAll();
			self->handsGenerator_.StartTracking( *pIDPos );

		}

	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onGestProg	(	xn::GestureGenerator&			generator,
										const XnChar*					strGesture,
										const XnPoint3D*				pPosition,
										XnFloat							fProgress,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );

	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onHandCreate	(	xn::HandsGenerator				&generator,
										XnUserID						user,
										const XnPoint3D*				pPosition,
										XnFloat							fTime,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );

	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onHandDestroy	(	xn::HandsGenerator				&generator,
										XnUserID						user,
										XnFloat							fTime,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );

//		self->handsGenerator_.StopTracking( user );
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_XnManager::onHandUpdate	(	xn::HandsGenerator				&generator,
										XnUserID						user,
										const XnPoint3D*				pPosition,
										XnFloat							fTime,
										void*							pThis )
	{
		C_XnManager*	self = reinterpret_cast< C_XnManager* >( pThis );

		C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().handPosition_[0] = pPosition->X;
		C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().handPosition_[1] = pPosition->Y;
		C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().handPosition_[2] = pPosition->Z;

		printf( "%f %f %f\n", pPosition->X, pPosition->Y, pPosition->Z );
	}
	
}