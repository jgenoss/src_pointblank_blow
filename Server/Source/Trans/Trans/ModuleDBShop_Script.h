
//////////////////////////////////////////////////////////////////////////
// Publisher
// [BillingDB_NorthA] CLink_NorthA::InputProcess 
#define USP_PRC_BUYITEM_BYPB			"EXECUTE prc_BuyItem_byPB '%s','%s',%d,'%s',%d,%d,%d "
// [BillingDB_Taiwan] CLink_Taiwan::InputProcess 
#define USP_SP_DEDUCTCASHBUYGOODS		"EXECUTE sp_deductCashBuyGoods '%s','%s',%d,'%s',%d,%d " // id, nick, itemid, itemname, itemcount, price


//////////////////////////////////////////////////////////////////////////
// [ZP_LOG] 
#define USP_60200			"EXEC usp_60200_zplog_loaduserdcash_m %d;"
#define USP_60500			"EXEC usp_60500_zplog_insertsalecash %d,%d;"

//////////////////////////////////////////////////////////////////////////
// [PB_User]
#define USP_SITEM_DELIVER_UPD			"EXEC usp_SItemDeliver_upd %d,'%c','%s';"
#define USP_SITEM_DELIVER_SEL			"EXEC usp_SItemDeliver_sel %d;"
#define USP_SITEM_DELIVER_SEL2			"EXEC usp_SItemDeliver_sel2 %d ;"
#define USP_SITEM_DELIVER_UPD2			"EXEC usp_SItemDeliver_upd2 %d,'%s' ;"

#define USP_SUSER_WARE_SEL				"EXEC usp_SUserWare_sel %d,%I64d"
#define USP_SUSER_WARE_SEL2				"EXEC usp_SUserWare_sel2 %d,%I64d,%d;"
#define USP_SUSER_WARE_SEL3				"EXEC usp_SUserWare_sel3 %d,%I64d;"
#define USP_SUSER_WARE_SEL4				"EXEC usp_SUserWare_sel4 %d,%I64d,%I64d,%d;"
#define USP_SUSER_WARE_SEL5				"EXEC usp_SUserWare_sel5 %d,%I64d,%d;"
#define USP_SUSER_WARE_SEL6				"EXEC usp_SUserWare_sel6 %d,%I64d,%I64d;"
#define USP_SUSER_WARE_SEL7				"EXEC usp_SUserWare_sel7 %d,%I64d,%I64d;"
#define USP_SUSER_WARE_SEL8				"EXEC usp_SUserWare_sel8 %d, %I64d, %I64d;"
#define USP_SUSER_WARE_SEL9				"EXEC usp_SUserWare_sel9 %d,%I64d,%d,%d;"

#define USP_SUSER_WARE_UPD				"EXEC usp_SUserWare_upd %d,%d,%I64d,%I64d;"
#define USP_SUSER_WARE_UPD2				"EXEC usp_SUserWare_upd2 %d,%d,%d,%I64d,%I64d;"
#define USP_SUSER_WARE_UPD3				"EXEC usp_SUserWare_upd3 %d,%I64d,%I64d,%d"
#define USP_SUSER_WARE_UPD4				"EXEC usp_SUserWare_upd4 %d,%I64d,%I64d,%d;"

#define USP_SUSER_WARE_DEL				"EXEC usp_SUserWare_del %d,%I64d,%I64d"

#define USP_SUSER_GIFT_INS				"EXEC usp_SUserGift_ins %d,%I64d,%d, %d, '%s','%s';"
#define USP_SUSER_GIFT_SEL				"EXEC usp_SUserGift_sel %d,%I64d;"
#define USP_SUSER_GIFT_SEL2				"EXEC usp_SUserGift_sel2 %d,%I64d,%d;"
#define USP_SUSER_GIFT_DEL				"EXEC usp_SUserGift_del %d,%I64d,%d;"
#define USP_SUSER_GIFT_UPD				"EXEC usp_SUserGift_upd %d,%I64d,%d,%d;"

#define USP_SGIFT_DELIVER_UPD			"EXEC usp_SGiftDeliver_upd %d,%d;"
#define USP_SGIFT_DELIVER_SEL			"EXEC usp_SGiftDeliver_sel %d ;"
#define USP_SSHOP_COUPON_SCAN			"EXEC usp_SShopCoupon_scan"
#define USP_SUSER_WARE_CHECK_SALE_COUPON_COUNT	"EXEC usp_SUserWare_CheckSaleCouponCount %d, %I64d;"


#define USP_SUSER_WARE_UPD_STATE		"EXEC usp_SUserWare_upd_State %d,%I64d,%I64d,%d;"  