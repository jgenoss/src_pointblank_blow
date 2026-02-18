#pragma once

class i3GuiEditorFramework : public i3Framework
{
	I3_CLASS_DEFINE( i3GuiEditorFramework );

public:
	i3GuiEditorFramework(void);
	virtual ~i3GuiEditorFramework(void);

protected:
	i3Node * m_pCurNode;

public:
	i3Node	*	getCurNode(void)			{ return m_pCurNode;}
	void		setCurNode(i3Node * pNode)	{ m_pCurNode = pNode;}
public:
	void Reset(void);
	void SetSceneGraph( i3Node * pSG );

	virtual void	OnUpdate( REAL32 tm);
};
