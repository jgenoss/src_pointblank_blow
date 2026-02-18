#if !defined( __I3_OPT_SCENE_DUMP_H)
#define __I3_OPT_SCENE_DUMP_H

#include "i3SceneOptimizer.h"

class i3OptSceneDump : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptSceneDump);
protected:
	char	m_szPath[MAX_PATH];
	char	m_szName[MAX_PATH];

	i3FileStream *		m_pStream;

protected:
	void	i3TransformDump( i3Transform * pTrans);
	void	i3GeometryDump( i3Geometry * pGeo);
	void	i3NodeDump( i3Node * pNode);
	void	DumpHeader( i3Node * pRoot);

public:
	i3OptSceneDump(void);
	virtual ~i3OptSceneDump(void);

	void	SetPath( char * pszPath)		{ i3String::Copy( m_szPath, pszPath); }

	virtual BOOL OnNode( i3Node * pNode);
	virtual void Trace( i3Node * pRoot);
};

#endif

