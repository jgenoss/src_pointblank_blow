#if !defined( __GAME_MAP_H)
#define __GAME_MAP_H

// "CommonDef.h"의 enum STAGE_ID참조..

void			ChangeMyRoomInfoStageID_ForNonNetworkDebug();

void			create_gmap(STAGE_UID t);
void			destroy_gmap();

template<> struct is_tinst_manual<class gmap> : std::tr1::true_type {};

class gmap
{
public:
	static gmap*		i() { return tinst<gmap>(); }
	virtual void		Process_InitObject(i3Object * pObj);
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const {   	i3Vector::Set( &pos, 10.0f, 2.0f, 10.0f );  }

	virtual void		Process_Event_ArmedBomb_Battle() {}
	virtual void		Process_MoveRepairTrain() {}
	virtual void		Process_SetToOnObjectCamera() {}
	virtual void		Process_Event_RoundStart_MissionNoticeString_From_GameMode() {}

	virtual ~gmap() {}
};

class gmap_port_akaba : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {  i3Vector::Set( &pos, 3.0f, 2.0f, 32.0f );   }
};

class gmap_red_rock : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override { i3Vector::Set( &pos, 9.0f, 2.0f, 45.0f );    }
};

class gmap_library : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {   	i3Vector::Set( &pos, 32.0f, 6.0f, 7.0f );  }
};

class gmap_mstation : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {  	i3Vector::Set( &pos, 5.0f, 4.0f, 33.0f );   }
};

class gmap_burninghall : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {  	i3Vector::Set( &pos, 31.0f, 6.0f, 4.0f );   }
};

class gmap_downtown : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {  	i3Vector::Set( &pos, 34.0f, 7.0f, 34.0f );   }
};

class gmap_luxville : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {    i3Vector::Set( &pos, -4.0f, 8.0f, -24.0f ); }
};

class gmap_outpost  : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {    i3Vector::Set( &pos, 17.0f, 8.0f, 15.0f ); }
	virtual void		Process_SetToOnObjectCamera() override;
};

class gmap_blowcity : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override { 	i3Vector::Set( &pos, -14.0f, 7.0f, 34.0f );    }
};

class gmap_stormtube : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {  	i3Vector::Set( &pos, -14.0f, 2.0f, 40.0f );   }
	virtual void		Process_Event_ArmedBomb_Battle() override;
	virtual void		Process_MoveRepairTrain() override;
	void ResetRepairTrain();		// 현재 쓰이지 않고 있으나, 남김..
};

class gmap_sentrybase : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {   	i3Vector::Set( &pos, -17.0f, 6.0f, 37.0f ); }
};

class gmap_hospital : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {    i3Vector::Set( &pos, 21.0f, 3.0f, 37.0f ); }
};

class gmap_downtown2 : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {   	i3Vector::Set( &pos, -25.0f, -10.0f, 43.0f );  }
};

class gmap_shoppingcenter : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {   	i3Vector::Set( &pos, -43.0f, 2.0f, 19.0f );  }
};

class gmap_sandstorm : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {   i3Vector::Set( &pos, -5.0f, 2.0f, 27.0f );  }
};

class gmap_breeding_nest : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {   i3Vector::Set( &pos, 5.0f, 2.0f, 27.0f );  }
};

class gmap_d_uptown : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {   i3Vector::Set( &pos, 18.0f, 3.0f, 61.0f );  }
};

class gmap_dino_breakdown : public gmap
{
	virtual void		GetPos_Stroke_F9(VEC3D& pos) const override {  	i3Vector::Set( &pos, -80.0f, 2.0f, 0.0f );   }

};

class gmap_destroy_base : public gmap 
{
private:
	virtual void 	Process_Event_RoundStart_MissionNoticeString_From_GameMode() override;
};

class gmap_breakdown : public gmap_destroy_base
{
private:
	virtual void Process_InitObject(i3Object * pObj) override;
	virtual void Process_Event_RoundStart_MissionNoticeString_From_GameMode() override;
};

class gmap_helispot : public gmap_destroy_base
{
private:
	virtual void Process_InitObject(i3Object * pObj) override;
	virtual void Process_Event_RoundStart_MissionNoticeString_From_GameMode() override;
};

class gmap_giran_base : public gmap_destroy_base
{
private:
	virtual void Process_InitObject(i3Object * pObj) override;
	virtual void Process_Event_RoundStart_MissionNoticeString_From_GameMode() override;
};
class gmap_runaway : public gmap_destroy_base
{
private:
	virtual void Process_InitObject(i3Object * pObj) override;
	virtual void Process_Event_RoundStart_MissionNoticeString_From_GameMode() override;
};


class gmap_giran : public gmap_giran_base
{

};

class gmap_giran2 : public gmap_giran_base
{

};

class gmap_defense_base : public gmap 
{
private:
	virtual void		Process_Event_RoundStart_MissionNoticeString_From_GameMode() override;
};

class gmap_blackpanther : public gmap_defense_base
{
private:
	virtual void		Process_InitObject(i3Object * pObj) override;
	virtual void		Process_Event_RoundStart_MissionNoticeString_From_GameMode() override;
};

#endif