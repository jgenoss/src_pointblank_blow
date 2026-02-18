#pragma once

#include "UISlotList.h"
#include "UI3DView.h"


class cUIMain : public i3GameNode
{
	I3_CLASS_DEFINE( cUIMain, i3GameNode );

public:
	cUIMain(void);
	~cUIMain(void);

	bool Create(i3Node* pRenderNode);
	bool LoadFile(LPCSTR pPath);
	bool LoadUI(LPCSTR pszPath);

	bool SetUITexture( UI_TEXTURE_TYPE textype, INT32 nIndex, i3Texture* pTex );
	void ResetItemList( INT32 nStartIndex );
	void SetSize(POINT2D& Size);
	void ResizeUI(void);
	void OnLButtonDown(const POINT& point);

	INT32 GetScrollCount(void) const;
	INT32 GetColPage(void) const;

	void	Set3DView(i3SceneGraphInfo * pSgInfo);
	
	cUI3DView* Get3DView() { return &m_3DView; }
	virtual void OnUpdate( RT_REAL32 rDeltaSeconds );
protected:
	void _Destroy(void);


private:
	cUI3DView			m_3DView;
	cUISlotList			m_SlotList;

	i3Node*				m_pSceneNode;
};
