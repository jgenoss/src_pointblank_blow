#if !defined( __I3_SKIN_H)
#define __I3_SKIN_H

#include "i3Skeleton.h"
#include "i3Bone.h"
#include "i3Shape.h"
#include "i3Node.h"
#include "i3BoneMatrixListAttr.h"
#include "i3Animation.h"

class i3SceneTracer;

class I3_EXPORT_SCENE i3Skin : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Skin);

protected:
	// Persistent Members
	i3List			m_SkelList;
	i3List			m_ShapeList;

	REAL32			m_Slope;
	REAL32			m_Const;

	INT32			m_CurLevel;

	i3Animation *	m_pCurAnim;

	// Volatile Members
	i3List				m_CombinerList;

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo);

	void		_PrepareCombiner(void);
	void		_RemoveAllCombiner(void);

public:
	i3Skin(void);
	virtual ~i3Skin(void);

	void				AddSkeleton( i3Skeleton * pSkel);
	void				RemoveSkeleton( i3Skeleton * pSkel);
	void				RemoveAllSkeleton(void);
	UINT32				getSkeletonCount(void)					{ return (UINT32) m_SkelList.GetCount(); }
	i3Skeleton *		getSkeleton( UINT32 idx)				{ return (i3Skeleton *) m_SkelList.Items[idx]; }

	void				AddShape( i3Shape * pShape);
	void				RemoveShape( i3Shape * pShape);
	void				RemoveAllShape(void);
	UINT32				getShapeCount(void)						{ return (UINT32) m_ShapeList.GetCount(); }
	i3Shape *			getShape( UINT32 idx)					{ return (i3Shape *) m_ShapeList.Items[idx]; }

	// LOD Level = Slope * (d^2) + Const
	REAL32				getSlope(void)									{ return m_Slope; }
	void				setSlope( REAL32 slope)							{ m_Slope = slope; }

	REAL32				getConst(void)									{ return m_Const; }
	void				setConst( REAL32 cst)							{ m_Const = cst; }

	INT32				getCurrentLevel(void)							{ return m_CurLevel; }
	void				setCurrentLevel( INT32 l)						{ m_CurLevel = l; }
	void				CalcLOD(void);
	i3Skeleton *		getCurrentSkeleton(void)						{ return (i3Skeleton *) m_SkelList.Items[ m_CurLevel]; }
	i3Shape *			getCurrentShape(void)							{ return (i3Shape *) m_ShapeList.Items[ m_CurLevel]; }

	void				PlayAnim( i3Animation * pAnim, 
								RT_REAL32 rStartTime = __RT_0,
								RT_REAL32 interval = __RT_0, 
								RT_REAL32 timeScale = __RT_1,
								I3_ANIM_BLEND_MODE blendMode = I3_ANIM_BLEND_MODE_REPLACE, 
								I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT, 
								I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP );
	void				StopAnim(void);
	void				SetPlayAnim( RT_REAL32 tm);
	RT_REAL32			GetDuration(void);
	i3Animation *		GetCurrentAnim(void)							{ return m_pCurAnim; }

	void				Traverse( i3SceneTracer * pTracer);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif

