#if !defined( __I3_RASTERIZE_CHAIN_SIMPLE_MOTION_BLUR_H)
#define __I3_RASTERIZE_CHAIN_SIMPLE_MOTION_BLUR_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_SimpleMotionBlur : public i3RasterizeChainBase
{
	I3_EXPORT_CLASS_DEFINE( i3RasterizeChain_SimpleMotionBlur, i3RasterizeChainBase);

protected:
	bool			m_bfirstEnable = true;

	UINT32			m_nBlurHeight;		// [initialize at constructor]
	UINT32			m_nBlurWidth;		// [initialize at constructor]

	COLORREAL		m_BlurColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	REAL32			m_fAlphaBlur;		// [initialize at constructor]
	REAL32			m_fAlphaStart;		// [initialize at constructor]
	REAL32			m_fAlphaEnd;		// [initialize at constructor]
	REAL32			m_fBlurTime = 0.0f;
	REAL32			m_fProcessingTime = 0.0f;

	I3G_TEXTURE_BIND_CONTEXT	m_ctxTex;
	i3Shader *		m_pShader_BitBlt = nullptr;
	i3Shader *		m_pShader_Blend = nullptr;
	

	i3RasterizeChainContext *	m_pCtxTexIn = nullptr;
	i3RasterizeChainContext *	m_pCtxTexOut = nullptr;

public:
	i3RasterizeChain_SimpleMotionBlur(void);
	virtual ~i3RasterizeChain_SimpleMotionBlur(void);

	void			ApplyAccumBlur( I3RC_UPDATE * pInfo);
	void			ApplyBlur( I3RC_UPDATE * pInfo);

	void			SetColor( UINT8 r, UINT8 g, UINT8 b)		{ i3Color::Set( &m_BlurColor, r, g, b, 255);			}
	void			SetColor( I3COLOR * pColor)					{ i3Color::Set( &m_BlurColor, pColor);					}
	void			SetColorReal( REAL32 r, REAL32 g, REAL32 b)		
					{ 
						COLORREAL rColor;
						i3Color::Set( &rColor, r, g, b, 1.0f);			
						i3Color::Set( &m_BlurColor, &rColor);
					}	
	void			SetColorReal( COLORREAL color)				{ i3Color::Set( &m_BlurColor, &color);					}

	void			SetBlurAlpha( REAL32 val)		{ m_fAlphaBlur = val;				}
	REAL32			GetBlurAlpha( void)				{ return m_fAlphaBlur;				}

	void			SetBlurAlphaStart( REAL32 val)	{ m_fAlphaStart = i3Math::Clamp( val, 0.0f, 1.f);		}
	void			SetBlurAlphaEnd( REAL32 val)	{ m_fAlphaEnd = i3Math::Clamp( val, 0.0f, 1.f);			}
	REAL32			GetBlurAlphaStart( void)		{ return m_fAlphaStart;				}
	REAL32			GetBlurAlphaEnd( void)			{ return m_fAlphaEnd;				}
	//void			AddBlurAlpha( REAL32 val);

	REAL32			GetBlurTime(void)				{ return m_fBlurTime;				}
	REAL32			GetProcessingTime(void)			{ return m_fProcessingTime;			}

	//	fTime이 0.0f이면 무한 반복
	//	fStartBlurVal과 fEndBlurVal 값이 같으면 알파 기복 없이 일정한 값으로 유지
	void			StartEffect(REAL32 fTime, REAL32 fStartBlurVal, REAL32 fEndBlurVal);
	void			EndEffect(void);

	virtual void	SetEnable(bool bFlag) override;
	virtual INT32	GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec) override;
	virtual void	ApplyChain( I3RC_UPDATE * pInfo) override;
	virtual void	OnCreate( i3RasterizeChainManager * pManager) override;
};

#endif	// __I3_RASTERIZE_CHAIN_BLUR_H
