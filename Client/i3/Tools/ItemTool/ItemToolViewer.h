#pragma once

#include "ITSwapChain.h"

enum eSwapChainType
{
	E_SWAPCHAIN_MAIN,
	E_SWAPCHAIN_WEAPON,
	E_SWAPCHAIN_ITEM,
	MAX_SWAPCHAIN
};

typedef std::vector<cSwapChainInfo*> vSwapChainCon;

class cItemToolViewer : public i3Viewer
{

public:
	I3_CLASS_DEFINE(cItemToolViewer, i3Viewer);
	cItemToolViewer(void);
	virtual ~cItemToolViewer(void);

	virtual BOOL	Create( void * pPlatformData, i3VideoInfo * pVideo, UINT32 DeviceMask = I3I_DEVICE_MASK_CONTROLLER | I3I_DEVICE_MASK_KEYBOARD, BOOL bEnableShader = TRUE, BOOL bDialogBoxMode = FALSE);
	virtual void	Render();
	virtual void	Resize( UINT32 cx, UINT32 cy);
	
	void			CreateLater();
	void			UpdateInput();

	void			RenderSwapChain();
	void			OnlyRenderSwapChain(cSwapChainInfo* pSwapChain);
	void			TraceSwapChain(cSwapChainInfo* pSwapChain);
	
	void			FindMaxResolution(INT32& nMaxW, INT32& nMaxH);

	//윈도우 메세지 핸들 등록(버쳐 함수가 아니라 그냥 비슷하게 새로만들었음)
	void			SetWndProc(HWND hWnd, bool bIsMainHwnd = true);
	void			_ToolSubclassWindow(HWND hWnd);
	void			_ToolUndoSubclassWindow(HWND hWnd);

	static LRESULT WINAPI _ItemToolViewerWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//디바이스 리셋(swapchain 리셋용)
	static void		PrevDeviceReset(bool bLostDevice);
	static void		AfterDeviceReset(i3RenderContext * pCtx);

public: //get,set
	void		SetSwapChainSgContext(i3SceneGraphContext * pCtx, eSwapChainType eType);

	void		SetWeaponHwnd(HWND hWnd, eSwapChainType eType = E_SWAPCHAIN_WEAPON);
	void		SetWeaponRect(RECT rt, eSwapChainType eType = E_SWAPCHAIN_WEAPON);
	void		SetSCFrameWork(i3Framework* pFrameWork, eSwapChainType eType = E_SWAPCHAIN_WEAPON);

	INT32		GetMaxW() const { return m_nMaxW; }
	INT32		GetMaxH() const { return m_nMaxH; }

	//해당 swapchain의 뷰어 크기
	INT32		GetWeaponDlgWidth(eSwapChainType eType = E_SWAPCHAIN_WEAPON) const { return m_vSwapChain[eType]->GetDlgWidth(); }
	INT32		GetWeaponDlgHeight(eSwapChainType eType = E_SWAPCHAIN_WEAPON) const { return m_vSwapChain[eType]->GetDlgHeight(); }

private:
	//스왑 체인 리스트 (eSwapchain 참고)
	static vSwapChainCon m_vSwapChain;

	//현재 모니터의 최대 크기
	INT32 m_nMaxW;
	INT32 m_nMaxH;

	//사운드 트랙바에 넘겨줄 시간
	float m_fCurrentAnimTime;

	//viewer 생성 시점
	bool m_bIsLoadedViewer;
};
