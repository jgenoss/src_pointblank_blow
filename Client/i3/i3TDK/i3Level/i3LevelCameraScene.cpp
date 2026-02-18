#include "stdafx.h"
#include "i3LevelCameraScene.h"
#include "i3LevelGlobalVariable.h"

I3_CLASS_INSTANCE( i3LevelSceneCamera);

i3LevelSceneCamera::i3LevelSceneCamera(void)
{
	SetName( "SceneCamera");
	
	/////////////////////////////////////////////////////////
	//Set Hero Icon
	m_pHeroIcon = i3LevelControl::new_object();
	m_pHeroIcon->SetStyle( I3_LEVEL_STYLE_VOLATILE | I3_LEVEL_STYLE_CONTROL | I3_LEVEL_STYLE_NOSCALE | I3_LEVEL_STYLE_NOROTATE);
	m_pHeroIcon->SetName( GetName());
	{
		i3LevelResSceneGraph * pRes = nullptr;
		i3LevelResSceneGraph * pNewRes = i3LevelResSceneGraph::new_object();
		pRes = g_pScene->GetCommonHeroResource();
		pRes->CopyTo( pNewRes, I3_COPY_INSTANCE);
		m_pHeroIcon->SetSymbol( pNewRes->GetSceneRoot());
		I3_MUST_RELEASE(pNewRes);
	}
	g_pScene->AddElement( m_pHeroIcon, "Hero");
	m_pHeroIcon->SetScale( 10.0f, 10.0f, 10.0f);
	m_pHeroIcon->setPos( 0.0f, 0.0f, 0.0f ); 
}

i3LevelSceneCamera::~i3LevelSceneCamera(void)
{
	//Delete Action List 
	DeleteAllAction(); 
/*
	//Delete Icon
	if(m_pCameraIcon != nullptr)
	{
		g_pScene->RemoveElement( m_pCameraIcon );
		m_pCameraIcon->Release(); 
		m_pCameraIcon = nullptr; 
	}
*/
	if(m_pHeroIcon != nullptr)
	{
		g_pScene->RemoveElement( m_pHeroIcon );
		//m_pHeroIcon->Release(); 
		m_pHeroIcon = nullptr; 
	}

}

void i3LevelSceneCamera::OnCreate(REAL32 TotalTime)
{

}

void i3LevelSceneCamera::SetIcon(void)
{
	VEC3D Pos; 
	setXYZ(&Pos, 0.0f, 0.0f, 0.0f); 

	if(m_pStartAction == nullptr)
	{
		
	}else
	{
		if( m_pStartAction->m_AtState == I3Level_SCENECAMERA_AT_HERO)
		{
//			i3Vector::Add(&Pos, &m_HeaderInfo.m_CamPos, g_pScene->GetHeroPos() ); 			
		}else
		{
			i3Vector::Copy(&Pos, &m_pStartAction->m_At ); 
		}
	}

	m_pCameraIcon->setPos( &Pos ); 

	return; 
}

REAL32 i3LevelSceneCamera::GetTotalTime(void)
{ 
	
	REAL32 TotalTime = 0.0f; 
	I3_Level_CAMERA_ACTION * pAction;
	
	for(size_t i = 0; i < m_ActionList.size(); i++)
	{
		pAction = m_ActionList[i]; 
		TotalTime = TotalTime + pAction->m_TotalTime + pAction->m_InterTime ; 

	}
	return TotalTime; 
}

void i3LevelSceneCamera::DeleteAllAction(void)
{

	//Delete Header 
	i3mem::FillZero(&m_HeaderInfo, sizeof(I3_Level_CAMERA_HEADER)); 	

	//Delete All Boby 
	I3_Level_CAMERA_ACTION * pAction;
	for(size_t i = 0; i < m_ActionList.size(); i++)
	{
		pAction = m_ActionList[i]; 
		delete pAction; 
	}
}

I3_Level_CAMERA_ACTION * i3LevelSceneCamera::GetAction(INT32 idx)
{
	I3_Level_CAMERA_ACTION * pAction = m_ActionList[idx];
	return pAction; 
}

I3_Level_CAMERA_ACTION * i3LevelSceneCamera::InsertAction(I3_Level_CAMERA_ACTION * pPrevAction, I3_Level_CAMERA_ACTION * pNextAction)
{
	I3_Level_CAMERA_ACTION * pAction = new I3_Level_CAMERA_ACTION; 

	i3mem::FillZero( pAction, sizeof(I3_Level_CAMERA_ACTION) ); 
	if(pPrevAction == nullptr && pNextAction == nullptr)
	{
		//처음 만들어진넘이다. 
		OnCreate(1.0f); 
		m_pStartAction = pAction; 
		m_pEndAction   = pAction; 
	}else
	{
		if(pPrevAction == nullptr)
		{
			//가장 처음에 넣어지는 녀석입니다. 
			m_pStartAction = pAction; 
		}else
		{
			pPrevAction->m_pNext = (char *)pAction; 
		}

		if(pNextAction == nullptr) 
		{			
			m_pEndAction = pAction; 
		}else
		{
			pNextAction->m_pPrev = (char *)pAction; 
		}
	}

	pAction->m_pPrev = pPrevAction; 
	pAction->m_pNext = pNextAction; 

	m_ActionList.push_back(pAction); 
	m_HeaderInfo.m_TotalActionCount++; 

	// 초기값을 넣는다. 	
	pAction->m_TotalTime  = 5.0f;									//토탈 타임 	
	pAction->m_AtState	  =	I3Level_SCENECAMERA_AT_HERO;				//At 방향 	
	setXYZ(&pAction->m_At, 0.0f, 0.0f, 0.0f); 	
	setXYZ(&pAction->m_AttachPos, 0.0f, 10.f, 20.f); 

	pAction->m_EffectState	= 0;			
	pAction->m_CircleSpeed	= 10.f; 
	pAction->m_TrembleRand	= 0.2f; 

	pAction->m_InterpolateState = I3Level_SCENECAMERA_INTER_LINEAR;	
	pAction->m_InterTime  = 0.0f;	

	return pAction; 
}

//이거 여기서해야나?? 하는 생각을 하지만..그래도 해야할듯..^^
void i3LevelSceneCamera::ChangeAction(I3_Level_CAMERA_ACTION * pAction, REAL32 Time, INT32 AtState, INT32 EffectState, VEC3D vAt, VEC3D vAttachPostion)
{
	pAction->m_TotalTime	= Time; 
	pAction->m_AtState		= AtState; 
	pAction->m_EffectState	= EffectState; 
	i3Vector::Copy( &pAction->m_At, &vAt ); 
	i3Vector::Copy( &pAction->m_AttachPos, &vAttachPostion); 

	//아이콘 변경하기. 
	SetIcon(); 
	return;
}

void i3LevelSceneCamera::DeleteAction(I3_Level_CAMERA_ACTION * pAction)
{
	I3_Level_CAMERA_ACTION * pPrevAction = (I3_Level_CAMERA_ACTION *)pAction->m_pPrev; 
	I3_Level_CAMERA_ACTION * pNextAction = (I3_Level_CAMERA_ACTION *)pAction->m_pNext; 

	if((pAction == m_pStartAction) && (pAction == m_pEndAction))
	{
		m_pStartAction = nullptr; 
		m_pStartAction = nullptr; 

	}else if( pAction == m_pStartAction)
	{
		m_pStartAction			= pNextAction; 
		pNextAction->m_pPrev	= nullptr; 
        
	}else if( pAction == m_pEndAction)
	{
		m_pEndAction			= pPrevAction; 
		pPrevAction->m_pNext	= nullptr; 
	}else
	{
		pPrevAction->m_pNext = pNextAction; 
		pNextAction->m_pPrev = pPrevAction; 
	}

	i3::vu::remove(m_ActionList, pAction); 

	delete pAction; 
	
	m_HeaderInfo.m_TotalActionCount--; 

	return;
}


BOOL i3LevelSceneCamera::OnLoad(const char * szName)
{
	BOOL Rv; 
#ifdef I3_DEBUG
	I3TRACE("[i3LevelSceneCamera::OnLoad] Filename is Null \n"); 
	if(szName == nullptr)return FALSE; 
#endif
	strncpy(m_szPath, szName, sizeof(m_szPath) - 1); 

	i3FileStream FileStream; 	
	Rv = FileStream.Open(szName, STREAM_READ | STREAM_SHAREREAD);
	if( Rv == FALSE)
	{
//		DebugBox( g_pView->m_hWnd, "%s I3C를 로딩할 수 없습니다.", szName);
		return FALSE; 
	}	
	OnLoad( &FileStream ); 
	return Rv; 
}

BOOL i3LevelSceneCamera::OnLoad( i3FileStream * pStream)
{
	if( pStream == nullptr) return FALSE; 

	UINT32 Rc = 0; 
	UINT32 Result = 0; 

	//Set Action List	
	DeleteAllAction(); 

	//Read Header 
	i3::pack::Level_CAMERA_HEADER m_FileHeader; 
	Rc = pStream->Read( &m_FileHeader, sizeof(i3::pack::Level_CAMERA_HEADER) ); 
	m_HeaderInfo.m_TotalActionCount = m_FileHeader.m_TotalActionCount; 
	m_HeaderInfo.m_Ver				= m_FileHeader.m_Ver; 	
	Result += Rc;

	//툴에서는 하나씩 메모리 할당을 한다. 
	I3_Level_CAMERA_ACTION * pPreAction = nullptr; 
	I3_Level_CAMERA_ACTION * pNowAction = nullptr; 

	i3::pack::Level_CAMERA_ACTION FileChunk; 
	for(INT32 i = 0; i < m_HeaderInfo.m_TotalActionCount; i++)
	{
		pNowAction = new I3_Level_CAMERA_ACTION; 

		Rc = pStream->Read( &FileChunk, sizeof(i3::pack::Level_CAMERA_ACTION) ); 
		Result += Rc;
		
		pNowAction->m_TotalTime		=	FileChunk.m_TotalTime;		
		pNowAction->m_AtState		=	FileChunk.m_AtState;			
		pNowAction->m_EffectState	=	FileChunk.m_EffectState;	
		i3Vector::Set( & pNowAction->m_At, FileChunk.m_AtX, FileChunk.m_AtY, FileChunk.m_AtZ); 		
		i3Vector::Set( & pNowAction->m_AttachPos, FileChunk.m_AttachPosX, FileChunk.m_AttachPosY, FileChunk.m_AttachPosZ); 
		pNowAction->m_CircleSpeed	=	FileChunk.m_CircleSpeed;		
		pNowAction->m_TrembleRand	=	FileChunk.m_TrembleRand;				
		pNowAction->m_InterpolateState=	FileChunk.m_InterpolateState;	
		pNowAction->m_InterTime		=	FileChunk.m_InterTime;		

		//Rc = pStream->Read( pNowAction, sizeof(I3_Level_CAMERA_ACTION) ); 
		

		if(i == 0)
		{
			m_pStartAction = pNowAction; 
		}else
		{		
			pPreAction->m_pNext = (char*)pNowAction; 
			pNowAction->m_pPrev = (char*)pPreAction; 
		}
		pNowAction->m_pNext = nullptr; 

		pPreAction			= pNowAction;
		m_ActionList.push_back(pNowAction); 
	}	

	m_pEndAction = pNowAction; 

	//여기에서 파일 크기를 검사한다. 
	//if(Result != pStream->GetSize())
	//{
	//	I3TRACE("File Load Error [i3LevelSceneCamera::OnLoad]\n"); 
	//	DeleteAllAction(); 
	//	return FALSE; 
	//}

	return TRUE; 
}

UINT32 i3LevelSceneCamera::OnLoad( i3ResourceFile * pResFile)
{
	if( OnLoad( (i3FileStream *) pResFile->GetStream()) == FALSE)
		return STREAM_ERR;

	return 10;
}

BOOL i3LevelSceneCamera::OnSave(void)
{
	BOOL Rv;  
	i3FileStream	FileStream; 
	Rv = FileStream.Create(m_szPath, STREAM_WRITE); 
	if( Rv == FALSE)
	{
//		DebugBox( g_pView->m_hWnd, "%s INI 파일을 생성할 수 없습니다.", pszPath);
		return FALSE; 
	}
	Rv = OnSave(&FileStream); 
	FileStream.Close(); 

	return TRUE; 
}

BOOL i3LevelSceneCamera::OnSaveAs(const char * szName)
{
	//Set File Name 
	strncpy(m_szPath, szName, sizeof(m_szPath) - 1);
	return OnSave(); 
}

BOOL i3LevelSceneCamera::OnSave( i3FileStream * pStream)
{
	if( pStream == nullptr) return FALSE; 

	UINT32 Rc = 0; 
	UINT32 Result = 0; 

	//Read Header 
	i3::pack::Level_CAMERA_HEADER		FileHeader; 
	 
	FileHeader.m_TotalActionCount	= (INT32) m_ActionList.size(); 
	Rc = pStream->Write( &FileHeader, sizeof(i3::pack::Level_CAMERA_HEADER) ); 
	Result += Rc;
	
	I3_Level_CAMERA_ACTION			*	pAction = m_pStartAction; 
	i3::pack::Level_CAMERA_ACTION		FileChunk; 
	INT32 Counter = 0; 
	while(pAction != nullptr)
	{
		FileChunk.m_TotalTime		= 	pAction->m_TotalTime;	
		FileChunk.m_AtState			= 	pAction->m_AtState	;
		FileChunk.m_EffectState		= 	pAction->m_EffectState	;
		FileChunk.m_CircleSpeed		= 	pAction->m_CircleSpeed;
		FileChunk.m_TrembleRand		= 	pAction->m_TrembleRand;	
		FileChunk.m_InterpolateState=	pAction->m_InterpolateState;
		FileChunk.m_InterTime		= 	pAction->m_InterTime;
		FileChunk.m_AtX				=	getX(& pAction->m_At); 
		FileChunk.m_AtY				=	getY(& pAction->m_At); 
		FileChunk.m_AtZ				=  	getZ(& pAction->m_At); 
		FileChunk.m_AttachPosX		=	getX(& pAction->m_AttachPos);
		FileChunk.m_AttachPosY		=	getY(& pAction->m_AttachPos);
		FileChunk.m_AttachPosZ		=	getZ(& pAction->m_AttachPos);

		Rc = pStream->Write( &FileChunk, sizeof(i3::pack::Level_CAMERA_ACTION)); 
		Result += Rc;
		Counter++; 
		pAction = (I3_Level_CAMERA_ACTION *)pAction->m_pNext; 
	}
	
	if( Counter != (INT32)m_ActionList.size() )
	{
		I3TRACE("파일 저장에 문제가 있습니다. 성백 불러보아요..\n"); 
	}
    
	return TRUE; 
}

UINT32 i3LevelSceneCamera::OnSave( i3ResourceFile * pResFile)
{
	if( OnSave( (i3FileStream *) pResFile->GetStream()) == FALSE)
		return STREAM_ERR;

	return 10;
}

void i3LevelSceneCamera::Play(void)
{
	if(m_pStartAction == nullptr)return; 
	m_LocalTime = 0.0f; 
	m_NowAction = m_pStartAction; 
	MATRIX * pCam = GetTransform();

	if( m_NowAction->m_AtState == I3Level_SCENECAMERA_AT_HERO	)
		i3Matrix::SetPos(pCam, m_pHeroIcon->getPos() ); 
	else	
		i3Matrix::SetPos(pCam, & m_NowAction->m_At ); 
	
	//Set Add AttachPos 
	i3Vector::Add( i3Matrix::GetPos(pCam), i3Matrix::GetPos(pCam), & m_NowAction->m_AttachPos ); 
	i3Matrix::Copy(&m_BeMat, pCam); 

	m_PlayState = I3Level_SCENECAMERA_PLAY; 
	return; 
}

void i3LevelSceneCamera::Pause(void)
{
	return; 
}

void i3LevelSceneCamera::Stop(void)
{
	return; 
}

void i3LevelSceneCamera::OnDrive( REAL32 time)
{
	if(m_NowAction == nullptr) 
	{
		return; 
	}

	m_LocalTime = m_LocalTime + time; 
	MATRIX * pCam = GetTransform();

	switch(m_PlayState)
	{
	case I3Level_SCENECAMERA_PLAY:
		{			
			MATRIX	 Mat; 
			
			//카메라 연출입니다. 		
			BOOL Tremble = FALSE; 
			BOOL Circle  = FALSE; 
			VEC3D	NewPos; 
			VEC3D	AtPos; 

			//1. 아무런 것도 없을때 
			i3Vector::Copy( &NewPos, i3Matrix::GetPos(&m_BeMat) ); 

			/////////////////////////////////////////////////////////////////////////////////////////
			switch(m_NowAction->m_AtState)
			{
			case I3Level_SCENECAMERA_AT_HERO		: i3Vector::Copy(&AtPos, m_pHeroIcon->getPos() ); break; 
			case I3Level_SCENECAMERA_AT_POS		: i3Vector::Copy(&AtPos, &m_NowAction->m_At );	  break; 
			}

			if( (m_NowAction->m_EffectState & I3Level_SCENECAMERA_EFFECT_CIRCLE) == I3Level_SCENECAMERA_EFFECT_CIRCLE) 
				Circle = TRUE; 
			if( (m_NowAction->m_EffectState & I3Level_SCENECAMERA_EFFECT_TREMBLE) == I3Level_SCENECAMERA_EFFECT_TREMBLE) 
				Tremble = TRUE; 

			//2. 원모양으로 돌때 
			if(Circle == TRUE)
			{
				MATRIX	Mat2;
				VEC3D	TransPos; 
				
				i3Matrix::SetPos( &Mat2, &AtPos ); 

				i3Matrix::Inverse(&Mat2, nullptr, &Mat2 ); 
				i3Vector::TransformCoord(&TransPos, i3Matrix::GetPos(&m_BeMat), &Mat2); 
				i3Matrix::SetPos( &Mat2, &TransPos ); 			
				i3Matrix::PostRotateYDeg(&Mat2, (m_NowAction->m_CircleSpeed * time)); 			
				i3Vector::Add( i3Matrix::GetPos(&Mat2), i3Matrix::GetPos(&Mat2), &AtPos); 
				i3Vector::Copy( &NewPos, i3Matrix::GetPos(&Mat2) ); 
			}

			////////////////////////////////////////////////////////////
			//화면 보기 
			REAL32 Theta, Rho;
			VEC3D	Normal; 				
			i3Vector::Sub(&Normal, i3Matrix::GetPos(pCam), &AtPos); 
			Theta = atan2f( i3Vector::GetX(&Normal), i3Vector::GetZ(&Normal) );

			i3Matrix::PostRotateY(&Mat,Theta);

			REAL32 fLen;
			fLen = i3Math::Sqrt( (getX(&Normal) * getX(&Normal)) + (getZ(&Normal) * getZ(&Normal)) );
			Rho = atan2f(getY(&Normal), fLen);

			i3Matrix::PreRotateX(&Mat, - Rho);

			//Set Pos
			i3Matrix::SetPos(&Mat, &NewPos); 
			i3Matrix::Copy(&m_BeMat, &Mat); 

			////////////////////////////////////////////////////////////////////////////////////////////////
			//효과
			if(Tremble == TRUE)
			{
				VEC3D  RandVec;
				i3Vector::SetX(&RandVec, i3Math::Randf2() * m_NowAction->m_TrembleRand ); 	
				i3Vector::SetY(&RandVec, i3Math::Randf2() * m_NowAction->m_TrembleRand ); 	
				i3Vector::SetZ(&RandVec, i3Math::Randf2() * m_NowAction->m_TrembleRand ); 	
				i3Vector::Add( i3Matrix::GetPos(&Mat) , i3Matrix::GetPos(&Mat), &RandVec); 		
			}


			//다음 상태로 이동합니다. 
			if( m_LocalTime > m_NowAction->m_TotalTime)
			{
				if(m_NowAction->m_InterTime > 0.00000001f )
				{
					switch(m_NowAction->m_InterpolateState)
					{
					case I3Level_SCENECAMERA_INTER_LINEAR:	
						{
							m_LocalTime = m_LocalTime - m_NowAction->m_TotalTime; 
							m_PlayState = I3Level_SCENECAMERA_INTER;						

							//SetStart 
							i3Vector::Copy( &m_InterStaAt, &AtPos); 
							i3Vector::Copy( &m_InterStaPos , i3Matrix::GetPos(&Mat) );  

							//SetEnd
							I3_Level_CAMERA_ACTION * pNext = (I3_Level_CAMERA_ACTION *)m_NowAction->m_pNext; 
							if(pNext != nullptr)
							{
								switch(pNext->m_AtState)
								{
								case I3Level_SCENECAMERA_AT_HERO	: 
									i3Vector::Copy( &m_InterEndAt, m_pHeroIcon->getPos() );								
									break; 
								case I3Level_SCENECAMERA_AT_POS	: 
									i3Vector::Copy(&m_InterEndAt, & pNext->m_At);										
									break; 
								}						                        		
								i3Vector::Add( &m_InterEndPos, &m_InterEndAt, & pNext->m_AttachPos ); 
							}else
							{
								m_NowAction = nullptr; 
							}
						}
						break; 
					}
				}else
				{
					m_LocalTime = m_LocalTime - m_NowAction->m_TotalTime; 
					m_NowAction = (I3_Level_CAMERA_ACTION	*)m_NowAction->m_pNext; 
					if(m_NowAction != nullptr)
					{
						if( m_NowAction->m_AtState == I3Level_SCENECAMERA_AT_HERO	)
							i3Matrix::SetPos(pCam, m_pHeroIcon->getPos() ); 
						else	
							i3Matrix::SetPos(pCam, & m_NowAction->m_At ); 

						//Set Add AttachPos 
						i3Vector::Add( i3Matrix::GetPos(pCam), i3Matrix::GetPos(pCam), & m_NowAction->m_AttachPos ); 
						i3Matrix::Copy(&m_BeMat, pCam); 
					}
				}
			}

			i3Matrix::Copy(pCam, &Mat);  
		}
		break; 
	case I3Level_SCENECAMERA_INTER: 
		{
			MATRIX	Mat; 
			REAL32	InterTime; 
	
			//1. Set Time 			
			InterTime = m_LocalTime * ( 1.0f/ m_NowAction->m_InterTime);						
			
			//2. Set Pos 
			REAL32 x, y, z, Inverse; 
			VEC3D	Pos, AtPos; 
			Inverse = 1.0f - InterTime; 
			x = ( Inverse * getX(&m_InterStaPos) ) + (InterTime * getX(&m_InterEndPos) ); 
			y = ( Inverse * getY(&m_InterStaPos) ) + (InterTime * getY(&m_InterEndPos) ); 
			z = ( Inverse * getZ(&m_InterStaPos) ) + (InterTime * getZ(&m_InterEndPos) ); 
			i3Vector::Set(&Pos, x, y, z); 			
            	
			//3. Set At
			x = ( Inverse * getX(&m_InterStaAt) ) + (InterTime * getX(&m_InterEndAt) ); 
			y = ( Inverse * getY(&m_InterStaAt) ) + (InterTime * getY(&m_InterEndAt) ); 
			z = ( Inverse * getZ(&m_InterStaAt) ) + (InterTime * getZ(&m_InterEndAt) ); 
			i3Vector::Set(&AtPos, x, y, z); 			

			//4. Set Camera At
			REAL32 Theta, Rho;
			i3Vector::Sub(&AtPos, &Pos, &AtPos); 
			Theta = atan2f( i3Vector::GetX(&AtPos), i3Vector::GetZ(&AtPos) );
			i3Matrix::PostRotateY(&Mat, Theta);

			REAL32 fLen;
			fLen = i3Math::Sqrt( (getX(&AtPos) * getX(&AtPos)) + (getZ(&AtPos) * getZ(&AtPos)) );
			Rho = atan2f(getY(&AtPos), fLen);

			i3Matrix::PreRotateX(&Mat, - Rho);
			
			i3Matrix::SetPos(&Mat, &Pos); 

			i3Matrix::Copy(&m_BeMat, &Mat); 
			i3Matrix::Copy(pCam, &Mat); 

			//이거 다음을 위해서 			
			//m_LocalTime = m_LocalTime + time; 
			if( (m_LocalTime + time) > m_NowAction->m_InterTime )			
			{
				m_LocalTime = m_LocalTime - m_NowAction->m_InterTime; 
				m_NowAction = (I3_Level_CAMERA_ACTION	*)m_NowAction->m_pNext; 
				m_PlayState = I3Level_SCENECAMERA_PLAY;
			}
		}
		break; 
	}

	return; 
}

void i3LevelSceneCamera::GetActionString(INT32 idx, char * pName, UINT16 nBufSize)
{
	switch(idx)
	{
	case I3Level_SCENECAMERA_INTER_LINEAR	: strncpy(pName, "I3Level_SCENECAMERA_INTER_LINEAR", nBufSize);		break; 
	case I3Level_SCENECAMERA_AT_HERO		: strncpy(pName, "I3Level_SCENECAMERA_AT_HERO", nBufSize);			break; 
	case I3Level_SCENECAMERA_AT_POS			: strncpy(pName, "I3Level_SCENECAMERA_AT_POS", nBufSize);			break; 
	default : I3ASSERT_0; break; 
	}
	return; 
}

INT32 i3LevelSceneCamera::GetActionIdx(const char * pName)
{
	INT32 idx = -1; 
	if( strcmp( pName, "I3Level_SCENECAMERA_INTER_LINEAR") == 0 )			idx = I3Level_SCENECAMERA_INTER_LINEAR; 
	else if( strcmp( pName, "I3Level_SCENECAMERA_AT_HERO") == 0 )			idx = I3Level_SCENECAMERA_AT_HERO; 
	else if( strcmp( pName, "I3Level_SCENECAMERA_AT_POS") == 0 )			idx = I3Level_SCENECAMERA_AT_POS; 

	if(idx == -1)I3ASSERT_0; 
	return idx; 
}
