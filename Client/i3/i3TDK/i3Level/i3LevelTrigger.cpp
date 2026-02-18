#include "stdafx.h"
#include "i3LevelTrigger.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelParserHelper.h"

I3_CLASS_INSTANCE( i3LevelTrigger);

i3LevelTrigger::~i3LevelTrigger(void)
{
	RemoveAllAction();

	if(m_pIcon != nullptr)
	{
		if( g_pScene != nullptr)
			g_pScene->RemoveElement( m_pIcon);

		I3_SAFE_RELEASE( m_pIcon);
	}
}

void i3LevelTrigger::AddAction( i3LevelAction * pAction)
{
	I3_MUST_ADDREF(pAction);

	m_ActionList.push_back( pAction);
}

void i3LevelTrigger::RemoveAction( i3LevelAction * pAction)
{
	I3_MUST_RELEASE(pAction);

	i3::vu::remove(m_ActionList, pAction);
}

void i3LevelTrigger::RemoveAllAction(void)
{
	
	i3LevelAction * pAction;

	for( size_t i = 0; i < m_ActionList.size(); i++)
	{
		pAction = m_ActionList[i];

		I3_MUST_RELEASE(pAction);
	}

	m_ActionList.clear();
}

INT32 i3LevelTrigger::FindActionByName( const char * pszName)
{
	i3LevelAction * pAction;

	for(size_t i = 0; i < m_ActionList.size(); i++)
	{
		pAction = m_ActionList[i];

		if( i3::generic_is_iequal( pAction->GetName(), pszName) )
			return i;
	}

	return -1;
}

INT32 i3LevelTrigger::FindAction( i3LevelAction * pAct)
{
	
	i3LevelAction * pAction;

	for(size_t i = 0; i < m_ActionList.size(); i++)
	{
		pAction = m_ActionList[i];

		if( pAction == pAct)
			return i;
	}

	return -1;
}

void i3LevelTrigger::Freeze(void)
{
	if( m_pIcon != nullptr)
	{
		m_pIcon->Freeze();
	}
}

void i3LevelTrigger::Unfreeze(void)
{
	if( m_pIcon != nullptr)
	{
		m_pIcon->Unfreeze();
	}
}

void	i3LevelTrigger::GetInformationString( char * pszStr)
{
}


void	i3LevelTrigger::OnAttachScene( i3LevelScene * pScene)
{
	if( m_pIcon != nullptr)
		pScene->AddElement( m_pIcon, "Trigger");
}

void	i3LevelTrigger::OnDetachScene( i3LevelScene * pScene)
{
	if( m_pIcon != nullptr)
		pScene->RemoveElement( m_pIcon);
}

void	i3LevelTrigger::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelPropertyContainer::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of< i3LevelTrigger*>(pDest));

	i3LevelTrigger * pTrigger = (i3LevelTrigger*)pDest;


	i3LevelAction * pAction;

	pTrigger->RemoveAllAction();
	for(size_t i = 0; i < m_ActionList.size(); i++)
	{
		pAction = (i3LevelAction *) GetAction( i);

		pTrigger->AddAction( pAction);
	}
}

void	i3LevelTrigger::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3LevelAction * pAction;

	for( i = 0; i < GetActionCount(); i++)
	{
		pAction = GetAction( i);

		pAction->OnBuildObjectList( List);
	}

	i3LevelPropertyContainer::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct LEVEL_TRIGGER
	{
		INT8			m_ID[4] = { 'T', 'T', 'G', '1' };
		UINT32			m_InitState = 0;

		i3::pack::VEC3D	m_IconPos;
		i3::pack::VEC3D	m_IconScale;
		UINT32			m_IconColor = 0;

		INT32			m_RepeatCount = 0;
		UINT32			m_ActionCount = 0;

		UINT32			m_Reserved[16] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelTrigger::OnSave( i3ResourceFile * pFile)
{
	UINT32 Rc, Result;
	pack::LEVEL_TRIGGER Info;
	INT32 i;

	Result = i3LevelPropertyContainer::OnSave( pFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	{
		Info.m_InitState = m_InitState;
		Info.m_ActionCount = GetActionCount();
		Info.m_RepeatCount = GetRepeatCount();

		if( m_pIcon != nullptr)
		{
			Info.m_IconPos.x = i3Vector::GetX( m_pIcon->getPos());
			Info.m_IconPos.y = i3Vector::GetY( m_pIcon->getPos());
			Info.m_IconPos.z = i3Vector::GetZ( m_pIcon->getPos());

			/*
			Info.m_IconScale.x = i3Vector::GetX( m_pIcon->GetScale());
			Info.m_IconScale.y = i3Vector::GetY( m_pIcon->GetScale());
			Info.m_IconScale.z = i3Vector::GetZ( m_pIcon->GetScale());
			*/

			{
				COLOR col;

				m_pIcon->GetColor( &col);
				Info.m_IconColor = i3Color::ConvertRGBA32( &col);
			}
		}

		Rc = pFile->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelTrigger::OnSave()", "Could not write trigger information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( i = 0; i < GetActionCount(); i++)
	{
		OBJREF ref = (OBJREF) pFile->GetObjectPersistID( GetAction( i));

		Rc = pFile->Write( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelTrigger::OnSave()", "Could not write action reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32	i3LevelTrigger::OnLoad( i3ResourceFile * pFile)
{
	UINT32 Rc, Result;
	pack::LEVEL_TRIGGER Info;
	INT32 i;

	Result = i3LevelPropertyContainer::OnLoad( pFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	{
		Rc = pFile->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelTrigger::OnLoad()", "Could not read trigger information.");
			return STREAM_ERR;
		}
		Result += Rc;

		SetInitState( Info.m_InitState);
		SetRepeatCount( Info.m_RepeatCount);

		if( m_pIcon != nullptr)
		{
			VEC3D vec;
			COLOR col;

			i3Vector::Set( &vec, Info.m_IconPos.x, Info.m_IconPos.y, Info.m_IconPos.z);
			m_pIcon->setPos( &vec);

			//i3Vector::Set( &vec, Info.m_IconScale.x, Info.m_IconScale.y, Info.m_IconScale.z);
			//m_pIcon->SetScale( &vec);

			if( Info.m_IconColor != 0xFFFFFFF)
			{
				i3Color::ConvertRGBA32( Info.m_IconColor, &col);
				m_pIcon->SetColor( &col);
			}
		}

	}

	for( i = 0; i < (INT32) Info.m_ActionCount; i++)
	{
		OBJREF ref;
		i3LevelAction * pAction;

		Rc = pFile->Read( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3LevelTrigger::OnLoad()", "Could not read action reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pAction = (i3LevelAction *) pFile->FindObjectByID( ref);
		I3ASSERT( pAction);

		AddAction( pAction);
	}

	return Result;
}
