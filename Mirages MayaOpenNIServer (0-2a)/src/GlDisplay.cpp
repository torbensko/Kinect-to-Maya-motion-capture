#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

#include "GlDisplay.hpp"

namespace UI{

	using namespace MayaOpenNIServer;
	
	//----------------------------------
	// C_GlByteImageBuffer
	//----------------------------------
	C_GlByteImageBuffer::C_GlByteImageBuffer():
		pBuffer_( 0 ),
		width_( 0 ),
		height_( 0 ),
		channelCount_( 3 )
	{
		
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	C_GlByteImageBuffer::~C_GlByteImageBuffer(){
		if( pBuffer_ ){
			delete[] pBuffer_;
			pBuffer_ = 0;
		}
	}

	
	//- - - - - - - - - - - - - - - - - - - -
	//
	C_GlByteImageBuffer::BYTE* C_GlByteImageBuffer::getBuffer		(	void ){
		return pBuffer_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	const C_GlByteImageBuffer::BYTE* C_GlByteImageBuffer::getBuffer		(	void ) const{
		return pBuffer_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlByteImageBuffer::allocBuffer		(	void ){
		if( width_ > 0 && height_ > 0 && channelCount_ > 0 ){
			int alSize = width_ * height_ * channelCount_;
			pBuffer_ = new BYTE[ alSize ];
		}
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	int	C_GlByteImageBuffer::getWidth		(	void ){
		return width_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	int	C_GlByteImageBuffer::getHeight		(	void ){
		return height_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlByteImageBuffer::setSize	(	int		width,
											int		height )
	{
		width_ = width;
		height_ = height;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlByteImageBuffer::setChannelCount	(	int		count ){
		channelCount_ = count;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	int C_GlByteImageBuffer::getChannelCount	(	void ){
		return channelCount_;
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	GLuint C_GlByteImageBuffer::getGLTextureID	(	void ){
		return glTexId_;
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlByteImageBuffer::assignGLTexture	(	void ){
		
		glEnable( GL_TEXTURE_2D );
		
		glGenTextures( 1, &glTexId_ );
		glBindTexture( GL_TEXTURE_2D, glTexId_ );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glDisable( GL_TEXTURE_2D );
		
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	bool C_GlByteImageBuffer::isAlloced( void ){
		if( pBuffer_ ){
			return true;
		}
		return false;
	}
	
	//----------------------------------
	// C_GlDisplay
	//----------------------------------
	C_GlDisplay::C_GlDisplay(){
		float c [11][3] = {
			{ 0.0f,	1.0f,	1.0f },
			{ 0.0f,	0.0f,	1.0f },
			{ 0.0f,	1.0f,	0.0f },
			{ 1.0f,	1.0f,	0.0f },
			{ 1.0f,	0.0f,	0.0f },
			{ 0.0f,	1.0f,	1.0f },
			{ 0.0f,	0.0f,	1.0f },
			{ 0.0f,	1.0f,	0.0f },
			{ 1.0f,	1.0f,	0.0f },
			{ 1.0f,	1.0f,	1.0f },
			{ 1.0f,	1.0f,	1.0f },
		};

		for( unsigned int i = 0; i < 11; ++i ){
			for( unsigned int j = 0; j < 3; ++j ){
				depthColorTable_[i][j] = c[i][j];
			}
		}

	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	C_GlDisplay::~C_GlDisplay(){
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlDisplay::drawDepthMaps( void ){
		
		using namespace MayaOpenNIServer;

		if( !C_MayaOpenNIServerManager::getManager()->getXnManager().isInit() ){
			return;
		}

		xn::SceneMetaData sceneMetaData;
		xn::DepthMetaData depthMetaData;
		C_MayaOpenNIServerManager::getManager()->getXnManager().getDepthGenerator().GetMetaData( depthMetaData );
		
		if( !depthImage_.isAlloced() ){
			depthImage_.setSize( depthMetaData.XRes(), depthMetaData.YRes() );
			depthImage_.allocBuffer();
			depthImage_.assignGLTexture();
		}

		glDisable( GL_DEPTH_TEST );
		glDisable( GL_TEXTURE_2D );
		glEnableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
		
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		glOrtho( 0, C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeX_, C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeY_, 0, -1.0, 1.0 );

		drawDepthImageToBuffer();
		drawBuffer( depthImage_.getBuffer(), depthImage_.getWidth(), depthImage_.getHeight(),
					static_cast< float >( C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeX_ ), 0.0f,
					0.0f,	static_cast< float >( C_MayaOpenNIServerManager::getManager()->getBehavior().glWindowSizeY_ ) );

		drawSkelton();

		glPopMatrix();
		
	}
	
	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlDisplay::drawSkelton( void ){

		struct drawBone{
			inline void	operator()(	int	jointIndexFrom, int	jointIndexTo )
			{
				XnPoint3D pt[2];
				
				pt[0].X = C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[ jointIndexFrom - 1 ].oriPos_[0];
				pt[0].Y = C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[ jointIndexFrom - 1 ].oriPos_[1];
				pt[0].Z = C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[ jointIndexFrom - 1 ].oriPos_[2];
				pt[1].X = C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[ jointIndexTo - 1 ].oriPos_[0];
				pt[1].Y = C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[ jointIndexTo - 1 ].oriPos_[1];
				pt[1].Z = C_MayaOpenNIServerManager::getManager()->getCurrentCaptureData().skelParams_[ jointIndexTo - 1 ].oriPos_[2];
				
				C_MayaOpenNIServerManager::getManager()->getXnManager().getDepthGenerator().ConvertRealWorldToProjective( 2, pt, pt );

				glVertex3i( pt[0].X, pt[0].Y, 0 );
				glVertex3i( pt[1].X, pt[1].Y, 0 );
			};
		};

		XnUserID	userIds[10];
		XnUInt16	maxUserCount = 10;
		C_MayaOpenNIServerManager::getManager()->getXnManager().getUserGenerator().GetUsers( userIds, maxUserCount );

		if( C_MayaOpenNIServerManager::getManager()->getXnManager().getUserGenerator().GetSkeletonCap().IsTracking( userIds[0] ) ){
			
			glBegin( GL_LINES );
			glLineWidth( 6.0f );

			glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
			
			drawBone()( XN_SKEL_HEAD, XN_SKEL_NECK );
			
			drawBone()( XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER );
			drawBone()( XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW );
			drawBone()( XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND );

			drawBone()( XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER );
			drawBone()( XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW );
			drawBone()( XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND );

			drawBone()( XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO );
			drawBone()( XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO );

			drawBone()( XN_SKEL_TORSO, XN_SKEL_LEFT_HIP );
			drawBone()( XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE );
			drawBone()( XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT );

			drawBone()( XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP );
			drawBone()( XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE );
			drawBone()( XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT );

			drawBone()( XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP );

			glEnd();
		}
		
	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlDisplay::drawBuffer	(	unsigned char*			targetBuffer,
										int						width,
										int						height,
										float					leftPos,
										float					upperPos,
										float					rightPos,
										float					lowerPos )
	{

		static GLfloat uvCoord[8] = { 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat	v[8] = {	leftPos,	lowerPos,
							leftPos,	upperPos,
							rightPos,	upperPos,
							rightPos,	lowerPos };

		glBindTexture( GL_TEXTURE_2D, depthImage_.getGLTextureID() );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, targetBuffer );

		glColor4f( 0.75f, 0.75f, 0.75f, 1.0f );

		glEnable( GL_TEXTURE_2D );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		glTexCoordPointer( 2, GL_FLOAT, 0, uvCoord );

		glVertexPointer( 2, GL_FLOAT, 0, v );
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

		glFlush();

		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisable( GL_TEXTURE_2D );



	}

	//- - - - - - - - - - - - - - - - - - - -
	//
	void C_GlDisplay::drawDepthImageToBuffer	(	void ){

		static const int MAX_HIST_DEPTH( 10000 );

		xn::DepthMetaData	depthMetaData;
		xn::SceneMetaData	sceneMetaData;
		C_MayaOpenNIServerManager::getManager()->getXnManager().getDepthGenerator().GetMetaData( depthMetaData );
		C_MayaOpenNIServerManager::getManager()->getXnManager().getUserGenerator().GetUserPixels( 0, sceneMetaData );

		const XnDepthPixel*	pSrcPix = depthMetaData.Data();
		const XnLabel* pSrcLabel = sceneMetaData.Data();

		//Make histragm
		int depthHistgram[ MAX_HIST_DEPTH ];
		memset( depthHistgram, 0, MAX_HIST_DEPTH * sizeof( int ) );

		unsigned int pixCnt( 0 );
		for( unsigned int y = 0; y < depthMetaData.YRes(); ++y ){
			for( unsigned int x = 0; x < depthMetaData.XRes(); ++x ){
				if( ( 0 < *pSrcPix ) && ( *pSrcPix < MAX_HIST_DEPTH ) ){
					depthHistgram[ *pSrcPix ] += 1;
					++pixCnt;
				}
				++pSrcPix;
			}
		}

		for( unsigned int i = 1; i < MAX_HIST_DEPTH; ++i ){
			depthHistgram[ i ] += depthHistgram[ i - 1 ];
		}

		if( pixCnt ){
			for( unsigned int i = 1; i < MAX_HIST_DEPTH; ++i ){
				depthHistgram[i] = static_cast< int >( 256.0f * ( 1.0f - ( static_cast< float >( depthHistgram[i] ) / static_cast< float >( pixCnt ) ) ) );
			}
		}

		pSrcPix = depthMetaData.Data();
		
		unsigned char* pDstPix = depthImage_.getBuffer();
		for( unsigned int y = 0; y < depthMetaData.YRes(); ++y ){
			for( unsigned int x = 0; x < depthMetaData.XRes(); ++x ){
				*( pDstPix ) = *( pDstPix + 1 ) = *( pDstPix + 2 ) = 0;
				if( *pSrcPix ){
					int colorId = 10;
					if( *pSrcLabel ){
						colorId = *pSrcLabel % 10;
					}

					float fDephtVal = static_cast< float >( depthHistgram[ *pSrcPix ] );
					*( pDstPix ) = static_cast< char >( fDephtVal * depthColorTable_[ colorId ][0] );
					*( pDstPix + 1 ) = static_cast< char >( fDephtVal * depthColorTable_[ colorId ][1] );
					*( pDstPix + 2 ) = static_cast< char >( fDephtVal * depthColorTable_[ colorId ][2] );
				}

				++pSrcPix;
				++pSrcLabel;
				pDstPix += 3;
			}
		}
	}

	
}