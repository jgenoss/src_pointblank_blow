// ============================================================================
//
// File: i3UI.h
//
// Desc: i3UI Main Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3UIROOT_H__
#define __I3UIROOT_H__

#include "i3UIDefines.h"
#include "i3UIFrameWnd.h"
#include "i3UIControl.h"

#define I3UI_MAX_NOTIFY		16

enum I3UI_TEXT_USAGE_TYPE
{
	I3UI_TEXT_USAGE_TYPE_TEXTNODEDX = 0,		// D3DXFont사용 (Window에서 사용가능( DirectX사용가능한...)
	I3UI_TEXT_USAGE_TYPE_TEXTNODE,				// 이전 버전의 폰트 사용( 어느곳에서나 가능)
};

class i3UIManager;

class I3_EXPORT_FRAMEWORK i3UIRoot : public i3ResourceObject
{
	I3_EXPORT_CLASS_DEFINE(i3UIRoot, i3ResourceObject);
protected:
	i3::vector<i3UIControl*> m_ChildList;

	INT32					m_nWidth = 0;
	INT32					m_nHeight = 0;

public:
	virtual					~i3UIRoot( void);

	void					setWidth( INT32 nWidth)							{ m_nWidth = nWidth;				}
	INT32					getWidth()										{ return m_nWidth;					}

	void					setHeight( INT32 nHeight)						{ m_nHeight = nHeight;				}
	INT32					getHeight()										{ return m_nHeight;					}

	void					AddFromUIManager( i3UIManager * pManager);
	void					RemoveAll( void);

	i3::vector<i3UIControl*>& getChildList(void)								{ return m_ChildList;}

	virtual void			OnUpdate( REAL32 rDeltaSeconds );

	INT32					AddControl( i3UIControl * pCtrl);
	INT32					RemoveControl( i3UIControl * pCtrl);
	
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void			OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
};

#endif // __I3UIROOT_H__
