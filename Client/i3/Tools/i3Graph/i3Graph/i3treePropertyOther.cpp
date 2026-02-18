#include "stdafx.h"
#include "i3treeProperty.h"
#include "i3treePropertyBase.h"
#include "i3Graph.h"
#include "i3GraphDlg.h"

#define OTHER_PROPERTY_START_ROW		0

void PropertyBoard::SetCollider(void)
{
	I3_P_INT		* pPropertyInt; 
	i3Collider		* pCollider; 	

	pCollider = (i3Collider *)m_pSelObj; 

	//For INT 1
	pPropertyInt = new I3_P_INT; 
	pPropertyInt->SetName("Style"); 
	pPropertyInt->SetValue( pCollider->GetStyle() ); 
	m_PropertyList.Add((void *)pPropertyInt);

}

void PropertyBoard::ChangeCollider(void)
{	
	I3_P_INT		* pPropertyInt; 
	i3Collider		* pCollider;
	
	pCollider = (i3Collider *)m_pSelObj; 

	switch(m_SelectLine)
	{
	case 0: 
		pPropertyInt = (I3_P_INT *)GetProperty(); 
		pCollider->SetStyle( pPropertyInt->GetValue() );
		break;	
	}

}

void PropertyBoard::SetCollideeMesh(void)
{
	i3CollideeMesh * pCollideeMesh; 
	pCollideeMesh= (i3CollideeMesh *)m_pSelObj; 
	I3_P_INT			* pInt;
	I3_P_TERRAIN_TYPE	* pTerrain; 

	pInt = new I3_P_INT; 
	pInt->SetName("TriangleCount"); 	
	pInt->SetValue( pCollideeMesh->GetTriangleCount() ); 
	pInt->AddStyle(I3_READONLY); 
	m_PropertyList.Add((void *)pInt); 

	pInt = new I3_P_INT; 
	pInt->SetName("Trigger"); 	
	pInt->SetValue( pCollideeMesh->GetTrigger() ); 
	pInt->AddStyle(I3_READONLY); 
	m_PropertyList.Add((void *)pInt); 

	pTerrain = new I3_P_TERRAIN_TYPE; 
	pTerrain->SetName("TerrainType"); 	
	pTerrain->SetValue( pCollideeMesh->GetTerrainType() ); 
	m_PropertyList.Add((void *)pTerrain); 
}

void PropertyBoard::ChangeCollideeMesh(void)
{
	i3CollideeMesh * pCollideeMesh; 
	pCollideeMesh = (i3CollideeMesh *)m_pSelObj; 
	I3_P_TERRAIN_TYPE	* pTerrain; 

	switch(m_SelectLine)
	{
	case OTHER_PROPERTY_START_ROW :	
		break; 
	case OTHER_PROPERTY_START_ROW +1:	
		break; 
	case OTHER_PROPERTY_START_ROW +2:	
		pTerrain = (I3_P_TERRAIN_TYPE *)GetProperty(); 
		pCollideeMesh->SetTerrainType(pTerrain->GetValue()); 
		break; 
	}

}

void PropertyBoard::SetCollideeSphere(void)
{
	i3CollideeSphere * pCollideeSphere; 
	I3_P_VEC3D	* pVec; 
	I3_P_REAL32 * pReal; 

	pCollideeSphere= (i3CollideeSphere *)m_pSelObj; 

	pVec = new I3_P_VEC3D; 
	pVec->SetName("Center"); 	
	pVec->SetValue(* pCollideeSphere->GetCenter() ); 
	m_PropertyList.Add((void *)pVec); 
	
	pReal = new I3_P_REAL32; 
	pReal->SetName("Radius"); 	
	pReal->SetValue(pCollideeSphere->GetRadius()); 
	m_PropertyList.Add((void *)pReal); 
}

void PropertyBoard::ChangeCollideeSphere(void)
{
	i3CollideeSphere * pCollideeSphere; 
	pCollideeSphere= (i3CollideeSphere *)m_pSelObj; 
	I3_P_VEC3D	* pVec; 
	I3_P_REAL32 * pReal; 

	switch(m_SelectLine)
	{
	case OTHER_PROPERTY_START_ROW :	
		pVec = (I3_P_VEC3D *)GetProperty(); 		
		pCollideeSphere->SetCenter(&pVec->GetValue()); 
		break; 
	case OTHER_PROPERTY_START_ROW +1 : 
		pReal = (I3_P_REAL32 *)GetProperty(); 	
		pCollideeSphere->SetRadius(pReal->GetValue()); 
		break; 
	}
}

