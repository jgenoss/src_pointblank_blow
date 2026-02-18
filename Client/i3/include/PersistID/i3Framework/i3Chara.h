
// ============================================================================
//
// i3Chara												by KOMET 2006.02.23
//
// ============================================================================

#ifndef __I3_CHARA_H
#define __I3_CHARA_H

#include "i3GameRes.h"
#include "i3GameResSceneGraph.h"
#include "i3GameResAnimation.h"
#include "i3GameObj.h"
#include "i3AI.h"
#include "i3AIState.h"
#include "i3AIContext.h"

class i3GameResChara;

#define		I3_CHARA_STATE_DISABLE_MOVE				0x00000001
#define		I3_CHARA_STATE_DISABLE_CONTROL			0x00000002


class I3_EXPORT_FRAMEWORK i3Chara : public i3GameObj
{
	I3_CLASS_DEFINE( i3Chara);

protected:
	UINT32					m_CharaState;
	i3SceneGraphInfo *		m_pSgInfo;

	//	AI
	i3List				m_AnimList; //	(i3AI *)

protected:
	void				_Rec_InitAIContext( i3GameNode * pChild);

public:
	UINT32				getCharaState(void)							{ return m_CharaState; }
	void				setCharaState( UINT32 state)				{ m_CharaState = state; }
	void				addCharaState( UINT32 mask)					{ m_CharaState |= mask; }
	void				removeCharaState( UINT32 mask)				{ m_CharaState &= ~mask; }
	bool				isCharaState( UINT32 mask)					{ return (m_CharaState & mask) != 0; }

	i3SceneGraphInfo *	getSceneGraphInfo(void)						{ return m_pSgInfo; }
	void				setSceneGraphInfo( i3SceneGraphInfo * pInfo)
	{
		I3_REF_CHANGE( m_pSgInfo, pInfo);
	}

	void				RemoveAllAnim( void);
	INT32				AddAnim( i3Animation * pAnim);
	void				RemoveAnim( i3Animation * pAnim);
	INT32				FindAnim( i3Animation * pAnim);
	i3Animation *		FindAnimByName( const char * pszName);
	INT32				getAnimCount(void)							{ return m_AnimList.GetCount(); }
	i3Animation *		getAnim( INT32 idx)							{ return (i3Animation *) m_AnimList.Items[idx]; }

	UINT32				getAnimPlayCount( INT32 idxBone)			{ return getSceneObject()->getAnimPlayCount( idxBone); }
	REAL32				getAnimPlayTime( INT32 idxBone)				{ return getSceneObject()->getAnimPlayTime( idxBone); }

public:
	i3Chara( void);
	virtual ~i3Chara( void);

	virtual void		Create( i3Framework * pFramework, i3GameResChara * pRes);
			
	//	Model & Animation
		
	BOOL			PlayAnimation( i3Animation * pAnim, I3_ANIM_BLEND_MODE nMode = I3_ANIM_BLEND_MODE_REPLACE, 
									RT_REAL32 fStart = __RT_0, RT_REAL32 fInterval = __RT_0, RT_REAL32 fTimeScale = __RT_1,
									I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT,
									I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP);

	void			StopAnimation( i3Animation * pAnim);

	void				SetCurAIState( const char * pszAIPath);
	i3AIContext *		FindAI( const char * pszAIPath);

	virtual void	OnUpdate( RT_REAL32 rDeltaSeconds );

	virtual			void	OnChangeAIState( i3AIContext * pCtx);
	
	virtual			void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual			void	OnBuildObjectList( i3List * pList);
	virtual			void	OnInitAfterLoad(void);
	virtual			UINT32	OnSave( i3ResourceFile * pResFile);
	virtual			UINT32	OnLoad( i3ResourceFile * pResFile);

	// Physix functions
	void			PHYSIX_RecreateActors( void );
};

#endif // __I3_CHARA_H