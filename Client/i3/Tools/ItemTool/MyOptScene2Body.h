#pragma once

class cMyOptScene2Body : public i3OptScene2Body
{
public:
	cMyOptScene2Body(void);
	virtual ~cMyOptScene2Body(void);

	i3::vector<i3Node*> TransList;
	i3Node  * pClone;

	typedef std::vector<i3Node* > vTmpboneref;
	std::map<std::string, vTmpboneref> m_mTmpBone;
	int m_nCnt;

	virtual void _Scene2Body( i3Node * pNode, i3Body * pBody, i3LOD * pLOD);
	i3Node *	_My_Rec_RemoveTransform( i3Node*& pNode, i3LOD * pLOD);
	
};
