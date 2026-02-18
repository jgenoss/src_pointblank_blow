#pragma once

class cSwapChainInfo : public i3ElementBase
{
public:
	static cSwapChainInfo* m_spSwapChainInfo;
	I3_CLASS_DEFINE(cSwapChainInfo, i3ElementBase);
	cSwapChainInfo();
	virtual ~cSwapChainInfo();

	void Create(i3RenderContext* pContext, i3SoundContext* pSoundContext);
	void UpdateSwapChain();
	bool RenderSwapChain();
	void BegineScene();
	void EndScene();
	
	void PrevDeviceReset(bool bLostDevice);
	void AfterDeviceReset(i3RenderContext * pCtx);

	void SetHwnd(HWND hWnd) { m_hWnd = hWnd; }
	void SetRectHwnd(RECT& pRt) { m_rect = pRt; }
	void SetSwapChain(IDirect3DSwapChain9* pSwapChain) { m_pSwapChain = pSwapChain; }

	RECT GetRect() const { return m_rect; }
	HWND GetHwnd() const { return m_hWnd; }
	IDirect3DSwapChain9* GetSwapChain() { return m_pSwapChain; }

	INT32 GetDlgHeight() const { return m_nDlgHeight; }
	INT32 GetDlgWidth() const { return m_nDlgWidth; }

	void Trace();
	void Render();

	i3SceneTracer * GetSwapChainTracer() const { return m_pSwapChainTracer; }
	void SetSwapChainTracer(i3SceneTracer * val) { m_pSwapChainTracer = val; }

	i3SceneGraphContext * GetSwapChainSgContext() const { return m_pSwapChainSgContext; }
	void SetSwapChainSgContext(i3SceneGraphContext * val);

	void	SetName(CString strName) { m_strClassName = strName; }
	CString GetName() { return m_strClassName; }

	i3Framework* GetCurrentFrameWork() const { return m_pCurrentFrameWork; }
	void SetCurrentFrameWork(i3Framework* val) { m_pCurrentFrameWork = val; }

	i3RenderAttrListAttr * GetP2DOutput() { return m_p2DOutput; }

private:
	HWND m_hWnd;
	RECT m_rect;

	INT32 m_nDlgWidth;
	INT32 m_nDlgHeight;

	CString m_strClassName;
	
public:
	IDirect3DSwapChain9* m_pSwapChain;
	IDirect3DSurface9* m_pBackBuffer;

	i3SceneTracer *			m_pSwapChainTracer;

	i3SceneGraphContext *	m_pSwapChainSgContext;

	i3Framework* m_pCurrentFrameWork;

	i3RenderAttrListAttr *	m_p2DOutput;
	
	//void P2DOutput(i3RenderAttrListAttr * val) { m_p2DOutput = val; }


	
};