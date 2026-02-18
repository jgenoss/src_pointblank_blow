#if !defined( __I3_SCENE_OBJECT_H)
#define __I3_SCENE_OBJECT_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3Transform2.h"
#include "i3SceneTracer.h"
#include "i3Body.h"

#include "i3Base/itl/function/function.h"

struct TransformNameCompareProc
{
	bool operator()( const i3Transform* p1, const i3Transform* p2 ) const 
	{
		if( p1->hasName() == false) return false;
		if( p2->hasName() == false) return true;
		return p1->GetNameString() < p2->GetNameString();
	}
};

typedef i3::shared_ptr<i3::vector_multiset<i3Transform*, TransformNameCompareProc> >		BoneListPtr;

class I3_EXPORT_SCENE i3SceneObject : public i3Transform
{
	I3SG_NODE_CLASS_DEFINE( i3SceneObject, i3Transform);

protected:
	// Persistant Members
	REAL32	m_timeScale = 1.0f;

	// Volatile Members
	BoneListPtr		m_pBoneList;
	i3Body *		m_pBody = nullptr;
	i3Node *		m_pWorldSpaceSource = nullptr;

	i3Animation *		m_pCurAnim = nullptr;
	I3_ANIM_PLAY_MODE	m_AnimPlayMode = I3_ANIM_PLAY_MODE_REPEAT;
	REAL32				m_ManualClampTime;

	i3::function<void()>	m_PreTraceCallback;

protected:
	void Rec_BuildBoneList( i3Node * pNode);

public:
	i3SceneObject(void);

	REAL32	GetTimeScale(void)							{ return m_timeScale; }
	void	SetTimeScale( REAL32 tm)						{ m_timeScale = tm; }

	void	SetPreTraceCallback(const i3::function<void()>& fn) { m_PreTraceCallback = fn; }
	void	ClearPreTraceCallback() { m_PreTraceCallback.clear(); }

	//Animation 
	void	PlayAnim(  i3Animation * pAnim, 
						I3_ANIM_BLEND_MODE BlendMode = I3_ANIM_BLEND_MODE_REPLACE,
						REAL32 timeStart = 0.0f,
						REAL32 Interval = 0.0f,
						REAL32 timeScale = 1.0f,
						REAL32 rManualClampTime = 1.0f,
						I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT,
						I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP
						);
	i3Animation *		getCurrentAnim(void)				{ return m_pCurAnim; }
	void	RefreshAnim(void);
	void	StopAnim( i3Animation * pAnim );
	void	SetAnimMat( const char * pNodeName, MATRIX * pMat);
	void	SetPlayTime( REAL32 tm);
	UINT32	getAnimPlayCount( INT32 idx)
	{
		if( m_pBody != nullptr)
			return m_pBody->getAnimPlayCount( idx);

		i3Transform * pTrans = GetBone( idx);

		return pTrans->getPlayCount();
	}

	REAL32	getAnimPlayTime( INT32 idx)
	{
		if( m_pBody != nullptr)
			return m_pBody->getPlayTime( idx);

		i3Transform * pTrans = GetBone( idx);

		return pTrans->getPlayTime();
	}

	void	traverse( REAL32 tm, i3SceneTracer * pTracer);

	const BoneListPtr&	getBoneList(void)	const					{ return m_pBoneList; }
	void			setBoneList( const BoneListPtr& pBoneList)		{ m_pBoneList = pBoneList; }

	void			BuildBoneList(void);
	i3Transform *	FindBoneByName( const char * pszBoneName);
	INT32			FindBoneIndexByName( const char * pszBoneName);

	MATRIX *		FindCacheMatrixByName( const char * pszBoneName);

	i3Body *		GetBody( void)						
	{
		return m_pBody;		 		
	}
	void			SetBody( i3Body * pBody)			
	{
		m_pBody = pBody; 
	}				// Internal use Only

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
		if( m_pBoneList == nullptr)return nullptr;
		I3ASSERT( i < (INT32)m_pBoneList->size()); 
		return m_pBoneList->get_vector()[i]; 
	}

	INT32			GetBoneCount(void)					
	{ 
		if( m_pBoneList == nullptr)
			return 0;

		return (INT32)m_pBoneList->size(); 
	}

	void			CalcBoneWorldMatrix( INT32 idxLODBone, MATRIX * pMtx, INT32 lod = -1);

	void			SetDynamicUpdateEnable( bool bFlag);
	void			FindWorldTransformSource(void);
	void			SetPathAnimation( i3Animation * pAnim, I3_ANIM_PLAY_MODE mode);

	virtual void	OnUpdate( i3SceneTracer * pTracer) override;
	virtual void	PreTrace(i3SceneTracer * pTracer) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnInitAfterLoad(void) override;
};

#endif
