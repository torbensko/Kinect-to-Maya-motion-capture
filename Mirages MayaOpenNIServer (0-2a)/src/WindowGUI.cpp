#include <fltk/Window.h>
#include <fltk/Widget.h>
#include <fltk/Item.h>
#include <fltk/run.h>
#include <fltk/compat/FL/Fl_Menu_Item.H>

#include "WindowGUI.hpp"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include <fltk/glut.h>
#include "windows.h"

#include "MayaOpenNIServerManager.hpp"

namespace UI{

	using namespace fltk;
	using namespace MayaOpenNIServer;

	void glutIdle (void)
	{
	
		C_MayaOpenNIServerManager::getManager()->updateCaptureData();
		
		glutPostRedisplay();
		
	}
	
	//----------------------------------
	// C_TrackWindow
	//----------------------------------
	C_TrackWindow::C_TrackWindow( int x, int y, int w, int h ):
		requirePostTrackInit_( false ),
		GlutWindow( x, y, w, h, 0 ),
		parentWindow_( 0 )
	{
		border( true );
		clear_double_buffer();
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_TrackWindow::initGL( void ){

		make_current();
		
		mode( fltk::RGB24_COLOR | fltk::DOUBLE_BUFFER | fltk::DEPTH_BUFFER );
		
		typedef BOOL( APIENTRY *PROC )( BOOL );
		PROC wglSwapIntervalEXT = ( PROC )wglGetProcAddress( "wglSwapIntervalEXT" );
		
		if( wglSwapIntervalEXT ){
			wglSwapIntervalEXT( TRUE );
		}

		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		
		glutIdleFunc(glutIdle);

		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_TrackWindow::initTexs( const xn::DepthMetaData& depthMD ){

		struct getClosestPowerOfTwo{
			inline unsigned int operator()( unsigned int n){
				unsigned int m = 2;
				while(m < n) m<<=1;
				return m;
			};
		};

		GLuint texID = 0;
		glGenTextures( 1, &depthTexID_ );

		texWidth_ = getClosestPowerOfTwo()( depthMD.XRes() );
		texHeight_ = getClosestPowerOfTwo()( depthMD.YRes() );
		pDepthTexBuf_ = new unsigned char[ texWidth_ * texHeight_ * 4 ];
		glBindTexture( GL_TEXTURE_2D, depthTexID_ );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}

	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_TrackWindow::draw( void ){
		
		if( !C_MayaOpenNIServerManager::getManager()->getXnManager().isInit() ){
			drawSweep();
		}else{
			drawTracker();
		}

		parentWindow_->updateServerStatus();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_TrackWindow::drawSweep( void ){

		static int cnt = 0;

		glDrawBuffer( GL_BACK );
		
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glBegin(GL_LINES);
		{
			glColor4f( (float)( cnt % 255 ) / 255.0f, 1.0f, 1.0f, 1.0f );
			float xPos = (float)( ( cnt % MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeX_ * 2 ) ) / MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeX_ - 1.0f;
			glVertex2f( xPos, -1.0f );
			glVertex2f( xPos, 1.0f );
		}
		glEnd();

		requirePostTrackInit_ = true;

		++cnt;

	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_TrackWindow::drawTracker( void ){

		glDrawBuffer( GL_BACK );
		
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		// Process the data
		glDisplay_.drawDepthMaps();

		parentWindow_->updateValueOutputs();
		
	}
	
	//----------------------------------
	// C_MainWindow
	//----------------------------------
	C_MainWindow::C_MainWindow( const char*	label ):
		Window( USEDEFAULT, USEDEFAULT, 959, 550, label, true ),
		childGlView_( 0 ),
		enableGlView_( true )
	{
		this->label( "Maya OpenNI Server GUI", "GUI" );

		//Window CallBacks
		this->callback( CB_onChangeWindow, this );

		//Make Tabs
		this->begin();{
			{
				Fl_Menu_Item menutable[] = {
					{"&File",0,0,0,FL_SUBMENU},
						{ "&Quit",	fltk::COMMAND+'Q', CB_onPressButtonExit, 0 },
						{0},
					{0}
				};
				
				fltk::MenuBar* o = new fltk::MenuBar( 0, 0, 1039, 20 );
				o->menu( menutable );
			}
			
			
			{	pTabGroup_ = new TabGroup( 5, 30, 300, 500 );
				pTabGroup_->begin();

				int widgetYPos = 0;
				
				//TabPage 1
				{	pTabPage1_ = new fltk::Group( 5, 20, 290, 500, "Server Setting" );
					pTabPage1_->begin();

					pValSliderSmoothing_ = new fltk::ValueSlider( 80, 10, 150, 20, "Smoothing" );
					pValSliderSmoothing_->range( 0.0, 1.0 );
					pValSliderSmoothing_->value( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().smoothing_ );
					pValSliderSmoothing_->callback( CB_onChangeSmoothing, this );
					
					const char* offLab[3] = { "OffsetX", "OffsetY", "OffsetZ" };
					void ( *cbs[3] )( fltk::Widget*, void* ) = { C_MainWindow::CB_onChangeOffsetX, C_MainWindow::CB_onChangeOffsetY, C_MainWindow::CB_onChangeOffsetZ };

					for( unsigned int i = 0; i < 3; ++i ){
						pValSliderOffsets[i] = new fltk::ValueSlider( 0, 50 + i * 34, 250, 20, offLab[i] );
						pValSliderOffsets[i]->range( -10000.0, 10000.0 );
						pValSliderOffsets[i]->value( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().posOffset_[i] );
						pValSliderOffsets[i]->callback( cbs[i], this );
					}
					
					pNumInputPortNumber = new fltk::NumericInput( 100, 200, 100, 20, "Server Port" );
					pNumInputPortNumber->value( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().portNumber_ );
					pNumInputPortNumber->when( fltk::WHEN_CHANGED );
					pNumInputPortNumber->callback( CB_onChangePortNumber, this );

					pInputServerAddress = new fltk::Input( 100, 224, 100, 20, "Server Address" );
					pInputServerAddress->text( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().serverAddress_.c_str() );
					pInputServerAddress->when( fltk::WHEN_CHANGED );
					pInputServerAddress->callback( CB_onChangeServerAddress, this );
					
					//Restart button
					pButtonStartStop = new fltk::Button( 5, 260, 200, 30, "Start server" );
					pButtonStartStop->callback( CB_onPressButtonStartServer, this );

					pOutputServerStatus = new fltk::Output( 100, 290, 100, 20, "Server status" );
					pOutputServerStatus->text( "Stopped" );
					
					pOutputClientStatus = new fltk::Output( 100, 312, 100, 20, "Client status" );
					pOutputClientStatus->text( "Disconnected" );
					
					pTabPage1_->end();
					fltk::Group::current()->resizable( pTabPage1_ );
				}

				//TabPage2
				{	pTabPage2_ = new fltk::Group( 5, 20, 290, 500, "Gestrues" );
					pTabPage2_->begin();

					pNumInputCmdPortNumber = new fltk::NumericInput( 130, 5, 100, 20, "Maya Command Port" );
					pNumInputCmdPortNumber->value( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().cmdPortNumber_ );
					pNumInputCmdPortNumber->when( fltk::WHEN_CHANGED );
					pNumInputCmdPortNumber->callback( CB_onChangeCmdPortNumber, this );
					
					pCheck_Gesture1_ = new fltk::CheckButton( 5, 35, 100, 20, "Wave Gesture" );
					pCheck_Gesture1_->value( C_MayaOpenNIServerManager::getManager()->getBehavior().useGesture_[0] );
					pCheck_Gesture1_->callback( CB_onChangeCheckGesture, this );
					
					pTextEditor_Gesture1_ = new fltk::TextEditor( 5, 55, 270, 100, "" );
					pTextEditor_Gesture1_->when( fltk::WHEN_CHANGED );
					pTextEditor_Gesture1_->callback( CB_onChangeEditor_Gesture1, this );
					pTextEditor_Gesture1_->text( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().gestureCommands_[0].c_str() );

					pCheck_Gesture2_ = new fltk::CheckButton( 5, 160, 100, 20, "Click Gesture" );
					pCheck_Gesture2_->value( C_MayaOpenNIServerManager::getManager()->getBehavior().useGesture_[1] );
					pCheck_Gesture2_->callback( CB_onChangeCheckGesture, this );
					
					pTextEditor_Gesture2_ = new fltk::TextEditor( 5, 180, 270, 100, "" );
					pTextEditor_Gesture2_->when( fltk::WHEN_CHANGED );
					pTextEditor_Gesture2_->callback( CB_onChangeEditor_Gesture2, this );
					pTextEditor_Gesture2_->text( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().gestureCommands_[1].c_str() );


					pChoiceTrackingGesture_ = new fltk::Choice( 130, 380, 100, 20, "Hand Tracking Gesture" );
					pChoiceTrackingGesture_->begin();
					{
						new fltk::Item( "----------" );
						new fltk::Item( GESTURE_NAMES[ kGestureWave ] );
						new fltk::Item( GESTURE_NAMES[ kGestureClick ] );
					}
					pChoiceTrackingGesture_->end();
					if( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().handTrackingGestureId_ != kGestureNone ){
						pChoiceTrackingGesture_->value( (int)MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().handTrackingGestureId_ + 1 );
					}
					pChoiceTrackingGesture_->callback( CB_onChangeTrackingGesture, this );
					
					pTabPage2_->end();
					fltk::Group::current()->resizable( pTabPage2_ );
				}

				//TabPage3
				{	pTabPage3_ = new fltk::Group( 5, 20, 290, 500, "Raw Values" );
					pTabPage3_->begin();

					for( unsigned int i = 0; i < 24; ++i ){
						pPosValueOutputsX[ i ] = new fltk::ValueOutput( 10, i * 20, 50, 16 );
					}

					for( unsigned int i = 0; i < 24; ++i ){
						pPosValueOutputsY[ i ] = new fltk::ValueOutput( 60, i * 20, 50, 16 );
					}

					for( unsigned int i = 0; i < 24; ++i ){
						pPosValueOutputsZ[ i ] = new fltk::ValueOutput( 110, i * 20, 50, 16 );
					}

					pTabPage3_->end();
					fltk::Group::current()->resizable( pTabPage3_ );
				}
				
				pTabGroup_->end();
			}

			//Child GLWindow init
			if( enableGlView_ ){
				childGlView_ = new C_TrackWindow( 310, 50, MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeX_, MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeY_ );
				childGlView_->setParentWindows( this );
				childGlView_->initGL();
			}
		}
		this->end();

	}

	//- - - - - - - - - - - - - - - - - -
	//
	C_MainWindow::~C_MainWindow(){
		if( childGlView_ ){
			delete childGlView_;
		}

		for( unsigned int i = 0; i < 3; ++i ){
			delete pValSliderOffsets[i];
		}

		delete pTabPage2_;
		delete pTabPage1_;
		delete pTabGroup_;

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::updateValueOutputs( void ){
		
		for( char i = 0; i < 24; ++i ){
			pPosValueOutputsX[i]->value( C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[i].pos_[0] );
			pPosValueOutputsY[i]->value( C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[i].pos_[1] );
			pPosValueOutputsZ[i]->value( C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[i].pos_[2] );
		}
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::updateServerStatus( void ){

		if( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->isClientConnected() ){
			pOutputClientStatus->text( "Connected" );
			pButtonStartStop->activate();
			pButtonStartStop->label( "Stop server" );
			pButtonStartStop->redraw_label();
		}else if( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->isServerStarted() ){
			pOutputClientStatus->text( "Disconnected" );
			pButtonStartStop->label( "waiting for connection" );
			pButtonStartStop->redraw_label();
			pButtonStartStop->deactivate();
		}else{
			pOutputClientStatus->text( "Disconnected" );
			pButtonStartStop->activate();
			pButtonStartStop->label( "Start server" );
			pButtonStartStop->redraw_label();
		}
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::show( void ){
		Window::show();
		if( enableGlView_ && childGlView_ ){
			childGlView_->show();
		}
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::onPressExit( void ){
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->killThreads();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::onChangeSmoothing( void ){
		if( C_MayaOpenNIServerManager::getManager()->getXnManager().isInit() ){
			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().smoothing_ = pValSliderSmoothing_->value();
			C_MayaOpenNIServerManager::getManager()->getXnManager().getUserGenerator().GetSkeletonCap().SetSmoothing( static_cast< XnFloat >( pValSliderSmoothing_->value() ) );
			C_MayaOpenNIServerManager::getManager()->getXnManager().getHandsGenerator().SetSmoothing( static_cast< XnFloat >( pValSliderSmoothing_->value() ) );
		}
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::onChangePortNumber( void ){
		int val = strtol( pNumInputPortNumber->text(),0,0 );
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().portNumber_ = val;
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::onChangeServerAddress( void ){
		std::string val = pInputServerAddress->text();
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().serverAddress_ = val;
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::onCloseWindow( void ){
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->killThreads();
	}

	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::onChangeOffset( int axis, double offset ){
		
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().posOffset_[axis] = offset;
	}
	
	//- - - - - - - - - - - - - - - - - -
	//Static  CallBacks
	//
	void C_MainWindow::CB_onPressButtonExit( fltk::Widget*	pWidget,
												void*		pThis )
	{
		( (C_MainWindow*)pThis )->onPressExit();
		
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onPressButtonStartServer( fltk::Widget*	pWidget,
													void*		pThis )
	{
		if( !MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->isServerStarted() ){
			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->startServerThread();
			( (C_MainWindow*)pThis )->pOutputServerStatus->text( "Started" );
		}else if( MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->isClientConnected() ){
			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->stopServer();
			( (C_MainWindow*)pThis )->pOutputServerStatus->text( "Stopped" );
		}
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onPressButtonRestartServer( fltk::Widget*	pWidget,
														void*		pThis )
	{
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->startServerThread();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeServerAddress( fltk::Widget*	pWidget,
												void*		pThis )
	{
		( (C_MainWindow*)pThis )->onChangeServerAddress();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeSmoothing( fltk::Widget*	pWidget,
												void*		pThis )
	{
		( (C_MainWindow*)pThis )->onChangeSmoothing();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangePortNumber( fltk::Widget*	pWidget,
												void*		pThis )
	{
		( (C_MainWindow*)pThis )->onChangePortNumber();
	}

	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeWindow( fltk::Widget*	pWidget,
											void*		pThis )
	{
		( (C_MainWindow*)pThis )->onCloseWindow();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeOffsetX( fltk::Widget*	pWidget,
												void*		pThis )
	{
		( (C_MainWindow*)pThis )->onChangeOffset( 0, ( (C_MainWindow*)pThis )->pValSliderOffsets[0]->value() );
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeOffsetY( fltk::Widget*	pWidget,
												void*		pThis )
	{
		( (C_MainWindow*)pThis )->onChangeOffset( 1, ( (C_MainWindow*)pThis )->pValSliderOffsets[1]->value() );
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeOffsetZ( fltk::Widget*	pWidget,
												void*		pThis )
	{
		( (C_MainWindow*)pThis )->onChangeOffset( 2, ( (C_MainWindow*)pThis )->pValSliderOffsets[2]->value() );
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeEditor_Gesture1( fltk::Widget*	pWidget,
													void*		pThis )
	{
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().gestureCommands_[0] = 
		( (C_MainWindow*)pThis )->pTextEditor_Gesture1_->text();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeEditor_Gesture2( fltk::Widget*	pWidget,
													void*		pThis )
	{
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().gestureCommands_[1] = 
		( (C_MainWindow*)pThis )->pTextEditor_Gesture2_->text();
	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeCheckGesture( fltk::Widget*	pWidget,
												void*		pThis )
	{
		C_MainWindow* self = (C_MainWindow*)pThis;

		C_MayaOpenNIServerManager::getManager()->getBehavior().useGesture_[0] = self->pCheck_Gesture1_->value();
		C_MayaOpenNIServerManager::getManager()->getBehavior().useGesture_[1] = self->pCheck_Gesture2_->value();

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeCmdPortNumber( fltk::Widget*	pWidget,
												void*		pThis )
	{
		C_MainWindow* self = (C_MainWindow*)pThis;

		int val = strtol( self->pNumInputCmdPortNumber->text(),0,0 );
		MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().cmdPortNumber_ = val;

	}
	
	//- - - - - - - - - - - - - - - - - -
	//
	void C_MainWindow::CB_onChangeTrackingGesture(	fltk::Widget*	pWidget,
													void*		pThis )
	{
		C_MainWindow* self = (C_MainWindow*)pThis;

		int val = self->pChoiceTrackingGesture_->value();
		if( val == 0 ){
			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().handTrackingGestureId_ = kGestureNone;
		}else{
			MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().handTrackingGestureId_ = (E_GestureId)( val - 1 );
		}

	}
	
	//----------------------------------
	// makeUI
	//----------------------------------
	unsigned int __stdcall	makeUI	(	void* arg ){
		using namespace fltk;

		C_MainWindow* window = new C_MainWindow();
#if 0
		C_TrackWindow* trackWindown = new C_TrackWindow( 310, 50, MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeX_, MayaOpenNIServer::C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeY_ );
		trackWindown->setParentWindows( window );
		trackWindown->initGL();
		trackWindown->show();
#endif
		window->show();

		return run();
	}
	
}