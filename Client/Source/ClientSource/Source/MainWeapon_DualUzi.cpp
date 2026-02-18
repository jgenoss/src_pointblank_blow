#include "pch.h"
#include "MainWeapon_DualUzi.h"
#include "i3Base/string/ext/utf16_to_mb.h"

MainWeapon_DualUzi::MainWeapon_DualUzi(WeaponBase* p) : MainWeapon_DualSMG(p)
{
		
	
}

// MainWeapon_Dual::OnCreate()를 간접호출하지 않는다... (원래가 그렇게 구현되었던 상태이며 나중에 어떻게든 고칠것..)
void	MainWeapon_DualUzi::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_Dual::OnCreate( bCreateInWeapon);

	// 소음기가 없는 무기는 소음기를 안 보이도록 한다.
	{
		const INT32 cnt = 2;
		char * pszSilencerName[cnt] = {
			"Silencer_Material",
			"Model_Silencer_Material",
		};

		i3Body * pBody = m_pWeaponBase->getSceneObject()->GetBody();

		m_SilencerMaterialList.clear();

		for(INT32 repeat=0; repeat < cnt; repeat++)
		{
			for( UINT32 i = 0; i < pBody->getLODCount(); i++ )
			{
				i3LOD * pLOD = pBody->getLOD( i );
				i3Scene::FindNodeByName( pLOD->getShapeNode(), pszSilencerName[ repeat] , m_SilencerMaterialList );

				for( size_t j = 0; j < m_SilencerMaterialList.size(); j++)
				{
					i3Node * pNode = m_SilencerMaterialList[j];
					pNode->AddFlag( I3_NODEFLAG_DISABLE);		//	기본은 안보이게
				}
			}

			if( m_SilencerMaterialList.size() > 0)
				break;
		}
	}

}

void	MainWeapon_DualUzi::ReturnInstance() // MainWeapon_Dual::ReturnInstance()를 간접호출하지 않는다... (원래가 그렇게 구현되었던 상태이며 나중에 어떻게든 고칠것..)
{
	if( m_pLeftHandWeapon)	
	{
		m_pLeftHandWeapon->setPairWeapon( nullptr);
		g_pWeaponManager->ReleaseCharaWeapon( m_pLeftHandWeapon);
		m_pLeftHandWeapon = nullptr;
	}
}
