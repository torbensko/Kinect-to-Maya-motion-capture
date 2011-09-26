#ifndef __GLDISPLAYUI_HPP_INCLUDED__
#define __GLDISPLAYUI_HPP_INCLUDED__

#include <GL/glut.h>

#include "MayaOpenNIServerManager.hpp"

namespace UI{

	//----------------------------------
	// C_GlByteImageBuffer
	//----------------------------------
	class C_GlByteImageBuffer{
	 public:
		typedef	unsigned char BYTE;
		
	 public:
		//Creators
		C_GlByteImageBuffer();
		virtual ~C_GlByteImageBuffer();

	 private:
		//
		explicit C_GlByteImageBuffer( const C_GlByteImageBuffer& ){};
			
	 public:
		//Manipulators
		BYTE*			getBuffer		(	void );
		const	BYTE*	getBuffer		(	void ) const;
		void			allocBuffer		(	void );
		int				getWidth		(	void );
		int				getHeight		(	void );
		void			setSize			(	int		width,
											int		height );
		void			setChannelCount	(	int		count = 4 );
		int				getChannelCount	(	void );
		GLuint			getGLTextureID	(	void );
		void			assignGLTexture	(	void );
		bool			isAlloced		(	void );

	 protected:
		//Members
		BYTE*			pBuffer_;
		int				width_;
		int				height_;
		int				channelCount_;
		GLuint			glTexId_;
	};

	//----------------------------------
	// C_GlDisplay
	//----------------------------------
	class C_GlDisplay{
		
	 public:
		//Creators
		C_GlDisplay();
		virtual ~C_GlDisplay();
		
	 public:
		//Manipulators
		void	drawDepthMaps		(	void );
		void	drawSkelton			(	void );

		void	drawBuffer			(	unsigned char*			targetBuffer,
										int						width,
										int						height,
										float					leftPos,
										float					upperPos,
										float					rightPos,
										float					lowerPos );

		void	drawDepthImageToBuffer	(	void );

	 protected:
		//Members
		C_GlByteImageBuffer		depthImage_;
		float					depthColorTable_[11][3];

	};
	
}

#endif