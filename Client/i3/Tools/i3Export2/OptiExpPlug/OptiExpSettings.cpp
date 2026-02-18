
#include "stdafx.h"
#include "OptiExpSettings.h"

#include "OptiExp_Alli3OptSceneOptimizers.h"


//
// OptiExpSettings
//

OptiExpSettings**		OptiExpSettings::ms_expSettings = NULL;
INT32					OptiExpSettings::ms_numSettings = 0;


OptiExpSettings::OptiExpSettings()
{
}

OptiExpSettings::~OptiExpSettings()
{
	ClearOperation();
}

INT32 OptiExpSettings::GetNumExportOperation()
{
	return m_oper.size();
}

const char* OptiExpSettings::GetExportOperationTitle(INT32 index)
{
	if(index < 0 || index >= (INT32)m_operTitle.size())
		return "empty operation";

	return m_operTitle[index].c_str();
}

OptiExpTaskVec* OptiExpSettings::GetExportOperTasks(INT32 index)
{
	if(index < 0 || index >= (INT32)m_oper.size())
		return NULL;

	return m_oper[index];
}

void OptiExpSettings::GenerateOperation()
{
	ClearOperation();
}

void OptiExpSettings::ClearOperation()
{
	for(OperVec::size_type i=0; i< m_oper.size(); i++)
	{
		OptiExpTaskVec* tasks = m_oper[i];

		for(OptiExpTaskVec::size_type k=0; k<tasks->size(); k++)
			delete (*tasks)[i];
				
		delete tasks;
	}

	m_oper.clear();
}

void OptiExpSettings::FileName_ApplySuffix(char* name, const char* suffix, const char* ext)
{
	if(!name || !suffix)
		return;

	bool extMatch = true;
	char* extSt = strchr(name, '.');

	if(extSt)
	{
		char* e = extSt;

		if( !(*e) || stricmp(e, ext))
			extMatch = false;
	}
	else
	{
		extMatch = false;
	}
	
	char* nameSuffixEn = NULL;

	if(extSt)
	{
		nameSuffixEn = extSt;
		--nameSuffixEn;

		if(nameSuffixEn <= name)
		{
			nameSuffixEn = name;
			while( (*nameSuffixEn) != NULL) { nameSuffixEn++; }
			--nameSuffixEn;
		}
	}
	else
	{
		nameSuffixEn = name;
		while( (*nameSuffixEn) != NULL) { nameSuffixEn++; }
		--nameSuffixEn;
	}

	bool suffixMatch = true;
	{
		const char* sufst = suffix;
		const char* sufen = suffix;

		while(*sufen != NULL){ sufen++; }
		sufen--;
	
		for(; sufen >= sufst; sufen--, nameSuffixEn--)
		{
			if(nameSuffixEn < name)
			{
				suffixMatch = false;
				break;
			}

			if(tolower(*nameSuffixEn) != tolower(*sufen))
			{
				suffixMatch = false;
				break;
			}
		}
	}

	if(extMatch && suffixMatch)
		return;

	if(!extMatch && suffixMatch)
	{
		if(!extSt)
			strcat(name, ext);
		else
			strcpy(extSt, ext);
	}
	else
	{
		//  if(extMatch && !suffixMatch)
		//  if(!extMatch && !suffixMatch) 
		if(!extSt)
		{
			strcat(name, suffix);
			strcat(name, ext);
		}
		else
		{
			strcpy(extSt, suffix);
			strcat(extSt, ext);
		}
	}
}


//
//  OptiExpSettingsWorld
//

OptiExpSettingsWorld::OptiExpSettingsWorld()
{
	// generate operation title
	m_operTitle.push_back("export world(xxx_world.i3s)");
	m_operTitle.push_back("export hit collision(xxx_col_hit.i3s)");
	m_operTitle.push_back("export sm collision(xxx_col_hero.i3s)");
	m_operTitle.push_back("export occlusion mesh (xxx_occ.i3s)");
	//
	// export world
	//
	{
		OptiExpTaskVec* task = new OptiExpTaskVec;
		{
			{
				OptiExpTaskPhsXSetBeforeExport* x = new OptiExpTaskPhsXSetBeforeExport();
				x->m_dynType = OptiExpTaskPhsXSetBeforeExport::EPHY_DYT_NONE;

				task->push_back((OptiExpTask*)x);
			}

			{
				OptiExpTaskI3Exporter* x = new OptiExpTaskI3Exporter();

				x->m_option->m_bExportI3S = false;
				x->m_option->m_bRotateAxis = true;
				x->m_option->m_bReportError = true;
				x->m_option->m_bUseMaterialName = true;

				x->m_option->m_bExportMesh = true;
				x->m_option->m_bViewAfterExport = false;
				
				task->push_back((OptiExpTask*)x);
			}

			// start of OptiExpTaskDx9RenCtxContainer 
			{
				OptiExpTaskDx9RenCtxContainer* x = new OptiExpTaskDx9RenCtxContainer();

				// SubTask
				{
					OptiExpTaskMipmapGen* subTask = new OptiExpTaskMipmapGen();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				{
					OptiExpTaskCvtTextureToDDS* subTask = new OptiExpTaskCvtTextureToDDS();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				{
					OptiExpTaskNodeOpti* subTask = new OptiExpTaskNodeOpti();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				{
					//i3OptRenAttr
					OptiExpTaskRenAttr* subTask = new OptiExpTaskRenAttr();
					subTask->SetEnable(false);
					x->AddSubTask((OptiExpTask*)subTask);
				}

				{
					OptiExpTaskCreateBoundBox* subTask = new OptiExpTaskCreateBoundBox();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				{
					OptiExpTaskRemoveVtxComponent* subTask = new OptiExpTaskRemoveVtxComponent();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				task->push_back((OptiExpTask*)x);		

			} // end of OptiExpTaskDx9RenCtxContainer

			{
				//저장
				OptiExpTaskSaveI3S* x = new OptiExpTaskSaveI3S();
				task->push_back((OptiExpTask*)x);
			}
			
			m_oper.push_back(task);
		}
	}

	//
	// export hit
	//
	{
		OptiExpTaskVec* task = new OptiExpTaskVec;

		{
			// check dupplicated, Empty named
			OptiExpDupplicatedNameChecker* x = new OptiExpDupplicatedNameChecker();
			task->push_back((OptiExpTask*)x);
		}

		{
			// PHYSX - None
			OptiExpTaskPhsXSetBeforeExport* x = new OptiExpTaskPhsXSetBeforeExport();
			x->m_dynType = OptiExpTaskPhsXSetBeforeExport::EPHY_DYT_NONE;
			x->SetIncludeNode("Scene Root");
			x->SetIncludeNodeOnly(true);

			task->push_back((OptiExpTask*)x);
		}

		{
			// PHYSX
			OptiExpTaskPhsXSetBeforeExport* x = new OptiExpTaskPhsXSetBeforeExport();
			x->m_dynType = OptiExpTaskPhsXSetBeforeExport::EPHY_DYT_STATIC;
			x->SetExcludeNode("Scene Root"); 
			// Scene Root는 3dsmax의 최상위 노드(GetCOREInterface()->GetRootNode();
			// 사용자가만든 노드가 아니다. 물리처리(STATIC세팅)에 포함되지않게 

			task->push_back((OptiExpTask*)x);
		}

		{
			OptiExpCheckCollision* x = new OptiExpCheckCollision();
			x->SetEnable(false);
			task->push_back((OptiExpTask*)x);
		}

		OptiExpTaskSetAniTimeRange* prevAniTimeRange = NULL; //복구용
		{
			// 애니메이션 시간 -1, 0으로 설정

			OptiExpTaskSetAniTimeRange* x = new OptiExpTaskSetAniTimeRange(-1,0);
			prevAniTimeRange = x;
			task->push_back((OptiExpTask*)x);
		}

		{
			OptiExpTaskI3Exporter* x = new OptiExpTaskI3Exporter();

			x->m_option->m_bExportI3S = false;
			x->m_option->m_bRotateAxis = true;
			x->m_option->m_bReportError = true;

			x->m_option->m_bExportMesh = true;
			x->m_option->m_bExportLight = true;
			x->m_option->m_bExportCamera = true;

			x->m_option->m_bViewAfterExport = false;

			task->push_back((OptiExpTask*)x);
		}

		{
			// 애니메이션 시간 복구
			OptiExpTaskSetAniTimeRange* x = new OptiExpTaskSetAniTimeRange(prevAniTimeRange);
			prevAniTimeRange = x;
			task->push_back((OptiExpTask*)x);
		}

		{
			// 관통값설정
			OptiExpTaskSetupThickness* x = new OptiExpTaskSetupThickness();
			
			task->push_back((OptiExpTask*)x);
		}

		{
			OptiExpCheckPhysXShapeSet* x = new OptiExpCheckPhysXShapeSet();
			task->push_back((OptiExpTask*)x);
		}

		{
			//저장
			OptiExpTaskSaveI3S* x = new OptiExpTaskSaveI3S();
			task->push_back((OptiExpTask*)x);
		}

		m_oper.push_back(task);
	}

	//
	// export sm
	//
	{
		OptiExpTaskVec* task = new OptiExpTaskVec;

		{
			// check dupplicated, Empty named
			OptiExpDupplicatedNameChecker* x = new OptiExpDupplicatedNameChecker();
			x->SetEnable(false);
			task->push_back((OptiExpTask*)x);
		}

		{
			// PHYSX - None
			OptiExpTaskPhsXSetBeforeExport* x = new OptiExpTaskPhsXSetBeforeExport();
			x->m_dynType = OptiExpTaskPhsXSetBeforeExport::EPHY_DYT_NONE;
			x->SetIncludeNode("Scene Root");
			x->SetIncludeNodeOnly(true);

			task->push_back((OptiExpTask*)x);
		}

		{
			// PHYSX - Static
			OptiExpTaskPhsXSetBeforeExport* x = new OptiExpTaskPhsXSetBeforeExport();
			x->m_dynType = OptiExpTaskPhsXSetBeforeExport::EPHY_DYT_STATIC;
			x->SetExcludeNode("Scene Root");

			task->push_back((OptiExpTask*)x);
		}

		{
			// check collision mesh
			OptiExpCheckCollision* x = new OptiExpCheckCollision();
			task->push_back((OptiExpTask*)x);
		}

		{
			// 애니메이션 시간 -1,0으로 설정
		}

		{
			OptiExpTaskI3Exporter* x = new OptiExpTaskI3Exporter();

			x->m_option->m_bExportI3S = false;
			x->m_option->m_bRotateAxis = true;
			x->m_option->m_bReportError = true;

			x->m_option->m_bExportMesh = true;
			x->m_option->m_bExportLight = true;
			x->m_option->m_bExportCamera = true;

			x->m_option->m_bViewAfterExport = false;

			x->m_option->m_bCreateForSimulate = true;
			
			task->push_back((OptiExpTask*)x);
		}

		{
			OptiExpCheckPhysXShapeSet* x = new OptiExpCheckPhysXShapeSet();
			task->push_back((OptiExpTask*)x);
		}

		{
			//저장
			OptiExpTaskSaveI3S* x = new OptiExpTaskSaveI3S();
			task->push_back((OptiExpTask*)x);
		}

		m_oper.push_back(task);
	}

	//
	// export occlusion mesh
	//
	{
		OptiExpTaskVec* task = new OptiExpTaskVec;

		{
			OptiExpTaskPhsXSetBeforeExport* x = new OptiExpTaskPhsXSetBeforeExport();
			x->m_dynType = OptiExpTaskPhsXSetBeforeExport::EPHY_DYT_NONE;
			task->push_back((OptiExpTask*)x);
		}

		OptiExpTaskSetAniTimeRange* prevAniTimeRange = NULL; //복구용
		{
			// 애니메이션 시간 -1, 0으로 설정

			OptiExpTaskSetAniTimeRange* x = new OptiExpTaskSetAniTimeRange(-1, 0);
			prevAniTimeRange = x;
			task->push_back((OptiExpTask*)x);
		}
		
		{
			OptiExpTaskI3Exporter* x = new OptiExpTaskI3Exporter();

			x->m_option->m_bExportI3S = false;
			x->m_option->m_bRotateAxis = true;
			x->m_option->m_bReportError = true;
			x->m_option->m_bUseMaterialName = false;
			x->m_option->m_bExportMesh = true;
			x->m_option->m_bExportLight = false;
			x->m_option->m_bExportCamera = false;
			x->m_option->m_bViewAfterExport = false;

			task->push_back((OptiExpTask*)x);
		}

		// start of OptiExpTaskDx9RenCtxContainer 
		{
			OptiExpTaskDx9RenCtxContainer* x = new OptiExpTaskDx9RenCtxContainer();

			// SubTask
			{
				OptiExpTaskRemoveVtxComponent_occluder* subTask = new OptiExpTaskRemoveVtxComponent_occluder();
				x->AddSubTask((OptiExpTask*)subTask);
			}

			{
				OptiExpTaskRemoveAttr_occluder* subTask = new OptiExpTaskRemoveAttr_occluder();
				x->AddSubTask((OptiExpTask*)subTask);
			}
			
			task->push_back((OptiExpTask*)x);

		} // end of OptiExpTaskDx9RenCtxContainer
		
		{
			// 애니메이션 시간 복구
			OptiExpTaskSetAniTimeRange* x = new OptiExpTaskSetAniTimeRange(prevAniTimeRange);
			prevAniTimeRange = x;
			task->push_back((OptiExpTask*)x);
		}
		
		{
			//저장
			OptiExpTaskSaveI3S* x = new OptiExpTaskSaveI3S();
			task->push_back((OptiExpTask*)x);
		}

		m_oper.push_back(task);
	}
}

const char* OptiExpSettingsWorld::GetTitle()
{
	return "World";
}

void OptiExpSettingsWorld::ApplyFileName(INT32 operIndex, char* name)
{
	//switch(operIndex)
	//{
	//case 0:
	//	OptiExpSettings::FileName_ApplySuffix(name, "_world", ".i3s");
	//	break;

	//case 1: 
	//	OptiExpSettings::FileName_ApplySuffix(name, "_col_hit", ".i3s");
	//	break;

	//case 2:
	//	OptiExpSettings::FileName_ApplySuffix(name, "_col_hero", ".i3s");
	//	break;

	//default:
	//	break;
	//}
}


//
//  OptiExpSettingsCharacter
//

OptiExpSettingsCharacter::OptiExpSettingsCharacter()
{
	// generate operation title
	m_operTitle.push_back("Helmet/Weapon/Character LOD0 (PhysX Obj) (*.i3s)");
	m_operTitle.push_back("Helmet/Weapon/Character LOD1/LOD2        (*.i3s)");
	m_operTitle.push_back("Weapon animation (*.i3a)");

	//
	// export helmet/weapon/character LOD0
	//
	{
		OptiExpTaskVec* task = new OptiExpTaskVec;
		{
			// setting exporter option
			{
				OptiExpTaskI3Exporter* x = new OptiExpTaskI3Exporter();

				x->m_option->m_bExportI3S = false;

				x->m_option->m_bExportMesh = true;
				
				x->m_option->m_bExportShapeAsConvex = true;
				x->m_option->m_bCreateRappingConvex = true;
				x->m_option->m_bCreateForSimulate = true;
				
				task->push_back((OptiExpTask*)x);
			}

			// i3sOptRemoveAnim
			{
				OptiExpTaskRemoveAnim* x = new OptiExpTaskRemoveAnim();
				task->push_back((OptiExpTask*)x);
			}

			// Dx associated tasks
			{
				OptiExpTaskDx9RenCtxContainer* x = new OptiExpTaskDx9RenCtxContainer();

				// SubTask

				// exporter에서 텍스쳐를 i3TextureMem으로 생성후 DDS변환작업을하는데
				// Mipmap 생성후 DDS변환을 해야한다. 반대로하면 텍스쳐가 이상하게 익스포트된다.
				// 1. OptiExpTaskMipmapGen -> 2. OptiExpTaskCvtTextureToDDS 순서는 반드시지킬것
				{
					OptiExpTaskMipmapGen* subTask = new OptiExpTaskMipmapGen();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				{
					OptiExpTaskCvtTextureToDDS* subTask = new OptiExpTaskCvtTextureToDDS();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				task->push_back((OptiExpTask*)x);
			} 

			// save i3s
			{
				OptiExpTaskSaveI3S* x = new OptiExpTaskSaveI3S();
				task->push_back((OptiExpTask*)x);
			}

			m_oper.push_back(task);
		}
	}


	//
	// export helmet/weapon/character LOD1/LOD2
	//
	{
		OptiExpTaskVec* task = new OptiExpTaskVec;
		{
			// setting exporter option
			{
				OptiExpTaskI3Exporter* x = new OptiExpTaskI3Exporter();

				x->m_option->m_bExportI3S = false;
				x->m_option->m_bExportMesh = true;
				
				task->push_back((OptiExpTask*)x);
			}

			// i3sOptRemoveAnim
			{
				OptiExpTaskRemoveAnim* x = new OptiExpTaskRemoveAnim();
				task->push_back((OptiExpTask*)x);
			}

			// Dx associated tasks
			{
				OptiExpTaskDx9RenCtxContainer* x = new OptiExpTaskDx9RenCtxContainer();

				// SubTask
				{
					OptiExpTaskMipmapGen* subTask = new OptiExpTaskMipmapGen();
					x->AddSubTask((OptiExpTask*)subTask);
				}

				{
					OptiExpTaskCvtTextureToDDS* subTask = new OptiExpTaskCvtTextureToDDS();
					x->AddSubTask((OptiExpTask*)subTask);
				}				

				task->push_back((OptiExpTask*)x);
			}

			// save i3s
			{
				OptiExpTaskSaveI3S* x = new OptiExpTaskSaveI3S();
				task->push_back((OptiExpTask*)x);
			}

			m_oper.push_back(task);
		}
	}


	//
	// export weapon animation
	//
	if(0)
	{
		OptiExpTaskVec* task = new OptiExpTaskVec;
		{
			// setting bone list from 3dsmax file on export option 
			OptiExpAnimBoneListManager* animBoneListManager = NULL;
			{
				animBoneListManager = new OptiExpAnimBoneListManager();
				animBoneListManager->ReadListFromMaxFile();

				task->push_back((OptiExpTask*)animBoneListManager);
			}

			// setting exporter option
			{
				OptiExpTaskI3Exporter* x = new OptiExpTaskI3Exporter();

				x->m_option->m_bExportI3S = false;
				x->m_option->m_bExportAnim = true;
				x->m_option->m_SamplingInterval = 1;

				x->ApplyAnimBoneListOpt(animBoneListManager);
				task->push_back((OptiExpTask*)x);
			}

			// Animation Pack Builder
			{
				OptiExpTaskAnimPackBuilder* x = new OptiExpTaskAnimPackBuilder();
				task->push_back((OptiExpTask*)x);
			}

			m_oper.push_back(task);
		}
	}

}

const char* OptiExpSettingsCharacter::GetTitle()
{
	return "Character";
}



//
// implement static function for IOptmizeExpSettings
//

void IOptiExpSettings::CreateExpSettings()
{
	if(OptiExpSettings::ms_expSettings)
		return;

	OptiExpSettings::ms_numSettings = 2;
	OptiExpSettings::ms_expSettings = new OptiExpSettings*[OptiExpSettings::ms_numSettings];

	OptiExpSettings::ms_expSettings[0] = new OptiExpSettingsWorld;
	OptiExpSettings::ms_expSettings[1] = new OptiExpSettingsCharacter;	
}

void IOptiExpSettings::DestroyExpSettings()
{
	OptiExpSettings** expSetAry = OptiExpSettings::ms_expSettings;

	if(!expSetAry)
		return;

	for(INT32 i=0; i<OptiExpSettings::ms_numSettings; i++)
		delete expSetAry[i];

	delete [] expSetAry;

	OptiExpSettings::ms_expSettings = NULL;
	OptiExpSettings::ms_numSettings = 0;
}

IOptiExpSettings* IOptiExpSettings::GetExpSettings(INT32 index)
{
	if(!OptiExpSettings::ms_expSettings)
		return NULL;

	if(index < 0 || index >= OptiExpSettings::ms_numSettings)
		return NULL;

	return OptiExpSettings::ms_expSettings[index];
}

INT32 IOptiExpSettings::GetNumExpSettings()
{
	return OptiExpSettings::ms_numSettings;
}

