
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

//------------------------------------------------------------
typedef UINT32 I3_CHARA_STATE;
#define		I3_CHARA_STATE_DISABLE_MOVE				0x00000001
#define		I3_CHARA_STATE_DISABLE_CONTROL			0x00000002
#define		I3_CHARA_STATE_FLYING					0x00000004
#define		I3_CHARA_STATE_FALL						0x00000008
#define		I3_CHARA_STATE_TOUCHWALL				0x00000010

#define		I3_CHARA_STATE_TURN_THETA				0x00000020
#define		I3_CHARA_STATE_TURN_RHO					0x00000040
//#define		I3_CHARA_STATE_MOVE_RHO					0x00000080
//------------------------------------------------------------
typedef UINT32 I3_CHARA_STYLE;
#define		I3_CHARA_STYLE_FLY						0x00000100
#define		I3_CHARA_STYLE_JUMP						0x00000200
#define		I3_CHARA_STYLE_HUMAN					0x10000000



class I3_EXPORT_FRAMEWORK i3Chara : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3Chara, i3GameObj);

protected:
	I3_CHARA_STATE			m_CharaState = 0;
	I3_CHARA_STYLE			m_CharaStyle = 0;
	i3SceneGraphInfo *		m_pSgInfo = nullptr;


protected:
	void				_Rec_InitAIContext( i3GameNode * pChild);

public:
	I3_CHARA_STATE		getCharaState(void)									{ return m_CharaState; }
	void				setCharaState( I3_CHARA_STATE state)				{ m_CharaState = state; }
	void				addCharaState( I3_CHARA_STATE mask)					{ m_CharaState |= mask; }
	void				removeCharaState( I3_CHARA_STATE mask)				{ m_CharaState &= ~mask; }
	bool				isCharaState( I3_CHARA_STATE mask)					{ return (m_CharaState & mask) != 0; }

	I3_CHARA_STYLE		getCharaStyle(void)						{ return m_CharaStyle; }
	void				setCharaStyle( I3_CHARA_STYLE state)	{ m_CharaStyle = state; }
	void				addCharaStyle( I3_CHARA_STYLE mask)		{ m_CharaStyle |= mask; }
	void				removeCharaStyle( I3_CHARA_STYLE mask)	{ m_CharaStyle &= ~mask; }
	bool				isCharaStyle( I3_CHARA_STYLE mask)		{ return (m_CharaStyle & mask) != 0; }

	i3SceneGraphInfo *	getSceneGraphInfo(void)						{ return m_pSgInfo; }
	void				setSceneGraphInfo( i3SceneGraphInfo * pInfo)
	{
		I3_REF_CHANGE( m_pSgInfo, pInfo);
	}

	UINT32				getAnimPlayCount( INT32 idxBone)			{ return getSceneObject()->getAnimPlayCount( idxBone); }
	REAL32				getAnimPlayTime( INT32 idxBone)				{ return getSceneObject()->getAnimPlayTime( idxBone); }
	
	bool				canFly(void)					{ return isCharaStyle( I3_CHARA_STYLE_FLY); }
	void				SetCanFly( bool bState)
	{
		if( bState)
			addCharaStyle( I3_CHARA_STYLE_FLY);
		else
			removeCharaStyle( I3_CHARA_STYLE_FLY);
	}

public:
	virtual ~i3Chara( void);

	//CHAR_SOLO_RENDER 변수 추가함..
	virtual void		Create( i3Framework * pFramework, i3GameResChara * pRes, int nPVType = -1);	//nPVType = CHARA_VIEW_TYPE
	void				Create( i3Node * pSg);			// 사용되는 함수임. 삭제하지 마세요. (xgkick)
			
	//	Model & Animation
		
	bool			PlayAnimation( i3Animation * pAnim, I3_ANIM_BLEND_MODE nMode = I3_ANIM_BLEND_MODE_REPLACE, 
									REAL32 fStart = 0.0f, REAL32 fInterval = 0.0f, REAL32 fTimeScale = 1.0f, REAL32 rManualClampTime = 1.0f,
									I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT,
									I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP);

	void			StopAnimation( i3Animation * pAnim);

	void				SetCurAIState( const char * pszAIPath);
	i3AIContext *		FindAI( const char * pszAIPath);

	virtual void	ReturnInstance( void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;

	virtual			void	OnChangeAIState( i3AIContext * pCtx);
	
	virtual			void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual			void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual			void	OnInitAfterLoad(void) override;
	virtual			UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual			UINT32	OnLoad( i3ResourceFile * pResFile) override;

	/********************************************************************************/
	/*								Debug functions									*/
	/********************************************************************************/

#if defined( I3_DEBUG)
protected:
	i3AttrSet *	m_pRenderableBone = nullptr;
	INT32		m_iRenderableBoneCount = 0;
	bool		m_bEnableRenderBone = false;

	i3::vector<i3Node*> m_RenderableTransformList;

	void		_CreateRenderableTransform( i3Transform * pAttachNode);
	void		_FindAndCreateRenderableBone( i3Node * pRoot);
	void		CreateRenderableBoneRef( void);

	void		CreateRenderableBone( void);
	void		CreateRenderDiamond( i3Node * pAttachNode, VEC3D * pDir, bool bLeaf);

public:
	void		EnableRenderBone( bool bEnable);

	void		ToggleRenderBone( void)
	{
		m_bEnableRenderBone = !m_bEnableRenderBone;

		EnableRenderBone( m_bEnableRenderBone);
	}
#endif

protected:
	//CHAR_SOLO_RENDER
	int						m_nPVType = -1;	//CHARA_VIEW_TYPE

public:
	int GetPVType() const { return m_nPVType; }
};

#endif // __I3_CHARA_H