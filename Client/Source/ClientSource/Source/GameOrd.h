#if !defined( __GAME_ORD_H)
#define __GAME_ORD_H

void			create_gord(STAGE_UID uid);
void			destroy_gord();

template<> struct is_tinst_manual<class gord> : std::tr1::true_type {};

class gord 
{
public:
	static gord*		i() { return tinst<gord>(); }
	virtual void ChangeObjectSpecificName( i3Object* stage_obj) {}
	virtual	~gord() {}
};

class gord_uptown : public gord
{
private:
	virtual void ChangeObjectSpecificName( i3Object* stage_obj) override;
};

class gord_rushhour : public gord
{
private:
	virtual void ChangeObjectSpecificName( i3Object* stage_obj) override;
};

class gord_helispot : public gord
{
private:
	virtual void ChangeObjectSpecificName( i3Object* stage_obj) override;
};

class gord_outpost : public gord
{
private:
	virtual void ChangeObjectSpecificName( i3Object* stage_obj) override;
};

class gord_stormtube_base : public gord
{
private:
	virtual void ChangeObjectSpecificName( i3Object* stage_obj) override;
};

class gord_stormtube : public gord_stormtube_base
{

};

class gord_d_stormtube : public gord_stormtube_base
{

};

#endif