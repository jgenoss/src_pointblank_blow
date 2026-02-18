#if !defined( __I3_GFX_MOVIE_H)
#define __I3_GFX_MOVIE_H

#include "i3GfxResource.h"
#include "i3Texture.h"

#include "i3GfxDefine.h"
#include "smartptr.h"
#include "i3TextureDX.h"

#include <control.h>

class i3TextureRenderer;


template class I3_EXPORT_GFX ISmartPtrCOM<IGraphBuilder>;
template class I3_EXPORT_GFX ISmartPtrCOM<IBaseFilter>;
template class I3_EXPORT_GFX ISmartPtrCOM<IMediaControl>;
template class I3_EXPORT_GFX ISmartPtrCOM<IMediaSeeking>;
template class I3_EXPORT_GFX ISmartPtrCOM<IBasicAudio>;

class I3_EXPORT_GFX i3GfxMovie : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3GfxMovie, i3GfxResource );

	ISmartPtrCOM<IGraphBuilder>				m_pGB;				// GraphBuilder
	ISmartPtrCOM<IBaseFilter>				m_pRenderer;		// Renderer
	ISmartPtrCOM<IMediaControl>				m_pMC;				// IMediaControl
	ISmartPtrCOM<IMediaSeeking>				m_pMS;				// Seeking Pointer
	ISmartPtrCOM<IBasicAudio>				m_pBA;

	IMediaEvent*				m_pME = nullptr;
	i3TextureRenderer*			m_pCTR = nullptr;

	i3TextureDX*				m_pTexture[2] = { nullptr, nullptr };

	INT32						m_lVidWidth = 0;
	INT32						m_lVidHeight = 0;

	LONG						m_lVolume = 0;

	bool						m_bRepeat = true;
	char						m_pFileName[MAX_PATH] = { 0 };

	LONGLONG					m_lPos = 0;

public:

	i3GfxMovie();
	virtual ~i3GfxMovie();

	bool						InitDShowTextureRenderer( char* strFileName );

	bool						LoadBaseFilter( char* pstrFileName );
	bool						LoadASFFilter( char* pstrFileName );

	HRESULT						SetRenderOutputPins(IGraphBuilder *pGB, IBaseFilter *pFilter);

	long						CheckMovieStatus();
	void						CleanMovie();

	void						SetPlay();
	void						SetPause();
	void						SetStop();
	void						SetMute( bool bFlag );
	void						SetRate( double dRateAdjust);
	void						SetRepeat( bool bFlag)			{	m_bRepeat =  bFlag;		};
	void						SetPlayPosition(unsigned int);

	i3Texture*					GetTexture();
	void						SetTexture(i3TextureDX* pTexture[2])	{	m_pTexture[0] = pTexture[0]; m_pTexture[1] = pTexture[1]; };
	HRESULT						DoRender();
	INT32						GetVidWidth()					{	return m_lVidWidth;		};
	INT32						GetVidHeight()					{	return m_lVidHeight;	};
	bool						GetRepeat()						{	return m_bRepeat;		};

	virtual bool				Revive( i3RenderContext * pCtx) override;
	virtual bool				Destroy( bool bLostDevice) override;
};

#endif	// __I3_GFX_MOVIE_H