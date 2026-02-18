#pragma once

#include "i3NavMeshUtil.h"
#include "i3NavMeshPrimitive.h"

class I3_EXPORT_TDK i3NavMeshPrimitivePolygon : public i3NavMeshPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshPrimitivePolygon, i3NavMeshPrimitive );
	
public:
	enum
	{
		MAX_VERTEXARRAY_SIZE			= 2048 * 10 * 2,
		MAX_INDEXARRAY_SIZE				= 2048 * 10 * 2 * 3,
	};

public:
	void		SetVB( int idx, const VEC3D& v );

	void		ReBuild_VB();
	void		ReBuild_IB();

public:
	void		OnBuildMesh();	
	void		OnRender();

private:
	void		_Render_PolyList( COLORREAL& cr, MATRIX& mat, int nStart, int nPrimCnt, I3G_RENDER_MODE _mode );

private:
	int		m_nNormPrimCnt = 0;
	int		m_nSelPrimCnt = 0;

	bool	m_bHasOverPoly = false;
	bool	m_bSelOverPoly = false;

private:
	COLORREAL		m_crNormal = { 0.0f, 0.75f, 1.00f, 0.20f };		// normal
	COLORREAL		m_crSelect = { 0.5f, 0.75f, 0.25f, 0.43f };		// select
	COLORREAL		m_crLine = { 0.8f, 0.80f, 0.80f, 1.00f };		// line
	COLORREAL		m_crOverNormal = { 0.0f, 0.75f, 1.00f, 0.39f };	// overnormal
	COLORREAL		m_crOverSelect = { 0.5f, 0.75f, 0.25f, 0.60f };	// overselect

	MATRIX		m_matPoly;
	MATRIX		m_matLine;
};