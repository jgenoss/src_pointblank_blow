#include "pch.h"
#include "MainWeapon_DualUziWithSilencer.h"


MainWeapon_DualUziWithSilencer::MainWeapon_DualUziWithSilencer(WeaponBase* p) : MainWeapon_DualUzi(p)
{

}
	
void MainWeapon_DualUziWithSilencer::OnCreate( bool bCreateInWeapon)
{
	MainWeapon_DualUzi::OnCreate( bCreateInWeapon);

	//	소음기가 보인다.
	{
		for( size_t j = 0; j < m_SilencerMaterialList.size(); j++)
		{
			i3Node * pNode = m_SilencerMaterialList[j];
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);		
		}
	}
	
}

