#if !defined( __I3_SAT_GENERATOR_H)
#define __I3_SAT_GENERATOR_H

#include "i3UserAttr.h"

class i3SceneTracer;

#define	RPP							4

class I3_EXPORT_SCENE i3SATGenerator : public i3ElementBase
{
	I3_CLASS_DEFINE( i3SATGenerator);
protected:
	i3Texture *				m_pSwapChain[2];
	i3Texture *				m_pDepth;

	i3Shader *				m_pSumShader;

	i3VertexArray *			m_pVA;


	// 손쉬운 구현을 위해 UserAttr의 Callback 함수 기능을 사용한다.
	i3UserAttr *			m_pUserAttr;
	
public:
	i3SATGenerator(void);
	virtual ~i3SATGenerator(void);

	void				Create(void);

	void				Bind( i3Texture * pSrcTex, i3Texture * pDestTex, i3Texture * pDepth);

	void				Process( i3SceneTracer * pTracer);

	static void			RenderProc( i3RenderContext * pCtx, void * pParam);
};

#endif
