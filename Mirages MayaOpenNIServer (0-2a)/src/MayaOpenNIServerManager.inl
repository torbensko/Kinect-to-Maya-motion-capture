namespace MayaOpenNIServer{
	
	//--------------------------------------------------------
	// C_MayaOpenNIServerManager
	//--------------------------------------------------------
	const std::string& C_MayaOpenNIServerManager::getSkeltonName(	int	id ) const{
		return skelNames_[ id ];
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	const std::vector< std::string >&	C_MayaOpenNIServerManager::getSkeltonNames	(	void )const{
		return skelNames_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	uintptr_t	C_MayaOpenNIServerManager::getGUIThreadID		(	void ) const{
		return guiThreadId_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::setGUIThreadID		(	uintptr_t id ){
		guiThreadId_ = id;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	uintptr_t C_MayaOpenNIServerManager::getServerThreadID	(	void ) const{
		return guiThreadId_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::setServerThreadID	(	uintptr_t id ){
		serverThreadId_ = id;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::waitForGUIThreadEnd	(	void ){
		WaitForSingleObject( reinterpret_cast<HANDLE>( getGUIThreadID() ), INFINITE );
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	bool C_MayaOpenNIServerManager::setClientConnected	(	bool flag ){
		return clientConnected_ = flag;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	bool C_MayaOpenNIServerManager::isClientConnected	(	void ){
		return clientConnected_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	bool C_MayaOpenNIServerManager::isRequireRestartServer	(	void ){
		return requireRestartServer_; 
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::setRestartServer(	bool	flag ){
		requireRestartServer_ = flag;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	bool C_MayaOpenNIServerManager::isRequireStopServer	(	void ){
		return requireStopServer_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_MayaOpenNIServerManager::setStopServer		(	bool	flag ){
		requireStopServer_ = flag;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	C_XnManager&	C_MayaOpenNIServerManager::getXnManager( void ){
		return xnManager_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	const C_XnManager&	C_MayaOpenNIServerManager::getXnManager( void ) const{
		return xnManager_;
	}

	
	//- - - - - - - - - - - - - - - - - - - -
	//
	unsigned int	C_MayaOpenNIServerManager::getGestureFlag( void ) const{
		return gestureBit_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void	C_MayaOpenNIServerManager::setGestureFlag( unsigned int f ){
		gestureBit_ = f;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void	C_MayaOpenNIServerManager::orGestureFlag( unsigned int f ){
		gestureBit_ |= f;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void	C_MayaOpenNIServerManager::xorGestureFlag( unsigned int f ){
		gestureBit_ ^= ~f;
	}
}

