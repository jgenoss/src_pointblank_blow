#if !defined( __I3_EFFECT_PROJECT_H)
#define __I3_EFFECT_PROJECT_H

class i3EffectProject : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3EffectProject, i3PersistantElement);
protected:
	i3Node *		m_pWorld;
	i3Node *		m_pSky;
	i3Node *		m_pModel;
	i3::vector<i3TimeSequence*>	m_TimeSeqList;

protected:
	void			RemoveAllTimeSeq(void);

public:
	i3EffectProject(void);
	virtual ~i3EffectProject(void);

	void				SetWorld( i3Node * pWorld);
	i3Node *			GetWorld(void)					{ return m_pWorld; }

	void				SetSky( i3Node * pSky);
	i3Node *			GetSky(void)					{ return m_pSky; }

	void				SetModel( i3Node * pNode);
	i3Node *			GetModel(void)					{ return m_pModel; }

	void				AddTimeSeq( i3TimeSequence * pTimeSeq);
	INT32				GetTimeSeqCount(void)			{ return (INT32)m_TimeSeqList.size(); }
	i3TimeSequence *	GetTimeSeq( INT32 idx)			{ return m_TimeSeqList[idx]; }

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif