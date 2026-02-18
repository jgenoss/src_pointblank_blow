#if !defined( __I3_SCENE_OPTIMIZER_H)
#define __I3_SCENE_OPTIMIZER_H

#include "i3Base.h"
#include "i3Scene.h"

enum OPT_PHASE
{
	PHASE_NA = -1,
	PHASE_COMMON_SG,
	PHASE_INSTANCE_SG,
};

#define OPT_STYLE_APPLY_NODE		0x00000001
#define OPT_STYLE_APPLY_SG			0x00000002

class I3_EXPORT_OPT i3SceneOptimizer : public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( i3SceneOptimizer);
protected:
	UINT32					m_Style;
	OPT_PHASE				m_Phase;
	i3SceneGraphInfo *		m_pSg;
	i3Node *				m_pTraceRoot;

	i3MatrixStack *		m_pStack;
	BOOL				m_bMatrixStackEnable;

protected:
	virtual void		Rec_Trace( i3Node * pNode);
	virtual BOOL		OnPreNode( i3Node * pNode);
	virtual BOOL		OnNode( i3Node * pNode);
	virtual BOOL		OnPostNode( i3Node * pNode);

public:
	i3SceneOptimizer(void);
	virtual ~i3SceneOptimizer(void);

	UINT32				getStyle(void)				{ return m_Style; }
	void				setStyle( UINT32 mask)		{ m_Style = mask; }
	void				addStyle( UINT32 mask)		{ m_Style |= mask; }
	void				removeStyle( UINT32 mask)	{ m_Style &= ~mask; }
	bool				isStyle( UINT32 mask)		{ return (m_Style & mask) != 0; }

	i3SceneGraphInfo *	getSg(void)				{ return m_pSg; }
	void				setSg( i3SceneGraphInfo * pSg)
	{
		I3_REF_CHANGE( m_pSg, pSg);
	}

	i3Node *			getTraceRoot(void)			{ return m_pTraceRoot; }
	void				SetTraceRoot( i3Node * pNewRoot);

	BOOL				GetMatrixStackEnable(void)		{ return m_bMatrixStackEnable; }
	void				SetMatrixStackEnable( BOOL bFlag);

	virtual const char *	getDesc(void);

	virtual void		Trace( i3Node * pRoot);
	virtual void		Optimize( i3SceneGraphInfo * pSg);

	//	i3Optimizer 툴에서 ini 파일을 파싱한 Label 이름과 값을 각각 char 형으로 넘겨줍니다.
	//	각 i3Opt 클래스에서는 Trace가 실행되므로 값의 설정이 필요한 경우 이 함수를 사용하여 적절히 적용시켜주세요. by KOMET
	virtual void SetProperty( char * pszFieldName, char * pszValue);

	void	CallOptimizer( i3ClassMeta * pMeta);
};

#endif