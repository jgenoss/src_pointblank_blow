#pragma once

#include "RSCGenDefine.h"
//#include "GlobalVar.h"
#include "RSCNode.h"

class i3RSCGenProj : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3RSCGenProj, i3PersistantElement);

public:
	i3RSCGenProj(void);
	virtual ~i3RSCGenProj(void);

protected:
	RSCNode *	m_pRoot;

public:
	void		SetRoot( RSCNode * pRoot);
	RSCNode	*	GetRoot()	{	return m_pRoot;}

	virtual void OnBuildObjectList(i3::vector<i3PersistantElement*>& List);
	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};
