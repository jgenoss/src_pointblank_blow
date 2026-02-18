#include "i3GfxType.h"
#include "i3GfxMovie.h"

#include "i3GfxGlobalVariable.h"
#include <atlconv.h>
#include <streams.h>

#include "i3Base/string/ext/extract_fileext.h"

//I3_CLASS_INSTANCE( i3GfxMovie, i3GfxResource);
I3_CLASS_INSTANCE( i3GfxMovie);

#define MOVIE_ERROR(p)	p = false; goto e_Exit;
#define VOLUME_FULL     0L
#define VOLUME_SILENCE  -10000L


// TextureRander
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

class i3TextureRenderer : public CBaseVideoRenderer
{
	I3G_IMAGE_FORMAT			m_TextureFormat = I3G_IMAGE_FORMAT_NONE;

	bool						m_bUseDynamicTextures = false;
	INT32						m_lVidWidth = 0;
	INT32						m_lVidHeight = 0;
	LONG						m_lVidPitch = 0L;
	i3TextureDX*				m_pTexture[2] = { nullptr, nullptr };
	IMediaSample*				m_pSample = nullptr;

public:
	bool						m_bIsLook = false;
	i3TextureRenderer(LPUNKNOWN pUnk, HRESULT *phr) : CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer),
		NAME("Texture Renderer"), pUnk, phr) {}
	~i3TextureRenderer() { I3TRACE("Movie Texture Release \n"); }

	HRESULT						CheckMediaType(const CMediaType *pmt );
	HRESULT						SetMediaType(const CMediaType *pmt );
	HRESULT						DoRenderSample(IMediaSample *pMediaSample);

	i3Texture*					GetTexture();
	void						SetTexture(i3TextureDX** pTexture)	{	m_pTexture[0] = pTexture[0]; m_pTexture[1] = pTexture[1]; };

	INT32						GetVidWidth()								{	return m_lVidWidth;				};
	INT32						GetVidHeight()								{	return m_lVidHeight;			};
	IMediaSample*				GetMediaSample()							{	return m_pSample;				};
	INT32						GetUseDynamic()								{	return m_bUseDynamicTextures;	};
	I3G_IMAGE_FORMAT			GetTextureFormat()							{	return m_TextureFormat;			};
	INT32						GetPitch()									{	return m_lVidPitch;				};
};

i3Texture*	i3TextureRenderer::GetTexture()
{
	if(m_bIsLook)
	{
		return m_pTexture[1];
	}
	else
	{
		return m_pTexture[0];
	}
}


//-----------------------------------------------------------------------------
// 타입 체크
//-----------------------------------------------------------------------------
HRESULT i3TextureRenderer::CheckMediaType(const CMediaType *pmt)
{
	HRESULT   hr = E_FAIL;

	CheckPointer( pmt, E_POINTER );

	// Reject the connection if this is not a video type
	if( *pmt->FormatType() != FORMAT_VideoInfo ) {
		return E_INVALIDARG;
	}	

	if(IsEqualGUID( *pmt->Type(),    MEDIATYPE_Video)  &&
		IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24)) {
			hr = S_OK;
	}
	
	return hr;
}

//-----------------------------------------------------------------------------
// 미디어 타입세팅
//-----------------------------------------------------------------------------
HRESULT i3TextureRenderer::SetMediaType(const CMediaType *pmt)
{
	I3TRACE("Movie Texture Create\n");

	HRESULT hr;

	UINT uintWidth	= 2;
	UINT uintHeight = 2;

	D3DCAPS9 caps;
	VIDEOINFO *pviBmp;
	pviBmp = (VIDEOINFO *)pmt->Format();

	m_lVidWidth  = pviBmp->bmiHeader.biWidth;
	m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
	m_lVidPitch  = (m_lVidWidth * 3 + 3) & ~(3);

	i3mem::FillZero( &caps, sizeof(D3DCAPS9));
	hr = ((LPDIRECT3DDEVICE9)g_pRenderContext->GetNativeDevice())->GetDeviceCaps( &caps );
	if( caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES )
	{
		m_bUseDynamicTextures = true;    
	}

	if( caps.TextureCaps & D3DPTEXTURECAPS_POW2 )
	{
		while( (LONG)uintWidth < m_lVidWidth )
			uintWidth	= uintWidth << 1;
		while( (LONG)uintHeight < m_lVidHeight )
			uintHeight	= uintHeight << 1;
	}
	else
	{
		uintWidth	= m_lVidWidth;
		uintHeight	= m_lVidHeight;
	}	

	hr = E_UNEXPECTED;
	if( m_bUseDynamicTextures )
	{
		if(false == m_pTexture[0]->Create( uintWidth, uintHeight, 1, I3G_IMAGE_FORMAT_BGRX_8888, I3G_USAGE_NONE ))
		{
			m_bUseDynamicTextures = false;
		}
		m_pTexture[1]->Create( uintWidth, uintHeight, 1, I3G_IMAGE_FORMAT_BGRX_8888, I3G_USAGE_NONE );
	}

	if( false == m_bUseDynamicTextures )
	{
		if(false == m_pTexture[0]->Create( uintWidth, uintHeight, 1, I3G_IMAGE_FORMAT_BGRX_8888, 0 ))        
		{
			return hr;
		}
	}  

	m_TextureFormat = m_pTexture[0]->getSurface(0)->getFormat();
	m_TextureFormat = m_pTexture[1]->getSurface(0)->getFormat();

	if (m_TextureFormat != I3G_IMAGE_FORMAT_BGRX_8888 && m_TextureFormat != I3G_IMAGE_FORMAT_ARGB_1555) {
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT i3TextureRenderer::DoRenderSample( IMediaSample * pSample )
{
	m_pSample = pSample;
	return S_OK;
}


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


i3GfxMovie::i3GfxMovie() : m_lVolume(VOLUME_FULL)
{	
}

i3GfxMovie::~i3GfxMovie()
{
	CleanMovie();
}

bool	i3GfxMovie::InitDShowTextureRenderer( char* strFileName )
{	
	CoInitialize( nullptr );
	// filter graph	생성
	if (FAILED( CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB))) {	
		return false;
	}

	I3_SAFE_RELEASE_NODBG( m_pCTR );
	HRESULT	hr = S_OK;
	m_pCTR = new i3TextureRenderer( nullptr, &hr );
	if (FAILED(hr) || !m_pCTR) {
		return false;
	}

	NetworkDump_Malloc( m_pCTR, sizeof( i3TextureRenderer), __FILE__, __LINE__);
	
	m_pCTR->SetTexture( m_pTexture);
	m_pRenderer = m_pCTR;
	if (FAILED( m_pGB->AddFilter(m_pRenderer, L"TEXTURERENDERER"))) {
		return false;
	}

	char szExt[MAX_PATH] = {0, };
	i3::extract_fileext(strFileName, szExt);

	i3::safe_string_copy( m_pFileName, strFileName, MAX_PATH);
	
	// asf, wma, wmv
	if( i3::generic_is_iequal( szExt, "asf" )  || 
		i3::generic_is_iequal( szExt, "wma" )  ||
		i3::generic_is_iequal( szExt, "wmv" )  ) {
		if(false == LoadASFFilter( strFileName ))		return false;
	}
	// avi, qt, mov, mpg, mpeg, m1v
	else {	
		if(false == LoadBaseFilter( strFileName ))		return false;
	}

	m_lVidWidth		= m_pCTR->GetVidWidth();
	m_lVidHeight	= m_pCTR->GetVidHeight();

	m_pGB.QueryInterface(&m_pMC);
	m_pGB.QueryInterface(&m_pME);
	m_pGB->QueryInterface(IID_IMediaSeeking, (void**)&m_pMS);
	m_pGB->QueryInterface(IID_IBasicAudio, (void**)&m_pBA);
	m_pBA->put_Balance(0);
	m_pBA->put_Volume(-500);

	if (FAILED( m_pMC->Stop())) {
		return false;
	}

	return true;
}

// ------------------------------------------------------------------------- //
// NAME: LoadBaseFilter()
// DESC: avi, qt, mov, mpg, mpeg, m1v
// ------------------------------------------------------------------------- //
bool	i3GfxMovie::LoadBaseFilter( char* pstrFileName )
{
	bool				bResult			= true;
	IBaseFilter			*pFSrc			= nullptr;
	IPin				*pFSrcPinOut	= nullptr;

	WCHAR wFileName[MAX_PATH];
	wFileName[MAX_PATH-1]	= 0;
	USES_CONVERSION;
	wcsncpy(wFileName, T2W(pstrFileName), NUMELMS(wFileName));
	
	if(FAILED( m_pGB->AddSourceFilter ( wFileName, L"SOURCE", &pFSrc )) ) {
		MOVIE_ERROR( bResult );
	}

	if (FAILED( pFSrc->FindPin( L"Output", &pFSrcPinOut ))) {
		MOVIE_ERROR( bResult );
	}
	if (FAILED( m_pGB->Render( pFSrcPinOut ))) {
		MOVIE_ERROR( bResult );
	}

e_Exit:
	I3_SAFE_RELEASE_NODBG( pFSrc );
	I3_SAFE_RELEASE_NODBG( pFSrcPinOut );

	return bResult;
}

// ------------------------------------------------------------------------- //
// NAME: LoadBaseFilter()
// DESC: asf, wma, wmv
// ------------------------------------------------------------------------- //
bool	i3GfxMovie::LoadASFFilter( char* pstrFileName )
{
	bool				bResult			= true;
	IBaseFilter			*pFilter		= nullptr;
	IFileSourceFilter	*pFileSource	= nullptr;

	WCHAR wFileName[MAX_PATH];
	wFileName[MAX_PATH-1]	= 0;
	USES_CONVERSION;
	wcsncpy(wFileName, T2W(pstrFileName), NUMELMS(wFileName));

	// ASF 필터 로드
    if(FAILED( CoCreateInstance(CLSID_WMAsfReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **) &pFilter))) {
		MOVIE_ERROR( bResult );
	}
	
	// ASF 필터 십입
	if(FAILED( m_pGB->AddFilter( pFilter, L"ASF Reader" ))) {
		MOVIE_ERROR( bResult );
	}

	if(FAILED( pFilter->QueryInterface( IID_IFileSourceFilter, (void **) &pFileSource ))) {
		MOVIE_ERROR( bResult );
	}
	if(FAILED( pFileSource->Load( wFileName, nullptr ))) {
		MOVIE_ERROR( bResult );
	}
	if( FAILED( SetRenderOutputPins( m_pGB , pFilter ))) {
		MOVIE_ERROR( bResult );
	}

e_Exit:
	I3_SAFE_RELEASE_NODBG( pFilter );
	I3_SAFE_RELEASE_NODBG( pFileSource );

	return bResult;
}

// ------------------------------------------------------------------------- //
// NAME: RenderOutputPins()
// DESC: 필터 추가 삽입.
// ------------------------------------------------------------------------- //
HRESULT	i3GfxMovie::SetRenderOutputPins(IGraphBuilder *pGB, IBaseFilter *pFilter)
{
	HRESULT					hr					= S_OK;
	IEnumPins				*pEnumPin			= nullptr;
	IPin					*pConnectedPin		= nullptr;
	IPin					*pPin				= nullptr;
	PIN_DIRECTION			PinDirection;
	ULONG					ulFetched;

	// Enumerate all pins on the filter
	if (FAILED( hr = pFilter->EnumPins( &pEnumPin ))) {
		//MOVIE_ERROR( bResult );
		goto e_Exit;
	}
	// Step through every pin, looking for the output pins
	while (S_OK == (hr = pEnumPin->Next(1L, &pPin, &ulFetched)))
	{
		// Is this pin connected?  We're not interested in connected pins.
		hr = pPin->ConnectedTo(&pConnectedPin);

		I3_SAFE_RELEASE_NODBG( pConnectedPin );

		// If this pin is not connected, render it.
		if (VFW_E_NOT_CONNECTED == hr)
		{
			hr = pPin->QueryDirection(&PinDirection);
			if ((S_OK == hr) && (PinDirection == PINDIR_OUTPUT))
			{
				hr = pGB->Render(pPin);
			}
		}
		pConnectedPin->Release();
		pPin->Release();

		// If there was an error, stop enumerating
		if (FAILED(hr))				break;
	}

e_Exit:
	// Release pin enumerator
	I3_SAFE_RELEASE_NODBG( pEnumPin );
	return hr;
}

//-----------------------------------------------------------------------------
// CheckMovieStatus: If the movie has ended, rewind to beginning
//-----------------------------------------------------------------------------
long	i3GfxMovie::CheckMovieStatus(void)
{
	long evCode1, evCode2, evCode3;
	m_pME->GetEvent(&evCode1, &evCode2, &evCode3, 1);
	return evCode1;

}

i3Texture*	i3GfxMovie::GetTexture()					
{
	return m_pCTR->GetTexture();
}


//-----------------------------------------------------------------------------
// CleanupDShow
//-----------------------------------------------------------------------------
void	i3GfxMovie::CleanMovie(void)
{
	if (!(!m_pMC)) m_pMC->Stop();
    if (!(!m_pMC)) m_pMC.Release();
	if( m_pME != nullptr)	m_pME->Release();
	if (!(!m_pGB)) m_pGB.Release();
	if (!(!m_pRenderer)) m_pRenderer.Release();
	

	m_pRenderer		= nullptr;
	m_pCTR			= nullptr;
	m_lVolume		= VOLUME_FULL;
	m_bRepeat		= true;
	m_pME = nullptr;

	CoUninitialize();
}   

void	i3GfxMovie::SetPlay()
{
	if ( m_pMC )		m_pMC->Run();
}

void	i3GfxMovie::SetPause()
{
	if ( m_pMC )		m_pMC->Pause();
}

void	i3GfxMovie::SetStop()
{
	if ( m_pMC )
	{
		m_pMC->Stop();
		m_pMS->SetPositions(&m_lPos, AM_SEEKING_AbsolutePositioning, &m_lPos, AM_SEEKING_NoPositioning);
	}

}

void	i3GfxMovie::SetMute( bool bFlag )
{
	if( m_pGB == nullptr )			return;

	IBasicAudio* pBA;	
	m_pGB.QueryInterface(&pBA);

	if( pBA == nullptr )			return;

	LONG	lVolume;

	if( bFlag ) lVolume = m_lVolume;
	else		lVolume = VOLUME_SILENCE;
		
	pBA->put_Volume(lVolume);

	I3_SAFE_RELEASE_NODBG( pBA );
}

void	i3GfxMovie::SetRate( double dRateAdjust )
{
	if(m_pGB == nullptr)						return;
	if(dRateAdjust == 0)					return;

	IMediaPosition*	pMP = nullptr;	
	m_pGB.QueryInterface(&pMP);

	double dRate;

	if ( FAILED( pMP->get_Rate( &dRate )))	return;

	pMP->put_Rate( dRate + dRateAdjust );

	I3_SAFE_RELEASE_NODBG( pMP );
}

void i3GfxMovie::SetPlayPosition(unsigned int uPos)
{
	LONGLONG setTime = (LONGLONG)uPos * (LONGLONG)10000;
	m_pMS->SetPositions(&setTime, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
}

HRESULT i3GfxMovie::DoRender()
{
	BYTE		*pBmpBuffer, *pTxtBuffer;
	LONG		lTxtPitch;
	IMediaSample * pSample;

	BYTE*		pbS = nullptr;
	DWORD*		pdwS = nullptr;
	DWORD*		pdwD = nullptr;
	UINT		row, col, dwordWidth;
	i3Surface*	pSurface;
	pSample = m_pCTR->GetMediaSample();
	
	if(pSample == nullptr)
		return 0;
	CheckPointer(pSample,E_POINTER);
	CheckPointer(m_pTexture[0],E_UNEXPECTED);
	CheckPointer(m_pTexture[1],E_UNEXPECTED);
	

	pSample->GetPointer( &pBmpBuffer );
	if(m_pCTR->m_bIsLook)
		pSurface = (i3Surface*) m_pTexture[0]->getSurface(0);
	else
		pSurface = (i3Surface*) m_pTexture[1]->getSurface(0);

	if(pSurface->getSurface() == nullptr)
	{
		return 0;
	}
	if( m_pCTR->GetUseDynamic() )
	{
		//D3DUSAGE_DYNAMIC;
		pSurface->Lock( 0);
		pTxtBuffer = (BYTE*)pSurface->getLockedBuffer();
		lTxtPitch = (LONG)pSurface->getPitch();
	}
	else
	{
		pTxtBuffer = (BYTE*) m_pTexture[0]->Lock( 0);
		I3ASSERT( pTxtBuffer != nullptr);
		lTxtPitch = (LONG) m_pTexture[0]->GetPitch(0);
	}

	if (m_pCTR->GetTextureFormat() == I3G_IMAGE_FORMAT_BGRX_8888) 
	{
		dwordWidth = m_lVidWidth / 4;
		for( row = 0; row< (UINT)m_lVidHeight; row++)
		{
			pdwS = ( DWORD*)pBmpBuffer;
			pdwD = ( DWORD*)pTxtBuffer;

			for( col = 0; col < dwordWidth; col ++ )
			{
				if(pSurface->getSurface() == nullptr)
				{
					return 0;
				}
				pdwD[0] =  pdwS[0] | 0xFF000000;
				pdwD[1] = ((pdwS[1]<<8)  | 0xFF000000) | (pdwS[0]>>24);
				pdwD[2] = ((pdwS[2]<<16) | 0xFF000000) | (pdwS[1]>>16);
				pdwD[3] = 0xFF000000 | (pdwS[2]>>8);
				pdwD +=4;
				pdwS +=3;
			}

			pbS = (BYTE*) pdwS;
			for( col = 0; col < (UINT)m_lVidWidth % 4; col++)
			{
				*pdwD = 0xFF000000     |
					(pbS[2] << 16) |
					(pbS[1] <<  8) |
					(pbS[0]);
					pdwD++;
				pbS += 3;           
			}

			pBmpBuffer  += m_pCTR->GetPitch();
			pTxtBuffer	+= lTxtPitch;
		}
	}

	if (m_pCTR->GetTextureFormat() == I3G_IMAGE_FORMAT_ARGB_1555) 
	{
		for(int y = 0; y < m_lVidHeight; y++ ) 
		{
			BYTE *pBmpBufferOld = pBmpBuffer;
			BYTE *pTxtBufferOld = pTxtBuffer;   

			for (int x = 0; x < m_lVidWidth; x++) 
			{
				*(WORD *)pTxtBuffer = (WORD)
					(0x8000 +
					((pBmpBuffer[2] & 0xF8) << 7) +
					((pBmpBuffer[1] & 0xF8) << 2) +
					(pBmpBuffer[0] >> 3));

				pTxtBuffer += 2;
				pBmpBuffer += 3;
			}

			pBmpBuffer = pBmpBufferOld + m_pCTR->GetPitch();
			pTxtBuffer = pTxtBufferOld + lTxtPitch;
		}
	}
	pSurface->Unlock();
	m_pCTR->m_bIsLook = !m_pCTR->m_bIsLook;
	return S_OK;
}


bool	i3GfxMovie::Revive( i3RenderContext * pCtx)
{
	InitDShowTextureRenderer( m_pFileName );
	SetPlay();
	return true;
}

bool	i3GfxMovie::Destroy( bool bLostDevice)
{
	CleanMovie();
	return i3GfxResource::Destroy( bLostDevice);
}