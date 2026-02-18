#if !defined( __I3_SKIN_H)
#define __I3_SKIN_H

#include "i3Skeleton.h"
#include "i3Bone.h"
#include "i3Shape.h"
#include "i3Node.h"

class i3SceneTracer;

class I3_EXPORT_SCENE i3Skin : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Skin, i3Node);

protected:
	// Persistent Members
	i3::vector<i3Skeleton*> m_SkelList;
	i3::vector<i3Shape*>	m_ShapeList;

	REAL32			m_Slope = 0.0f;
	REAL32			m_Const = 0.0f;

	INT32			m_CurLevel = 0;

	i3Animation *	m_pCurAnim = nullptr;

	// Volatile Members
	i3::vector<i3TransformSourceCombiner*>	m_CombinerList;

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo) override;

	void		_PrepareCombiner(void);
	void		_RemoveAllCombiner(void);

public:
	i3Skin(void);
	virtual ~i3Skin(void);

	void				AddSkeleton( i3Skeleton * pSkel);
	void				RemoveSkeleton( i3Skeleton * pSkel);
	void				RemoveAllSkeleton(void);
	UINT32				getSkeletonCount(void)					{ return (UINT32) m_SkelList.size(); }
	i3Skeleton *		getSkeleton( UINT32 idx)				{ return m_SkelList[idx]; }

	void				AddShape( i3Shape * pShape);
	void				RemoveShape( i3Shape * pShape);
	void				RemoveAllShape(void);
	UINT32				getShapeCount(void)						{ return (UINT32) m_ShapeList.size(); }
	i3Shape *			getShape( UINT32 idx)					{ return m_ShapeList[idx]; }

	// LOD Level = Slope * (d^2) + Const
	REAL32				getSlope(void)									{ return m_Slope; }
	void				setSlope( REAL32 slope)							{ m_Slope = slope; }

	REAL32				getConst(void)									{ return m_Const; }
	void				setConst( REAL32 cst)							{ m_Const = cst; }

	INT32				getCurrentLevel(void)							{ return m_CurLevel; }
	void				setCurrentLevel( INT32 l)						{ m_CurLevel = l; }
	void				CalcLOD(void);
	i3Skeleton *		getCurrentSkeleton(void)						{ return m_SkelList[ m_CurLevel]; }
	i3Shape *			getCurrentShape(void)							{ return m_ShapeList[ m_CurLevel]; }

	void				PlayAnim( i3Animation * pAnim, 
								REAL32 rStartTime = 0.0f,
								REAL32 interval = 0.0f, 
								REAL32 timeScale = 1.0f,
								REAL32 rManualClampTime = 1.0f,
								I3_ANIM_BLEND_MODE blendMode = I3_ANIM_BLEND_MODE_REPLACE, 
								I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT, 
								I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP );
	void				StopAnim(void);
	void				SetPlayAnim( REAL32 tm);
	REAL32				GetDuration(void);
	i3Animation *		GetCurrentAnim(void)							{ return m_pCurAnim; }

	void				Traverse( i3SceneTracer * pTracer);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

};

#endif

