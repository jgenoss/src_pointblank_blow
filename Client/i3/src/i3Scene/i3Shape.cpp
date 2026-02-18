#include "i3SceneDef.h"
#include "i3Shape.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3Shape);

i3Shape::~i3Shape(void)
{
	RemoveAllGeometryAttr();
}

void i3Shape::AddGeometryAttr( i3GeometryAttr * pAttr)
{
	I3_MUST_ADDREF( pAttr);
	m_GeoList.push_back( pAttr);
}

void i3Shape::RemoveGeometryAttr( i3GeometryAttr * pAttr)
{
	I3ASSERT( pAttr != nullptr);

	size_t idx = i3::vu::index_of(m_GeoList, pAttr);
	I3ASSERT( idx != m_GeoList.size());

	I3_MUST_RELEASE( pAttr);

	m_GeoList.erase(m_GeoList.begin() + idx);
}

void i3Shape::RemoveAllGeometryAttr(void)
{
	INT32 i;
	i3GeometryAttr * pAttr;

	for( i = 0; i < getGeometryAttrCount(); i++)
	{
		pAttr = getGeometryAttr( i);

		I3_MUST_RELEASE( pAttr);
	}

	m_GeoList.clear();
}

void i3Shape::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3GeometryAttr * pAttr;

	for( i = 0; i < getGeometryAttrCount(); i++)
	{
		pAttr = getGeometryAttr( i);

		pAttr->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 SHAPE
	{
		UINT32		m_GeoCount = 0;
		UINT8		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3Shape::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SHAPE data;
	INT32 i;
	OBJREF ref;
	
	data.m_GeoCount = m_GeoList.size();

	Rc = pStream->Write( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write shape information.");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < getGeometryAttrCount(); i++)
	{
		ref = (OBJREF) pResFile->GetObjectPersistID( getGeometryAttr( i));

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Geometry Reference data I/O error");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3Shape::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SHAPE data;
	INT32 i;
	OBJREF ref;

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read shape information.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( data.m_GeoCount > 0)
	{
		m_GeoList.resize( data.m_GeoCount);

		for( i = 0; i < (INT32) data.m_GeoCount; i++)
		{
			i3GeometryAttr * pAttr;

			Rc = pStream->Read( &ref, sizeof(OBJREF));
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Geometry Reference data I/O error");
				return STREAM_ERR;
			}
			Result += Rc;

			pAttr = (i3GeometryAttr *) pResFile->FindObjectByID( ref);
			I3_MUST_ADDREF( pAttr);
			m_GeoList[i] = pAttr;
		}
	}

	return Result;
}
