#if !defined( __I3_RASTERIZE_CHAIN_SIMPLE_MOTION_BLUR_H)
#define __I3_RASTERIZE_CHAIN_SIMPLE_MOTION_BLUR_H

#include "i3RasterizeChainBase.h"

class I3_EXPORT_GFX i3RasterizeChain_SimpleMotionBlur : public i3RasterizeChainBase
{
	I3_CLASS_DEFINE( i3RasterizeChain_SimpleMotionBlur);

protected:
	BOOL			m_bfirstEnable;
	i3VertexArray *	m_pBlurVA;			//	블러 버텍스

	UINT32			m_nBlurHeight;
	UINT32			m_nBlurWidth;

	I3COLOR			m_BlurColor;
	REAL32			m_fAlphaBlur;
	REAL32			m_fAlphaStart;
	REAL32			m_fAlphaEnd;
	REAL32			m_fBlurTime;
	REAL32			m_fProcessingTime;
	BOOL			m_bActive;

public:
	i3RasterizeChain_SimpleMotionBlur(void);
	virtual ~i3RasterizeChain_SimpleMotionBlur(void);

	void			ApplyAccumBlur(void);
	void			UpdateAccumBlur(void);
	void			ApplyBlur(void);

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
	BOOL			IsActive(void)					{ return m_bActive;					}

	//	fTime이 0.0f이면 무한 반복
	//	fStartBlurVal과 fEndBlurVal 값이 같으면 알파 기복 없이 일정한 값으로 유지
	void			StartEffect(REAL32 fTime, REAL32 fStartBlurVal, REAL32 fEndBlurVal);
	void			EndEffect(void);

	virtual void	InitChain(void);
	virtual void	ApplyChain(REAL32 fDeltaSec);
	virtual void	SetEnable(BOOL bFlag);	

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	Revive( i3RenderContext * pCtx);
};

#endif	// __I3_RASTERIZE_CHAIN_BLUR_H
