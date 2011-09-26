#ifndef __CONTROLUI_HPP_INCLUDED__
#define __CONTROLUI_HPP_INCLUDED__


#include <fltk/Window.h>
#include <fltk/Widget.h>
#include <fltk/RadioButton.h>
#include <fltk/Group.h>
#include <fltk/TiledGroup.h>
#include <fltk/TabGroup.h>
#include <fltk/Input.h>
#include <fltk/Slider.h>
#include <fltk/ValueOutput.h>
#include <fltk/ValueSlider.h>
#include <fltk/NumericInput.h>
#include <fltk/Output.h>
#include <fltk/Menu.h>
#include <fltk/MenuBar.h>
#include <fltk/TextEditor.h>
#include <fltk/Choice.h>

#include <fltk/visual.h>
#include <fltk/glut.h>

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "GlDisplay.hpp"

namespace UI{

	class C_MainWindow;
	
	//----------------------------------
	// C_TrackWindow
	//----------------------------------
	class C_TrackWindow: public fltk::GlutWindow{
	 public:
		//Creators
		C_TrackWindow( int x, int y, int w, int h );

	 public:
		//Manipulators
		virtual void		draw			(	void );
		void				initGL			(	void );
		void				initTexs		(	const xn::DepthMetaData& depthMD );
		inline void			setParentWindows(	C_MainWindow*	p ){ parentWindow_ = p; };

		void				drawTracker		(	void );
		void				drawSweep		(	void );

	 protected:
		//Members
		bool				requirePostTrackInit_;

		GLuint				depthTexID_;
		unsigned char*		pDepthTexBuf_;
		int 				texWidth_;
		int					texHeight_;
		C_MainWindow*		parentWindow_;
		C_GlDisplay			glDisplay_;
		
	};
	

	//----------------------------------
	// C_MainWindow
	//----------------------------------
	class C_MainWindow: public fltk::Window{
	 public:
		//Creators
		C_MainWindow( const char*	label = 0 );
		virtual ~C_MainWindow();

	 public:
		//Manipulators
		virtual void			show					(	void );
		void					updateValueOutputs		(	void );
		void					updateServerStatus		(	void );
		
		void					onPressExit				(	void );
		void					onCloseWindow			(	void );
		void					onChangeSmoothing		(	void );
		void					onChangePortNumber		(	void );
		void					onChangeServerAddress	(	void );
		void					onChangeOffset			(	int		axs,
															double	offset );

	 public:
		//Static Manipulators
		static void		CB_onPressButtonExit			(	fltk::Widget*	pWidget,
															void*			pThis );

		static void		CB_onPressButtonStartServer		(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onPressButtonRestartServer	(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeSmoothing			(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangePortNumber			(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeServerAddress		(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeOffsetX				(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeOffsetY				(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeOffsetZ				(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeEditor_Gesture1		(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeEditor_Gesture2		(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeCheckGesture			(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeCmdPortNumber		(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeTrackingGesture		(	fltk::Widget*	pWidget,
															void*			pThis );
		
		static void		CB_onChangeWindow				(	fltk::Widget*	pWidget,
															void*			pThis );
	 protected:
		//Members
		fltk::TabGroup*			pTabGroup_;
		
		fltk::Group*			pTabPage1_;
		fltk::ValueSlider*		pValSliderSmoothing_;
		fltk::ValueSlider*		pValSliderOffsets[3];
		fltk::NumericInput*		pNumInputPortNumber;
		fltk::Input*			pInputServerAddress;
		fltk::Button*			pButtonStartStop;
		fltk::Output*			pOutputServerStatus;
		fltk::Output*			pOutputClientStatus;
		
		fltk::Group*			pTabPage2_;
		fltk::CheckButton*		pCheck_Gesture1_;
		fltk::TextEditor*		pTextEditor_Gesture1_;
		fltk::CheckButton*		pCheck_Gesture2_;
		fltk::TextEditor*		pTextEditor_Gesture2_;
		fltk::CheckButton*		pCheck_Gesture3_;
		fltk::TextEditor*		pTextEditor_Gesture3_;
		fltk::NumericInput*		pNumInputCmdPortNumber;
		fltk::Choice*			pChoiceTrackingGesture_;

		fltk::Group*			pTabPage3_;
		fltk::ValueOutput*		pPosValueOutputsX[24];
		fltk::ValueOutput*		pPosValueOutputsY[24];
		fltk::ValueOutput*		pPosValueOutputsZ[24];
		
		C_TrackWindow*		childGlView_;
		
		bool				enableGlView_;
		
	};
	
	
	//----------------------------------
	// makeUI
	//----------------------------------
	unsigned int __stdcall	makeUI	(	void* arg );
	
}

#endif