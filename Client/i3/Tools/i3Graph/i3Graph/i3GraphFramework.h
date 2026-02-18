#if !defined( __I3_GRAPH_FRAMEWORK_H)
#define __I3_GRAPH_FRAMEWORK_H

class i3GraphFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3GraphFramework);
protected:
	i3Transform *			m_pSelBoundRoot;
	i3ReflectMapBindAttr *	m_pReflectMapAttr;

	i3Transform *			m_pBSPPlane;

	void	_GetFullTransform( i3Node * pNode, MATRIX * pMtx);
public:
	i3GraphFramework(void);
	virtual ~i3GraphFramework(void);

	void			SelectNode( i3Node * pNode);

	void			SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize);

	virtual void	OnCreate(void);
};

#endif
