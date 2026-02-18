#if !defined( __I3_AI_CONTEXT_H)
#define __I3_AI_CONTEXT_H

#include "i3AI.h"
#include "i3AIState.h"
#include "i3GameNode.h"

class I3_EXPORT_FRAMEWORK i3AIContext : public i3GameNode
{
	I3_CLASS_DEFINE( i3AIContext);
protected:
	// Persistent members
	i3AI *			m_pAI;
	i3AIState *		m_pInitialAIState;

	// Volatile members
	i3Chara *		m_pChara;

	i3AIState *		m_pCurAIState;

	INT32			m_CurAIStateIndex;
	INT32			m_PrevAIStateIndex;
	INT32			m_NextAIStateIndex;

	REAL32			m_timeLocal;
	REAL32			m_timeStart;

public:
	i3AIContext(void);
	virtual ~i3AIContext(void);

	i3AIState *		getInitialAIState(void)		{ return m_pInitialAIState; }
	void			setInitialAIState( i3AIState * pState)
	{
		m_pInitialAIState = pState;
	}

	i3Chara *		getOwner(void)				{ return m_pChara; }
	void			setOwner( i3Chara * pChara)	{ m_pChara = pChara; }

	i3AI *			getAI(void)					{ return m_pAI; }
	void			SetAI( i3AI * pAI);

	REAL32			getLocalTime(void)			{ return m_timeLocal; }

	void			SetEnable( bool bEnable, bool bExclusive);

	i3AIState *		getCurrentAIState(void)		{ return m_pCurAIState; }
	i3AIState *		getNextAIState(void)		{ return m_pAI->getAIState( m_NextAIStateIndex); }

	INT32			getCurrnetAIStateIndex( void)	{ return m_CurAIStateIndex;		}
	INT32			getNextAIStateIndex( void)		{ return m_NextAIStateIndex;	}

	void			setCurrentAIState( INT32 idx, REAL32 timeStart = 0.0f, REAL32 timeScale = 1.0f)
	{
		if((m_pCurAIState != NULL) && isState( I3_GAMENODE_STATE_LOCKED))
			return;

		i3AIState * pAIState = getAIState( idx);
		I3ASSERT( pAIState != NULL);

		pAIState->setTimeScale( timeScale);

		m_NextAIStateIndex = idx;
		m_timeStart = timeStart;
	}

	void			setCurrentAITimeScale( REAL32 timeScale = 1.f )
	{
		if(m_pCurAIState == NULL)
			return;
		if( isState( I3_GAMENODE_STATE_LOCKED) )
			return;

		m_pCurAIState->setTimeScale( timeScale);
	}

	void			setCurrentAIState( i3AIState * pState, REAL32 timeStart = 0.0f, REAL32 timeScale = 1.0f)
	{
		if( pState == NULL)	return;

		INT32 idx = m_pAI->FindAIState( pState);
		I3ASSERT( idx != -1);

		setCurrentAIState( idx, timeStart, timeScale);
	}

	void			setCurrentAIState( const char * pszName, REAL32 timeStart = 0.0f, REAL32 timeScale = 1.0f)
	{
		if( pszName == NULL)	return;

		INT32 idx = FindAIStateByName( pszName);
		I3ASSERT( idx != -1);

		setCurrentAIState( idx, timeStart, timeScale);
	}

	INT32			AddAIState( i3AIState * pAIState)						{ return m_pAI->AddAIState( pAIState); }
	INT32			FindAIStateByName( const char * szName)					{ return m_pAI->FindAIStateByName( szName); }
	INT32			FindAIStateByID( INT32 id)								{ return m_pAI->FindAIStateByID( id); }
	INT32			FindAIState( i3AIState * pState)						{ return m_pAI->FindAIState( pState); }
	void			RemoveAIStateAll( void);
	void			RemoveAIStateFromId( INT32 id);
	void			RemoveAIStateFromIndex( INT32 index);
	void			RemoveAIState( i3AIState * pAIState);	

	INT32			getAIStateCount()										{ return m_pAI->getAIStateCount(); }	
	i3AIState *		getAIState( INT32 index)								{	return m_pAI->getAIState( index);	}

	INT32			SetAIStateImmediately( REAL32 tm);

	void			Reset(void);

	BOOL			StopAI( REAL32 tm);
	
	virtual void	OnChangeTime( REAL32 tm);
	virtual void	OnUpdate( RT_REAL32 tm);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual	void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
