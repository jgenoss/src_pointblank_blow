#ifndef INNOVA_BILLING_INTEGRATION_API_DEPRECATED_H
#define INNOVA_BILLING_INTEGRATION_API_DEPRECATED_H

/*!
 * \file    integration_api_deprecated.h
 * \brief   This header file contains server-side integration API 
 *          for 4game billing system that is deprecated and shouldn't be used anymore!!.
 *
 * Copyright (c) Innova Group: All Rights Reserved.
 *
 * \date    created 21-10-2011.
 *   function extensions are - {_w, _a, _u}:
 *      w - means that string param is in UNICODE encoding,
 *      a - means that string param is in ascii encoding (by default cyrillic ascii, can be changed in config),
 *      u - means that string param is in utf8 encoding, as unsigned char* string
 */

//! \brief deprecated login_struct
struct login_result
{
    uint_64 user_id;
    double user_balance;
    uint_64 loyalty_points;
    int user_status;
    int user_action;
};// struct login_result

#ifdef __GNUC__
    #ifdef DEPRECATED
    #undef DEPRECATED
    #endif
    #define DEPRECATED(func) func 
#endif

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
//! \defgroup api_group Public Functions
//! \{

    //! \defgroup api_group Deprecated functions (supported for backward compatibility)
    //! \{
    integration_api int DEPRECATED(innbill_login_w)( __in_param const wchar_t* user_name
                                       , __in_param const wchar_t* password
                                       , __in_param int exp_time
                                       , __in_param uint_64 ip
                                       , __out_param login_result* result);

    integration_api int DEPRECATED(innbill_login_a)( __in_param const char* user_name
                                       , __in_param const char* password
                                       , __in_param int exp_time
                                       , __in_param uint_64 ip
                                       , __out_param login_result* result);

    integration_api int DEPRECATED(innbill_login_u)( __in_param const unsigned char* user_name
                                       , __in_param const unsigned char* password
                                       , __in_param int exp_time
                                       , __in_param uint_64 ip
                                       , __out_param login_result* result);

    integration_api int DEPRECATED(innbill_check_user_status_i)( __in_param uint_64 user_id
                                                   , __out_param double* user_balance
                                                   , __out_param int* status);

   integration_api int DEPRECATED(innbill_check_user_status_w)( __in_param const wchar_t* user_name
                                                   , __out_param double* user_balance
                                                   , __out_param int* status);

    integration_api int DEPRECATED(innbill_check_user_status_a)( __in_param const char* user_name
                                                   , __out_param double* user_balance
                                                   , __out_param int* status);

    integration_api int DEPRECATED(innbill_check_user_status_u)( __in_param const unsigned char* user_name
                                                   , __out_param double* user_balance
                                                   , __out_param int* status);

    integration_api int DEPRECATED(innbill_check_user_status_ex_i)( __in_param uint_64 user_id
                                                      , __out_param double* user_balance
                                                      , __out_param int* status
                                                      , __out_param uint_64* time_remaining);

    integration_api int DEPRECATED(innbill_check_user_status_ex_w)( __in_param const wchar_t* user_name
                                                      , __out_param double* user_balance
                                                      , __out_param int* status
                                                      , __out_param uint_64* time_remaining);

    integration_api int DEPRECATED(innbill_check_user_status_ex_a)( __in_param const char* user_name
                                                      , __out_param double* user_balance
                                                      , __out_param int* status
                                                      , __out_param uint_64* time_remaining);

    integration_api int DEPRECATED(innbill_check_user_status_ex_u)( __in_param const unsigned char* user_name
                                                      , __out_param double* user_balance
                                                      , __out_param int* status
                                                      , __out_param uint_64* time_remaining);

    integration_api int DEPRECATED(innbill_logout_i)( __in_param uint_64 user_id
                                        , __in_param uint_64 time_elapsed);

    integration_api int DEPRECATED(innbill_logout_w)( __in_param const wchar_t* user_name
                                        , __in_param uint_64 time_elapsed);

    integration_api int DEPRECATED(innbill_logout_a)( __in_param const char* user_name
                                        , __in_param uint_64 time_elapsed);

    integration_api int DEPRECATED(innbill_logout_u)( __in_param const unsigned char* user_name
                                        , __in_param uint_64 time_elapsed);

    integration_api int DEPRECATED(innbill_buy_item_iiw)( __in_param uint_64 user_id
                                            , __in_param uint_64 item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const wchar_t* server_name
                                            , __in_param const wchar_t* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_iia)( __in_param uint_64 user_id
                                            , __in_param uint_64 item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const char* server_name
                                            , __in_param const char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_iiu)( __in_param uint_64 user_id
                                            , __in_param uint_64 item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const unsigned char* server_name
                                            , __in_param const unsigned char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_iww)( __in_param uint_64 user_id
                                            , __in_param const wchar_t* item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const wchar_t* server_name
                                            , __in_param const wchar_t* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_iaa)( __in_param uint_64 user_id
                                            , __in_param const char* item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const char* server_name
                                            , __in_param const char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_iuu)( __in_param uint_64 user_id
                                            , __in_param const unsigned char* item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const unsigned char* server_name
                                            , __in_param const unsigned char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_wiw)( __in_param const wchar_t* user_name
                                            , __in_param uint_64 item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const wchar_t* server_name
                                            , __in_param const wchar_t* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_aia)( __in_param const char* user_name
                                            , __in_param uint_64 item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const char* server_name
                                            , __in_param const char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_uiu)( __in_param const unsigned char* user_name
                                            , __in_param uint_64 item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const unsigned char* server_name
                                            , __in_param const unsigned char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_www)( __in_param const wchar_t* user_name
                                            , __in_param const wchar_t* item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const wchar_t* server_name
                                            , __in_param const wchar_t* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_aaa)( __in_param const char* user_name
                                            , __in_param const char* item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const char* server_name
                                            , __in_param const char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_buy_item_uuu)( __in_param const unsigned char* user_name
                                            , __in_param const unsigned char* item_id
                                            , __in_param int item_type
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const unsigned char* server_name
                                            , __in_param const unsigned char* character_name
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int DEPRECATED(innbill_cancel_buy_item_i)( __in_param uint_64 user_id
                                                 , __in_param uint_64 service_transaction_id
                                                 , __in_param uint_64 billing_transaction_id);

    integration_api int DEPRECATED(innbill_cancel_buy_item_w)( __in_param const wchar_t* user_name
                                                 , __in_param uint_64 service_transaction_id
                                                 , __in_param uint_64 billing_transaction_id);

    integration_api int DEPRECATED(innbill_cancel_buy_item_a)( __in_param const char* user_name
                                                 , __in_param uint_64 service_transaction_id
                                                 , __in_param uint_64 billing_transaction_id);

    integration_api int DEPRECATED(innbill_cancel_buy_item_u)( __in_param const unsigned char* user_name
                                                 , __in_param uint_64 service_transaction_id
                                                 , __in_param uint_64 billing_transaction_id);
    //! \}

    //! \}
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //INNOVA_BILLING_INTEGRATION_API_DEPRECATED_H

