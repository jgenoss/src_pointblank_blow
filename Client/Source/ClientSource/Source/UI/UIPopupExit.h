#pragma once
#include "UIPopupBase.h"

class UIPopupExit : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupExit, UIPopupBase);

	enum { MAX_PRESENT = 4, };

public:
	UIPopupExit();
	virtual ~UIPopupExit();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual bool	OnOKButton() override;

	virtual bool	OnPlayClickSound(i3UIControl * pCtrl) override;

private:
	void SetLabel();
	void SetRank();
	void SetPresent();

private:
	class UIImgSetCtrl* m_ImgBox[MAX_PRESENT];
};
