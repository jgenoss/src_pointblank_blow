#include "i3SceneDef.h"
#include "i3RegData.h"
#include "i3RegINT32.h"
#include "i3RegREAL32.h"
#include "i3RegSTRING.h"
#include "i3RegVEC2D.h"
#include "i3RegVEC3D.h"
#include "i3RegVEC4D.h"
#include "i3RegCOLOR.h"
#include "i3RegMATRIX.h"

I3_CLASS_INSTANCE( i3RegData);

void i3RegData::GetValue( INT32 * nValue)
{
	if( m_nDataType == I3REG_TYPE_INT32)
	{
		*nValue = *(((i3RegINT32*)this)->getValue());
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
		*nValue = 0;
	}
}

void i3RegData::GetValue( REAL32 * nValue)
{
	
	if( m_nDataType == I3REG_TYPE_REAL32)
	{
		*nValue = *(((i3RegREAL32*)this)->getValue());
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
		*nValue = 0.0f;
	}

}

void i3RegData::GetValue( i3::rc_wstring& outString)
{
	
	if( m_nDataType == I3REG_TYPE_STRING)
	{
		outString = ((i3RegString*)this)->getValue();
	}
	else
	{
		outString.clear();
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
	}
}

void i3RegData::GetValue( VEC2D * nValue)
{
	
	if( m_nDataType == I3REG_TYPE_VEC2D)
	{
		i3Vector::Copy( nValue, ((i3RegVEC2D*)this)->getValue());
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
		i3Vector::Zero( nValue);
	}
}

void i3RegData::GetValue( VEC3D * nValue)
{
		if( m_nDataType == I3REG_TYPE_VEC3D)
	{
		i3Vector::Copy( nValue, ((i3RegVEC3D*)this)->getValue());
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
		i3Vector::Zero( nValue);
	}
}

void i3RegData::GetValue( VEC4D * nValue)
{
	
	if( m_nDataType == I3REG_TYPE_VEC4D)
	{
		i3Vector::Copy( nValue, ((i3RegVEC4D*)this)->getValue());
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
		i3Vector::Zero( nValue);
	}
}

void i3RegData::GetValue( I3COLOR * pCol)
{
	
	if( m_nDataType == I3REG_TYPE_COLOR)
	{
		i3Color::Copy( pCol, ((i3RegCOLOR*)this)->getValue());
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
		i3Color::Set( pCol, (UINT8) 0,0,0,0);
	}
}

void i3RegData::GetValue( MATRIX * pMtx)
{
	
	if( m_nDataType == I3REG_TYPE_MATRIX)
	{
		i3Matrix::Copy( pMtx, ((i3RegMATRIX*)this)->getValue());
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "속성값의 종류가 맞지 않습니다.");
		i3Matrix::Identity( pMtx);
	}
}

void	i3RegData::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3NamedElement::OnBuildObjectList( List);
}

UINT32	i3RegData::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	UINT32 data;
	i3Stream * pStream	= pResFile->GetStream();

	Result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( Result);

	data = m_nDataType;

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegData::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	UINT32 data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_nDataType = data;

	return Result;
}
