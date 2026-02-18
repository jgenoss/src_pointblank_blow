#include "stdafx.h"
#include "i3LevelLayer.h"
#include "i3LevelElement3D.h"

I3_CLASS_INSTANCE( i3LevelLayer);

i3LevelLayer::~i3LevelLayer(void)
{
	RemoveAllElement();
}

void	i3LevelLayer::AddElement( i3LevelElement3D * pObj)
{
	I3ASSERT( pObj != nullptr);

	m_ElmList.push_back( pObj);

	if( getState() & I3LV_LAYER_STATE_HIDED)
	{
		pObj->Disable();
	}
	else
	{
		pObj->Enable();
	}
}

void	i3LevelLayer::RemoveElement( i3LevelElement3D * pObj)
{
	INT32 idx;

	I3ASSERT( pObj != nullptr);

	if( pObj->getLayer() == this)
	{
		// 
		pObj->m_pLayer = nullptr;
	}

	idx = FindElement( pObj);
	I3ASSERT( idx != -1);

	m_ElmList.erase(m_ElmList.begin() + idx);
}

INT32	i3LevelLayer::FindElement( i3LevelElement3D * pObj)
{
	INT32 i;

	for( i = 0; i < getElementCount(); i++)
	{
		if( getElement(i) == pObj)
			return i;
	}

	return -1;
}

void	i3LevelLayer::RemoveAllElement(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < getElementCount(); i++)
	{
		pElm = getElement( i);

		pElm->m_pLayer = nullptr;
	}

	m_ElmList.clear();
}

void i3LevelLayer::Show( BOOL bShow)
{
	INT32 i;
	i3LevelElement3D * pElm;

	if( bShow)
		removeState( I3LV_LAYER_STATE_HIDED);
	else
		addState( I3LV_LAYER_STATE_HIDED);

	for( i = 0; i < getElementCount(); i++)
	{
		pElm = getElement( i);

		if( bShow)
			pElm->Enable();
		else
			pElm->Disable();
	}
}

void i3LevelLayer::Freeze( BOOL bFreeze)
{
	INT32 i;
	i3LevelElement3D * pElm;

	if( bFreeze)
		addState( I3LV_LAYER_STATE_FREEZED);
	else
		removeState( I3LV_LAYER_STATE_FREEZED);

	for( i = 0; i < getElementCount(); i++)
	{
		pElm = getElement( i);

		if( bFreeze)
			pElm->Freeze();
		else
			pElm->Unfreeze();
	}
}

void i3LevelLayer::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelLayer*>(pDest));

	i3LevelLayer * pLayer = (i3LevelLayer *)pDest;

	pLayer->setState( getState());

	pLayer->RemoveAllElement();
	for( INT32 i = 0;i < getElementCount(); ++i)
	{
		pLayer->AddElement( getElement( i));
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct LEVEL_LAYER
	{
		INT8		m_ID[4] = { 'L', 'L', 'Y', '1' };
		UINT32		m_State = 0;
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3LevelLayer::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::LEVEL_LAYER data;

	result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_State = getState();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3LevelLayer::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::LEVEL_LAYER data;

	result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setState( data.m_State);

	return result;
}

bool i3LevelLayer::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->addAttr( "State",		m_State);

	return true;
}

bool i3LevelLayer::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->getAttr( "State",		& m_State);

	return true;
}
