#ifndef __SHOP_CONTEXT_BASKET__H
#define __SHOP_CONTEXT_BASKET__H

class CShopContext;

class CShopBasket
{
public:
	struct Basket
	{
		SHOP_GOODS_PACK * pack;
		int packInx;	//pack index

		int dpinx;	//dpInx
		int type;	//Price Type

		Basket(SHOP_GOODS_PACK * pac);
	};
	typedef std::vector< Basket > BasketList;

private:
	BasketList m_BasketList;

	T_ItemDBIdx		m_nCouponDBIndex[USE_BASKET_COUNT];

public:
	CShopBasket();
	~CShopBasket(void);

private:
	CShopContext* m_pParent;

public:
	void set_parent(CShopContext* parent)	{	m_pParent = parent;	}

	void clear();
	void push(SHOP_GOODS_PACK* pGood);
	void pop(size_t index = 0);
	
	size_t size() const	{	return m_BasketList.size();	}

	bool is_pack_price_dual(size_t pack);

	void set_pack_inx(size_t pack, int packInx);
	int get_pack_inx(size_t pack) const	{	return m_BasketList[ pack ].packInx;	}

	void set_pack_dpinx(size_t pack, int dpinx);
	int get_pack_dpinx(size_t pack) const	{	return m_BasketList[ pack ].dpinx;	}

	void set_pack_type(size_t pack, int type);
	int get_pack_type(size_t pack) const	{	return m_BasketList[ pack ].type;	}
	
	SHOP_GOODS* get_sel(size_t pack) const;
	SHOP_GOODS* get(size_t pack, size_t goods = 0) const;
	SHOP_GOODS_PACK* operator[](size_t index) const;

	void make_packet(BUY_BASKET * stBasket) const;
	bool duplicate(SHOP_GOODS_PACK* pGood) const;

	void SetCouponIndex(INT32 slot, T_ItemDBIdx CouponIdx) {	m_nCouponDBIndex[slot] = CouponIdx; }
	T_ItemDBIdx GetCouponIndex(INT32 slot) const { return m_nCouponDBIndex[slot]; }
};

#endif