#if !defined( __I3_VSM_BLUR_FILTER_H)
#define __I3_VSM_BLUR_FILTER_H

#include "i3UserAttr.h"

class i3SceneTracer;

class I3_EXPORT_SCENE i3VSM_BlurFilter : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3VSM_BlurFilter, i3ElementBase);
protected:
	i3Texture *				m_pSrcTex = nullptr;
	i3Texture *				m_pDestTex = nullptr;
	i3Texture *				m_pDepth = nullptr;

	i3Shader *				m_pShader = nullptr;

	i3VertexArray *			m_pVA = nullptr;

	VEC2D *					m_pOffset = nullptr;
	VEC4D *					m_pParams = nullptr;

	INT32					m_KernelSize = 0;


	// 손쉬운 구현을 위해 UserAttr의 Callback 함수 기능을 사용한다.
	i3UserAttr *			m_pUserAttr;

protected:
	void				_BuildParameters(void);
	void				_UpdateOffsetTable(void);
	
public:
	virtual ~i3VSM_BlurFilter(void);

	void				Create( INT32 kernelSize = 5 * 5);

	void				Bind( i3Texture * pSrcTex, i3Texture * pDestTex, i3Texture * pDepth);

	void				Process( i3SceneTracer * pTracer);

	static void			RenderProc( i3RenderContext * pCtx, void * pParam);
};

#endif
