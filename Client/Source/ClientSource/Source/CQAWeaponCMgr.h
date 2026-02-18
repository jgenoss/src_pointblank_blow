#pragma once

class CObjectEvtReciver : public i3EventReceiver
{
	I3_CLASS_DEFINE( CObjectEvtReciver, i3EventReceiver );
public:
	CObjectEvtReciver() : contact_(false) { i3Matrix::Identity(&mat_); }

public:
	virtual bool OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	MATRIX* getMatrix() { return &mat_; }
	bool getContact() { return contact_; }
	void setContact(bool cot) { contact_ = cot; }

private:
	MATRIX mat_;
	bool contact_;
};

class CQAWeaponCMgr : public i3::shared_ginst<CQAWeaponCMgr>
{
public:
	CQAWeaponCMgr();
	virtual ~CQAWeaponCMgr();

public:
	void setOnWeaponCollision(bool onoff)	{ onWeaponCollision = onoff; }
	bool getOnWeaponCollision() const		{ return onWeaponCollision; }
	i3::wstring getOutCollisionBox() const;

	void setValue(REAL32 aim, REAL32 scale, INT32 fire_count, INT32 max_count);
	void setAim(REAL32 aim);
	void setFireCount(INT32 count);
	void setBoxScale(REAL32 scale);
	void setBoxMaxCount(INT32 count);

public:
	void CreatePhysXBox(i3World* world);
	void ReleasePhysXBox();

	void ThrowBox(CGameCharaBase* pOwner);

	void Update();
	
private:
	i3Transform2* _CreateShape(REAL32 x, REAL32 y, REAL32 z, COLORREAL color);
	i3Node* _CreateBox( bool bTri, bool bLine, COLORREAL * pTriColor, COLORREAL * pLineColor);

	

private:
	
	i3::vector<i3Transform2*> shape_list;
	i3::vector<i3Transform2*> out_shape_list;

	REAL32 aim_;
	REAL32 box_size_;
	INT32 fire_count_;
	INT32 box_max_count_;

	bool	onWeaponCollision;
	
	INT32	count_;
	UINT32	res_count;

	UINT32	non_contact_outbox;
};