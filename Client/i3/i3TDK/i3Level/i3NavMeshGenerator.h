#pragma once

#include "Recast/Recast.h"
#include "i3NavMeshUtil.h"

class I3_EXPORT_TDK i3NavMeshGenerator : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshGenerator, i3ElementBase );

public:
	i3NavMeshGenerator();
	~i3NavMeshGenerator();	

public:
	rcPolyMesh*			GetPolyMesh()			{ return m_pmesh; }
	rcPolyMeshDetail*	GetPolyDetailMesh()		{ return m_dmesh; }

public:
	bool		DoGenerate( i3World* world, const navMeshConfig& cfg, fnAutoGenLoadingRatio pFn = nullptr );

private:	
	bool		_DoGenerate( const navMeshConfig& cfg, fnAutoGenLoadingRatio pFn );

	bool		_CalculateVertexList_byGeometry();
	bool		_CalculateVertexList_byTransform2();

	void		_CalculateWTM( i3Node* node, MATRIX& wmat );
	void		_CleanUp();

private:
	i3World*	m_world = nullptr;

private:
	float*		m_verts = nullptr;
	int*		m_tris = nullptr;

	int			m_nvert = 0;
	int			m_ntri = 0;

	D3DXVECTOR3			m_min = { 0, 0, 0 };
	D3DXVECTOR3			m_max = { 0, 0, 0 };

private:
	struct _VA_info
	{
		int		nVertCnt;
		int		nOffset;

		bool	bLoaded;
	};

	i3::map< i3VertexArray*, _VA_info >	 m_mapVA;

private:
	class logContext;

	// recast
private:
	logContext*		m_ctx;							// [initialize at constructor]

	//
	rcConfig				m_cfg;
	unsigned char*			m_triareas = nullptr;
	rcHeightfield*			m_solid = nullptr;
	rcCompactHeightfield*	m_chf = nullptr;
	rcContourSet*			m_cset = nullptr;

	rcPolyMesh*				m_pmesh = nullptr;
	rcPolyMeshDetail*		m_dmesh = nullptr;

	bool	m_keepInterResults = true;


private:
	class logContext : public rcContext
	{
		virtual void	doLog( const rcLogCategory category, const char* msg, const int len )  { category, msg, len; }
	};
};