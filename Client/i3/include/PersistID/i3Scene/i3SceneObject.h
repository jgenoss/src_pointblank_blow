#if !defined( __I3_SCENE_OBJECT_H)
#define __I3_SCENE_OBJECT_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3Animation.h"
#include "i3AnimationState.h"
#include "i3Transform2.h"
#include "i3SceneTracer.h"
#include "i3Body.h"

class I3_EXPORT_SCENE i3SceneObject : public i3Transform
{
	I3SG_NODE_CLASS_DEFINE( i3SceneObject);

protected:
	// Persistant Members
	REAL32	m_timeScale;

	// Volatile Members
	i3BinList *		m_pBoneList;
	i3Body *		m_pBody;
	i3Node *		m_pWorldSpaceSource;

	i3Animation *		m_pCurAnim;
	I3_ANIM_PLAY_MODE	m_AnimPlayMode;

protected:
	void Rec_BuildBoneList( i3Node * pNode);

	//void	AddAnimState( i3AnimationState * pAnimState);
	//void	ApplyAnimState( i3AnimationState * pAnimState);
	//INT32	FindPlayAnim( i3Animation * pAnim);
public:
	i3SceneObject(void);
	virtual ~i3SceneObject(void);

	REAL32	GetTimeScale(void)							{ return m_timeScale; }
	void	SetTimeScale( REAL32 tm)						{ m_timeScale = tm; }

	//Animation 
	void	PlayAnim(  i3Animation * pAnim, 
						I3_ANIM_BLEND_MODE BlendMode = I3_ANIM_BLEND_MODE_REPLACE,
						RT_REAL32 timeStart = __RT_0,
						RT_REAL32 Interval = __RT_0,
						RT_REAL32 timeScale = __RT_ONE,
						I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT,
						I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP
						);
	i3Animation *		getCurrentAnim(void)				{ return m_pCurAnim; }
	void	RefreshAnim(void);
	void	StopAnim( i3Animation * pAnim );
	void	SetAnimMat( const char * pNodeName, RT_MATRIX * pMat);
	void	SetPlayTime( RT_REAL32 tm);
	UINT32	getAnimPlayCount( INT32 idx)
	{
		if( m_pBody != NULL)
			return m_pBody->getAnimPlayCount( idx);

		i3Transform * pTrans = GetBone( idx);

		return pTrans->getPlayCount();
	}

	REAL32	getAnimPlayTime( INT32 idx)
	{
		if( m_pBody != NULL)
			return m_pBody->getPlayTime( idx);

		i3Transform * pTrans = GetBone( idx);

		return pTrans->getPlayTime();
	}

	void	traverse( REAL32 tm, i3SceneTracer * pTracer);

	i3BinList *		getBoneList(void)						{ return m_pBoneList; }
	void			setBoneList( i3BinList * pBoneList)		{ I3_REF_CHANGE( m_pBoneList, pBoneList); }

	void			BuildBoneList(void);
	i3Transform *	FindBoneByName( const char * pszBoneName);
	INT32			FindBoneIndexByName( const char * pszBoneName);

	MATRIX *		FindCacheMatrixByName( const char * pszBoneName);

	i3Body *		GetBody( void)						{ return m_pBody;		 		}
	void			SetBody( i3Body * pBody)			{ m_pBody = pBody; }				// Internal use Only

	i3Node *		getWorldSpaceSource( void )			{ return m_pWorldSpaceSource; }

	void			GetWorldSpaceSourceMatrix( MATRIX * pMtx);
	void			UpdateLastWorldTransform( MATRIX * pMatrix);

	//
	// pszBoneName으로 지정된 Bone을 검색해, pAttachNode를 연결할 지점을 준비한다.
	// 만약 일반적인 Sg라면 단순히 검색된 i3Transform Node에 Child로 연결하는 것이
	// 모두이겠으나, i3Body 구조를 사용하는 Sg의 경우라면, i3BoneRef node를
	// 생성하는 등의 준비 작업을 한다.
	i3Node *		AttachNodeToBone( const char * pszBoneName, i3Node * pAttachNode);

	i3Transform *	GetBone( INT32 i)					
	{ 
		if( m_pBoneList == NULL)return NULL;
		I3ASSERT( i < m_pBoneList->GetCount()); 
		return (i3Transform *) m_pBoneList->Items[i]; 
	}

	INT32			GetBoneCount(void)					
	{ 
		if( m_pBoneList == NULL)
			return 0;

		return m_pBoneList->GetCount(); 
	}

	void			CalcBoneWorldMatrix( INT32 idxLODBone, MATRIX * pMtx, INT32 lod = -1);

	void			SetDynamicUpdateEnable( bool bFlag);
	void			FindWorldTransformSource(void);
	void			SetPathAnimation( i3TransformSequence * pSeq, I3_ANIM_PLAY_MODE mode);

	virtual void	OnUpdate( i3SceneTracer * pTracer);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnInitAfterLoad(void);
};

#endif
