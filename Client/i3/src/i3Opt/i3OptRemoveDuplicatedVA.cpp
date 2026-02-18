#include "i3OptPCH.h"
#include "i3OptRemoveDuplicatedVA.h"

I3_CLASS_INSTANCE( i3OptRemoveDuplicatedVA);

void i3OptRemoveDuplicatedVA::Trace( i3Node * pRoot)
{
	VAINFO * pInfo;

	GatherAllVertexArray( pRoot);

	struct tagVAData {
		VEC3D		_position;
		VEC3D		_normal;
		VEC3D		_binormal;
		VEC3D		_tangent;
		VEC2D		_uv;
		UINT32		_blendindex[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, };
		REAL32		_blendweight[4] = { 0.0f };

		tagVAData() { }

		bool	IsSame( tagVAData & d) {
			if( i3Vector::isAlike( &_position, &d._position) == false)	return false;
			if( i3Vector::isAlike( &_normal, &d._normal) == false)		return false;
			if( i3Vector::isAlike( &_binormal, &d._binormal) == false)	return false;
			if( i3Vector::isAlike( &_tangent, &d._tangent) == false)		return false;
			if( i3Vector::isAlike( &_uv, &d._uv) == false)				return false;
			for( INT32 i = 0; i < 4; i++)
			{
				if( _blendindex[i] != d._blendindex[i]) return false;
				if( _blendweight[i] != d._blendweight[i]) return false;
			}
			return true;
		}
	};

	i3::vector<UINT32>	IndexedList;
	i3::vector<tagVAData>	VecList;

	for( INT32 i = 0; i < getGeoAttrCount(); i++)
	{
		i3GeometryAttr * pAttr = getGeoAttr( i);

		if( pAttr->GetVertexArray() == nullptr)
			continue;

		if( pAttr->GetIndexArray() == nullptr)
			continue;

		INT32 idxVA = FindVA( pAttr->GetVertexArray());
		I3ASSERT( idxVA != -1);

		pInfo = getVA( idxVA);

		if( pInfo->m_pNewVA != nullptr)
			continue;

		IndexedList.resize( pInfo->m_pOldVA->GetCount());

		pInfo->m_pOldVA->Lock();


		for( UINT32 j = 0; j < pInfo->m_pOldVA->GetCount(); j++)
		{
			tagVAData src;
			pInfo->m_pOldVA->GetPosition( j, &src._position);

			if( pInfo->m_pOldVA->HasNormal())
				pInfo->m_pOldVA->GetNormal( j, &src._normal);
			if( pInfo->m_pOldVA->HasBinormal())
				pInfo->m_pOldVA->GetBinormal( j, &src._binormal);
			if( pInfo->m_pOldVA->HasTangent())
				pInfo->m_pOldVA->GetTangent( j, &src._tangent);

			pInfo->m_pOldVA->GetTextureCoord( 0, j, &src._uv);
			
			for( INT32 k = 0; k < pInfo->m_pOldVA->GetBlendIndexCount(); k++)
			{
				src._blendindex[k] = pInfo->m_pOldVA->GetBlendIndex( k, j);
			}

			for( INT32 k = 0; k < pInfo->m_pOldVA->GetBlendWeightCount(); k++)
			{
				src._blendweight[k] = pInfo->m_pOldVA->GetBlendWeight( k, j);
			}

			bool bSame = false;
			for( UINT32 k = 0; k < VecList.size(); k++)
			{
				tagVAData &d = VecList.at(k);
				if( src.IsSame( d))
				{
					IndexedList[j] = k;
					bSame = true;
					break;
				}
			}

			if( bSame == false)
			{
				IndexedList[j] = VecList.size();
				VecList.push_back( src);
			}
		}

		pInfo->m_pOldVA->Unlock();

		if( pInfo->m_pOldVA->GetCount() != VecList.size())
		{
			pInfo->m_pNewVA = i3VertexArray::new_object();
			pInfo->m_pNewVA->Create( pInfo->m_pOldVA->GetFormat(), VecList.size(), pInfo->m_pOldVA->GetUsageFlag());

			pInfo->m_pNewVA->Lock();
			for( UINT32 j = 0; j < VecList.size(); j++)
			{
				tagVAData &d = VecList.at(j);
				pInfo->m_pNewVA->SetPosition( j, &d._position);
				pInfo->m_pNewVA->SetNormal( j, &d._normal);
				pInfo->m_pNewVA->SetBinormal( j, &d._binormal);
				pInfo->m_pNewVA->SetTangent( j, &d._tangent);
				pInfo->m_pNewVA->SetTextureCoord( 0, j, &d._uv);

				for( UINT32 k = 0; k < (UINT32)pInfo->m_pNewVA->GetBlendIndexCount(); k++)
				{
					pInfo->m_pNewVA->SetBlendIndex( k, j, d._blendindex[k]);
				}

				for( UINT32 k = 0; k < (UINT32)pInfo->m_pNewVA->GetBlendWeightCount(); k++)
				{
					pInfo->m_pNewVA->SetBlendWeight( k, j, d._blendweight[k]);
				}
			}
			pInfo->m_pNewVA->Unlock();

			// Index Àç±¸¼º
			pAttr->GetIndexArray()->Lock();
			for( UINT32 j = 0; j < pAttr->GetIndexArray()->GetCount(); j++)
			{
				UINT32 idx = pAttr->GetIndexArray()->GetIndex( j);
				pAttr->GetIndexArray()->SetIndex( j, IndexedList[ idx]);
			}
			pAttr->GetIndexArray()->Unlock();

			pAttr->SetVertexArray( pInfo->m_pNewVA, pAttr->GetPrimitiveType(), pAttr->GetPrimitiveCount());
		}

		VecList.clear();
	}
}
