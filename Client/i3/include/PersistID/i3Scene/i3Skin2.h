#if !defined( __I3_SKIN2_H)
#define __I3_SKIN2_H

#include "i3Skeleton.h"
#include "i3Shape.h"
#include "i3SkinLOD.h"
#include "i3Animation.h"

#define		I3SG_LOD_AUTO			0x00000001

class I3_EXPORT_SCENE i3Skin2 : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Skin2);
protected:
	i3List			m_LODList;
	UINT32			m_Style;
	
	REAL32			m_Slope;
	REAL32			m_Const;

	INT32			m_CurLevel;

	i3Animation *	m_pCurAnim;

	// Volatile Members
	i3List				m_CombinerList;
protected:
	virtual void		_GetWrappingBound( i3MatrixStack * pStack, i3BoundBox * pBox);

	void				_PrepareCombiner(void);
	void				_RemoveAllCombiner(void);
	void				_Rec_AdjustBoneRef( i3Node * pNode, i3BoneMatrixListAttr * pAttr);

public:
	i3Skin2(void);
	virtual ~i3Skin2(void);

	UINT32				getStyle(void)						{ return m_Style; }
	void				setStyle( UINT32 style)				{ m_Style = style; }
	void				addStyle( UINT32 style)				{ m_Style |= style; }
	void				removeStyle( UINT32 style)			{ m_Style &= ~style; }

	void				AddLOD( i3SkinLOD * pLOD);
	void				RemoveLOD( i3SkinLOD * pSkel);
	void				RemoveAllLOD(void);
	UINT32				getLODCount(void)					{ return (UINT32) m_LODList.GetCount(); }
	i3SkinLOD *			getLOD( UINT32 idx)					{ return (i3SkinLOD *) m_LODList.Items[idx]; }

	// LOD Level = Slope * (d^2) + Const
	REAL32				getSlope(void)									{ return m_Slope; }
	void				setSlope( REAL32 slope)							{ m_Slope = slope; }

	REAL32				getConst(void)									{ return m_Const; }
	void				setConst( REAL32 cst)							{ m_Const = cst; }

	INT32				getCurrentLevel(void)							{ return m_CurLevel; }
	void				setCurrentLevel( INT32 l)						{ m_CurLevel = l; }
	void				CalcLOD( i3SceneTracer * pTracer);
	i3SkinLOD *			getCurrentLOD(void)								{ return (i3SkinLOD *) m_LODList.Items[ m_CurLevel]; }

	void				PlayAnim( i3Animation * pAnim, 
								RT_REAL32 rStartTime = __RT_0,
								RT_REAL32 interval = __RT_0, 
								I3_ANIM_BLEND_MODE blendMode = I3_ANIM_BLEND_MODE_REPLACE, 
								I3_ANIM_PLAY_MODE destPlayMode = I3_ANIM_PLAY_MODE_REPEAT, 
								I3_ANIM_PLAY_MODE srcPlayMode = I3_ANIM_PLAY_MODE_STOP );
	void				StopAnim(void);
	void				SetPlayAnim( RT_REAL32 tm);
	RT_REAL32			GetDuration(void);
	i3Animation *		GetCurrentAnim(void)							{ return m_pCurAnim; }

	void				Traverse( i3SceneTracer * pTracer);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnChangeTime( REAL32 tm);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual void	OnInitAfterLoad(void);

};

#endif
