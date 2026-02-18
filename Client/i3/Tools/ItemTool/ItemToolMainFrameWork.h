#pragma once

#include "ItemToolSubFrameWork.h"
#include "UI/UIMain.h"

class cUIMain;
class cItemToolMainFrameWork : public cItemToolFrameWork
{
public:
	I3_CLASS_DEFINE(cItemToolMainFrameWork, cItemToolFrameWork);

	cItemToolMainFrameWork(void);
	virtual ~cItemToolMainFrameWork(void);

	virtual BOOL Create( i3Viewer * pViewer);
	virtual void OnCreate(void);
	virtual void OnSize(POINT2D * pSize);
	virtual void OnLButtonDown(UINT32 nFlag, POINT point);
	virtual void OnMouseWheel( UINT nFlag, short zDelta, POINT point);
	virtual void OnUpdate( RT_REAL32 timeDelta);

	virtual bool LoadFile(LPCSTR pPath, LPCSTR pExt);
	bool	LoadTexture(LPCSTR pPath, const int& nPef);
	void	LoadCurrentPefTexture(const CString& strPath, const CString& strKey, UI_TEXTURE_TYPE textype);

	INT32	GetScrollCount(void) const;
	INT32	GetColPage(void) const;
	void	ResetItemList(INT32 nStartIndex);
	void	ResizeUI(void);

	void	SetBeretTexture(const std::string& strDiffuse, const std::string& strNormal, const std::string strSpecular,
							bool bforced = false,
							const std::string& strSrcDiffuse = "", const std::string& strSrcNormal = "", const std::string strSrcSpecular = "");

	void	SetChangeTexture(i3AttrSet*	pAttrSet,i3ClassMeta* pMeta, std::string szSrcTexture, std::string szDestTexture, bool bForced );

	void	SetMatrixMesh(i3Transform* pTrans, float fX, float fY, float fZ, VEC3D vPos);
	cUIMain*		GetUIMain() { return m_pUIMain; }
protected:
	virtual bool	_LoadI3S(LPCSTR pszPath);
	virtual bool	_LoadI3UI(LPCSTR pszPath);

private:
	cUIMain*			m_pUIMain;

	i3Node*				m_pSceneGraphInfo;
};
