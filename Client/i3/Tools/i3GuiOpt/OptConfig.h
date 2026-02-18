#if !defined( __OPT_CONFIG_H)
#define __OPT_CONFIG_H

class COptConfig : public i3ResourceObject
{
	I3_CLASS_DEFINE( COptConfig, i3ResourceObject);
protected:
	i3::vector<i3SceneOptimizer*>		m_OptList;

public:
	COptConfig(void);
	virtual ~COptConfig(void);

	INT32				getCount(void)				{ return (INT32)m_OptList.size(); }
	i3SceneOptimizer *	getOpt( INT32 idx)			{ I3_BOUNDCHK( idx, getCount());	return m_OptList[idx]; }
	void				addOpt( i3SceneOptimizer * pOpt);
	void				removeOpt( i3SceneOptimizer * pOpt);
	void				removeAllOpt(void);
	INT32				FindOpt( i3SceneOptimizer * pOpt)		{ return i3::vu::int_index_of(m_OptList, pOpt); }

	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override final;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override final;
};

#endif
