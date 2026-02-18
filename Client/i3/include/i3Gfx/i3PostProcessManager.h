#if !defined(__I3_POSTPROCESS_MANAGER_H)
#define __I3_POSTPROCESS_MANAGER_H

#include "i3GfxResource.h"
#include "i3GfxGlobalVariable.h"
#include "i3RenderTarget.h"
#include "i3PostProcess.h"

#define SAMPLE_COUNT 9

class I3_EXPORT_GFX i3PostProcessManager : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE(i3PostProcessManager, i3GfxResource);

protected:
	i3RenderContext *					m_pCtx = nullptr;

	i3::vector<i3PostProcess*>			m_PostProcessList;
	i3::vector<i3RenderTarget*>			m_RenderTargetList;
	i3::vector<i3PostProcess*>			m_LastPostProcessList;	// shader effect above UI.

	i3RenderTarget *					m_pOriginRT = nullptr;
	i3RenderTarget *					m_pLastRT = nullptr;
	i3RenderTarget *					m_pOldDefaultRT;

	i3VertexArray *						m_pVA = nullptr;
	i3Shader *							m_pShader = nullptr;

	REAL32								m_fGaussiantheta = 0.8f;
	I3G_TEXTURE_BIND_CONTEXT			m_Ctx;

	VEC4D								m_fBlurOffsetWeight[SAMPLE_COUNT];

	REAL32								m_fDeltaTime = 0.0f;

private:

	void								_CreateQuadVertexArray();
	void								_ReleaseLists();
	void								_SetBlurParameter(REAL32 fx, REAL32 fy, UINT8 sampleCount, REAL32 theta);
	REAL32								_ComputeGaussian(REAL32 n, REAL32 theta);
	i3PostProcess *						_CheckEnablePostProcess();

	//이 함수의 직접 호출 금지. RegisterRenderTarget을 사용할 것.
	i3RenderTarget *					AddRenderTarget(UINT32 width, UINT32 height, I3G_IMAGE_FORMAT format, char * ppName = nullptr, bool isVolatile = true);

public:
	i3PostProcessManager();
	virtual ~i3PostProcessManager();

	void								Create(i3RenderContext * pGfx);
	
	i3RenderTarget *					RegisterRenderTarget(UINT32 width, UINT32 height, I3G_IMAGE_FORMAT format, char * ppName = nullptr, bool isVolatile = true);
	i3RenderTarget *					RegisterRenderTarget(REAL32 ratio, I3G_IMAGE_FORMAT format, char * ppName = nullptr, bool isVolatile = true);

	void								DeAllocRenderTarget(bool bAllTarget);

	i3RenderTarget *					FindRenderTarget(UINT32 width, UINT32 height, I3G_IMAGE_FORMAT format);
	i3RenderTarget *					FindRenderTargetByName(char * ppName);

	i3RenderTarget *					GetLastRenderTarget()								{ return m_pLastRT; }
	i3RenderTarget *					GetOriginalRenderTarget()							{ return m_pOriginRT; }
	i3RenderTarget *					ConfigRenderTarget();

	i3VertexArray *						GetQuadVertexArray()								{ return m_pVA; }
	void								DrawFullScreenQuad(i3RenderTarget * pRT, i3RenderContext * pCtx, i3VertexArray * pVA = nullptr);

	void								AddPostProcess(i3PostProcess * pProcess);
	void								AddLastPostProcess(i3PostProcess * pProcess);
	void								RemovePostProcess(i3PostProcess * pProcess);
	void								RemoveAllPostProcess();
	i3PostProcess *						FindPostProcess(i3ClassMeta * pMeta);

	i3PostProcess *						GetPostProcess(INT32 index)							{ return m_PostProcessList[index]; }
	INT32								GetPostProcessCount()								{ return m_PostProcessList.size(); }

	void								SetRenderTarget(i3RenderTarget * pTarget);

	void								Apply(REAL32 deltaTime);
	void								ApplyLast();
	void								ApplyGaussianBlur(i3RenderTarget * pDest, INT32 iterationCount, REAL32 theta);

	REAL32								GetDeltaTime()										{ return m_fDeltaTime; }
	inline INT32						getScreenWidth(void)								{ return m_pLastRT->GetWidth(); }
	inline INT32						getScreenHeight(void)								{ return m_pLastRT->GetHeight(); }

	void								LoadPostProcessFromXML(i3XMLFile * pXML);
	void								SavePostProcessToXML(i3XMLFile * pXML);

	virtual bool						Destroy(bool bLostDevice) override;
	virtual bool						isDestroyed() override;
	virtual bool						Revive(i3RenderContext * pCtx) override;
};


#endif