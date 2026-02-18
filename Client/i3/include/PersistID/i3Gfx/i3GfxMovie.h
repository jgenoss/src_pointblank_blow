#if !defined( __I3_GFX_MOVIE_H)
#define __I3_GFX_MOVIE_H

#include "i3GfxResource.h"
#include "i3Texture.h"

#include "i3GfxDefine.h"

class i3TextureRenderer;

class I3_EXPORT_GFX i3GfxMovie : public i3GfxResource
{
	I3_CLASS_DEFINE( i3GfxMovie );

	IGraphBuilder*				m_pGB;				// GraphBuilder
	//IMediaControl*				m_pMC;				// 컨트롤러
	//IMediaPosition*				m_pMP;				// 위치지정
	IMediaEvent*				m_pME;				// 이벤트
	//IMediaSeeking				m_pMS;				// 위치 이동 컨트롤
	IBaseFilter*				m_pRenderer;		// Renderer
	///IBasicAudio*				m_pBA;				// 오디오 컨트롤
	
	
	i3TextureRenderer*			m_pCTR;

	i3Texture*					m_pTexture;

	INT32						m_lVidWidth;
	INT32						m_lVidHeight;

	LONG						m_lVolume;

	BOOL						m_bRepeat;

public:

	i3GfxMovie();
	virtual ~i3GfxMovie();

	BOOL						InitDShowTextureRenderer( char* strFileName );

	BOOL						LoadBaseFilter( char* pstrFileName );
	BOOL						LoadASFFilter( char* pstrFileName );
	//BOOL						LoadNoSoundFilter( char* pstrFileName );	 // 현재 작동하지 않습니다.

	HRESULT						SetRenderOutputPins(IGraphBuilder *pGB, IBaseFilter *pFilter);

	void						CheckMovieStatus();
	void						CleanMovie();

	void						SetPlay();
	void						SetPause();
	void						SetStop();
	void						SetMute( BOOL bFlag );
	void						SetRate( double dRateAdjust);
	void						SetRepeat( BOOL bFlag)			{	m_bRepeat =  bFlag;		};

	i3Texture*					GetTexture()					{	return m_pTexture;		};
	INT32						GetVidWidth()					{	return m_lVidWidth;		};
	INT32						GetVidHeight()					{	return m_lVidHeight;	};
	BOOL						GetRepeat()						{	return m_bRepeat;		};
};

#endif	// __I3_GFX_MOVIE_H