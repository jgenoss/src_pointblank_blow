#if !defined( __I3_SAT_GENERATOR_H)
#define __I3_SAT_GENERATOR_H

#include "i3UserAttr.h"

class i3SceneTracer;

#define	RPP							4

class I3_EXPORT_SCENE i3SATGenerator : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3SATGenerator, i3ElementBase);
protected:
	i3Texture *				m_pSwapChain[2] = { nullptr, nullptr };
	i3Texture *				m_pDepth = nullptr;

	i3Shader *				m_pSumShader = nullptr;

	i3VertexArray *			m_pVA = nullptr;


	// 손쉬운 구현을 위해 UserAttr의 Callback 함수 기능을 사용한다.
	i3UserAttr *			m_pUserAttr;			// [initialize at constructor]
	
public:
	i3SATGenerator(void);
	virtual ~i3SATGenerator(void);

	void				Create(void);

	void				Bind( i3Texture * pSrcTex, i3Texture * pDestTex, i3Texture * pDepth);

	void				Process( i3SceneTracer * pTracer);

	static void			RenderProc( i3RenderContext * pCtx, void * pParam);
};

#endif
