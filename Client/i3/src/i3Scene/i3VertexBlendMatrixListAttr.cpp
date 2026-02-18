#include "i3SceneDef.h"
#include "i3VertexBlendMatrixListAttr.h"

I3_CLASS_INSTANCE( i3VertexBlendMatrixListAttr, i3RenderAttr);

i3VertexBlendMatrixListAttr * g_pLastBlendMatrixList;

i3VertexBlendMatrixListAttr::i3VertexBlendMatrixListAttr(void)
{
	SetID( I3_ATTRID_VERTEXBLENDMATRIXLIST);

	i3Matrix::Identity( &m_SkltMatrix);
	i3Matrix::Identity( &m_InvSkltMatrix);
}

i3VertexBlendMatrixListAttr::~i3VertexBlendMatrixListAttr(void)
{
	RemoveAllMatrix();
}

void i3VertexBlendMatrixListAttr::RemoveAllMatrix(void)
{
	INT32 i;
	i3MatrixObject * pMtxObj;

	for( i = 0; i < m_List.GetCount(); i++)
	{
		pMtxObj = (i3MatrixObject *) m_List.Items[i];

		pMtxObj->Release();
	}
}

BOOL i3VertexBlendMatrixListAttr::Create( INT32 count)
{
	INT32 i;
	i3MatrixObject * pMtx;

	for( i = 0; i < count; i++)
	{
		pMtx = i3MatrixObject::NewObject();

		m_List.Add( pMtx);
	}

	return TRUE;
}

void i3VertexBlendMatrixListAttr::SetMatrixObject( INT32 Idx, i3MatrixObject * pMatrixObject)
{
	i3MatrixObject * pOld;

	pOld = (i3MatrixObject *) m_List.Items[ Idx];

	pOld->Release();

	m_List.Items[Idx] = (void *) pMatrixObject;

	pMatrixObject->AddRef();
}

i3MatrixObject * i3VertexBlendMatrixListAttr::GetMatrixObject( INT32 Idx)
{
	return (i3MatrixObject *) m_List.Items[Idx];
}

void i3VertexBlendMatrixListAttr::Apply( i3RenderContext * pContext)
{
	INT32 i;
	i3MatrixObject * pMtx;

	// 뒤에 나타나는 igGeometryAttr에서 선택된 Blend Matrix Palette를 알 수 있도록
	// 전역 변수를 통해 전달한다.
	g_pLastBlendMatrixList = this;

	// 2003.11.10
	// 이진균
	//
	// 현재로는 100% Software Processing으로 Skinning Animation을 구현하고 있기 때문에
	// Blend Matrix를 설정해야 할 이유가 없다.
	for( i = 0; i < m_List.GetCount(); i++)
	{
		pMtx = (i3MatrixObject *) m_List.Items[i];

		//i3Matrix::Mul( &tmp, &m_SkltMatrix, pMtx->GetMatrix());
		//i3Matrix::Mul( &tmp, &tmp, &m_InvSkltMatrix);
		//pContext->SetVertexBlendMatrix( i, &tmp);

		pContext->SetVertexBlendMatrix( i, pMtx->GetMatrix());
	}
}

void i3VertexBlendMatrixListAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	INT32 i;
	i3VertexBlendMatrixListAttr * pDest = (i3VertexBlendMatrixListAttr *) pObj;

	i3RenderAttr::CopyTo( pObj, method);

	if( method == I3_COPY_REF)
	{
		for( i = 0; i < m_List.GetCount(); i++)
		{
			pDest->SetMatrixObject( i, GetMatrixObject(i));
		}
	}
	else
	{
		pDest->Create( m_List.GetCount());

		for( i = 0; i < m_List.GetCount(); i++)
		{
			i3MatrixObject * pSrcMtx, * pDestMtx;

			pSrcMtx = GetMatrixObject( i);
			pDestMtx = pDest->GetMatrixObject( i);

			i3Matrix::Copy( pDestMtx->GetMatrix(), pSrcMtx->GetMatrix());
		}
	}

	pDest->SetSkeletonMatrix( GetSkeletonMatrix(), GetInverseSkeletonMatrix());
}

BOOL i3VertexBlendMatrixListAttr::IsSame( i3RenderAttr * pAttr)
{
	I3ASSERT( pAttr->GetMeta() == i3VertexBlendMatrixListAttr::GetClassMeta());

	// 비교를 하는 편이 더 느릴 수 있기 때문에, 무조건 다른 Matrix Array라고 판단한다.

	return FALSE;
}

void i3VertexBlendMatrixListAttr::OnBuildObjectList( i3List * pList)
{

	pList->AddFromList( &m_List);

	i3RenderAttr::OnBuildObjectList( pList);
}

UINT32 i3VertexBlendMatrixListAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;
	i3Stream * pStream = pResFile->GetStream();
	INT32 i;

	Result = 0;

	// Count
	{
		UINT32 count = m_List.GetCount();

		Rc = pStream->Write( &count, sizeof(count));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnSave()", "Could not write matrix object count.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Skeleton Matrix
	{
		PERSIST_MATRIX pmtx;

		i3Matrix::Copy( &pmtx, &m_SkltMatrix);

		Rc = pStream->Write( &pmtx, sizeof( PERSIST_MATRIX));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnSave()", "Could not write skeleton matrix.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Inverse Matrix
	{
		PERSIST_MATRIX pmtx;

		i3Matrix::Copy( &pmtx, &m_InvSkltMatrix);

		Rc = pStream->Write( &pmtx, sizeof( PERSIST_MATRIX));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnSave()", "Could not write Inverse skeleton matrix.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( i = 0; i < m_List.GetCount(); i++)
	{
		UINT32 val = (UINT32) m_List.Items[i];

		Rc = pStream->Write( &val, sizeof(val));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnSave()", "Could not write matrix object.");
			return STREAM_ERR;
		}
		Result += Rc;
	}


	return Result;
}

UINT32 i3VertexBlendMatrixListAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3VertexBlendMatrixListAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Result, Rc;
	i3Stream * pStream = pResFile->GetStream();
	INT32 i;
	UINT32 count;

	Result = 0;

	// Count
	{
		Rc = pStream->Read( &count, sizeof(count));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnLoad()", "Could not read matrix object count.");
			return STREAM_ERR;
		}
		Result += Rc;

		Create( count);
	}

	// Skeleton Matrix
	{
		PERSIST_MATRIX pmtx;

		Rc = pStream->Read( &pmtx, sizeof(PERSIST_MATRIX));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnLoad()", "Could not read skeleton matrix.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Matrix::Copy( &m_SkltMatrix, &pmtx);
	}

	// Inverse Skeleton Matrix
	{
		PERSIST_MATRIX pmtx;

		Rc = pStream->Read( &pmtx, sizeof(PERSIST_MATRIX));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnLoad()", "Could not read skeleton matrix.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Matrix::Copy( &m_InvSkltMatrix, &pmtx);
	}

	for( i = 0; i < (INT32) count; i++)
	{
		UINT32 val;
		i3MatrixObject * pMtx;

		Rc = pStream->Read( &val, sizeof(val));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3VertexBlendMatrixListAttr::OnLoad()", "Could not read matrix object.");
			return STREAM_ERR;
		}
		Result += Rc;

		pMtx = (i3MatrixObject *) pResFile->FindObjectByID( val);

		SetMatrixObject( i, pMtx);
	}


	return Result;
}
