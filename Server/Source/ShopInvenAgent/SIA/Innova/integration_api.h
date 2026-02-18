#ifndef INNOVA_BILLING_INTEGRATION_API_H
#define INNOVA_BILLING_INTEGRATION_API_H

/*!
 * \file    integration_api.h
 * \brief   This header file contains server-side integration API
 *          for 4game billing system.
 *
 * Copyright (c) Innova Group: All Rights Reserved.
 *
 * \date    created 25-6-2009.
 *   function extensions are - {_w, _a, _u}:
 *      w - means that string param is in UNICODE encoding,
 *      a - means that string param is in ascii encoding (by default cyrillic ascii, can be changed in config),
 *      u - means that string param is in utf8 encoding, as unsigned char* string
 */

#define INNOVA_BILLING_INTEGRATION_API_VERSION 32006

#if defined(_WIN32) || defined(_WIN64)
    #ifdef integration_exports
    #define integration_api __declspec(dllexport)
    #else //integration_exports
    #define integration_api __declspec(dllimport)
    #endif //integration_exports
    typedef unsigned __int64 uint64;
    typedef __int64 int64;
#else // assuming defined linux
    #define integration_api
    #include <inttypes.h>
    typedef uint64_t uint64;
    typedef int64_t int64;
#endif //_WIN32 || _WIN64

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#define DEPRECATED(func) func
#endif

#define __in_param    // for input parameters
#define __out_param   // for output parameters
typedef uint64 uint_64;
typedef int64 int_64;

#define INNBILL_MAX_PASSWORD2_SIZE 32    //!< used in two password scenario (size specified in char's)

//! \brief The innbill_login_result2_a struct.
//!
//!        The object of this type should be passed as an output
//!        parameter to the login2(...) function. The library
//!        initializes it itself.
struct innbill_login_result2_a
{
    uint_64 user_id;                //!< The user ID in Billing DB.
    double user_balance;            //!< The user's balance.
    uint_64 loyalty_points;         //!< User loyalty points
    int account_flags;              //!< Meaning of flags depends on the service
    int account_level;              //!< 1 - free account, 2 - paid/premium account, 3 - trial account. The rest levels depend on the service/game
    char password2[INNBILL_MAX_PASSWORD2_SIZE]; //!< secondary password, used in two password scenario
};// struct innbill_login_result2_a

//! \brief The innbill_login_result2_a struct.
//!
//!        The object of this type should be passed as an output
//!        parameter to the login2(...) function. The library
//!        initializes it itself.
struct innbill_login_result2_u
{
    uint_64 user_id;                //!< The user ID in Billing DB.
    double user_balance;            //!< The user's balance.
    uint_64 loyalty_points;         //!< User loyalty points
    int account_flags;              //!< Meaning of flags depends on the service
    int account_level;              //!< 1 - free account, 2 - paid/premium account, 3 - trial account. The rest levels depend on the service/game
    unsigned char password2[INNBILL_MAX_PASSWORD2_SIZE]; //!< secondary password, used in two password scenario
};// struct innbill_login_result2_u

//! \brief The innbill_login_result2_w struct.
//!
//!        The object of this type should be passed as an output
//!        parameter to the login2(...) function. The library
//!        initializes it itself.
struct innbill_login_result2_w
{
    uint_64 user_id;                //!< The user ID in Billing DB.
    double user_balance;            //!< The user's balance.
    uint_64 loyalty_points;         //!< User loyalty points
    int account_flags;              //!< Meaning of flags depends on the service
    int account_level;              //!< 1 - free account, 2 - paid/premium account, 3 - trial account. The rest levels depend on the service/game
    wchar_t password2[INNBILL_MAX_PASSWORD2_SIZE]; //!< secondary password, used in two password scenario
};// struct innbill_login_result2_w

//! \brief The innbill_check_user_status_result struct.
//!
//!        The object of this type should be passed as an output
//!        parameter to the inncheck_status_result2_a(...) function. The library
//!        initializes it itself.
struct innbill_check_user_status_result
{
    double user_balance;            //!< The user's balance.
    uint_64 loyalty_points;         //!< User loyalty points
    int account_flags;              //!< Meaning of the flags depends on the service
    int account_level;              //!< 1 - free account, 2 - paid/premium account, 3 - trial account. The rest levels depend on the service/game
};// struct innbill_check_user_status_result

#define INNBILL_MAX_URL_SIZE 512     //!< maximum size of URL in character count
#define INNBILL_MAX_URL_TYPE 7       //!< types of web url returned in get_web_auth_url_*

//! \brief The innbill_get_web_auth_url_result_a struct.
//!
//!        The object of this type should be passed as an output
//!        parameter to the innbill_get_web_auth_url_a(...) function. The library
//!        initializes it itself.
struct innbill_get_web_auth_url_result_a
{
    char url[INNBILL_MAX_URL_SIZE];
};// struct innbill_get_web_auth_url_result_a

struct innbill_get_web_auth_url_result_u
{
    unsigned char url[INNBILL_MAX_URL_SIZE];
};// struct innbill_get_web_auth_url_result_u

struct innbill_get_web_auth_url_result_w
{
    wchar_t url[INNBILL_MAX_URL_SIZE];
};// struct innbill_get_web_auth_url_result_w

#include "integration_api_deprecated.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
//! \defgroup api_group Public Functions
//! \{

//////////////////////////////////////////////////////////////////////////
//! \defgroup init_group Initialization/Uninitialization
//! \{

    //! \brief  Initializes the library configuration parameters from
    //!         configuration file.
    //!
    //! \param  service_id  [out]   The unique service ID in Billing System.
    //!
    //! \return If no error occurs, returns 0. Otherwise, an error code is returned,
    //!         A specific error description can be retrieved by calling error_description(...).
    //!
    //! \remark This function should be the first function called from
    //!         integration_api library
    integration_api int innbill_initialize(__out_param uint_64* service_id);

    integration_api int innbill_initialize_a(__in_param const char* configPath,
                                             __out_param uint_64* service_id);

    integration_api int innbill_initialize_u(__in_param const unsigned char* configPath,
                                             __out_param uint_64* service_id);

    integration_api int innbill_initialize_w(__in_param const wchar_t* configPath,
                                             __out_param uint_64* service_id);

    //! \brief  Frees all resources allocated by this library.
    //! \remark This function should be the last function called from
    //!         integration_api library
    integration_api int innbill_uninitialize();


//! \}

//////////////////////////////////////////////////////////////////////////

//! \defgroup login_group Authentication and Authorization
//! \{


    //! \brief Called when a user authorizes, has more details about user account and allows two password scenario
    //!
    //! \param user_name    [in]     The user name.
    //! \param password     [in]     The user password.
    //! \param exp_time     [in]     The session expiration time in seconds, 0 if no expiration time.
    //! \param ip           [in]     IP address of the client.
    //! \param flags        [in]     Authentication flags: 0 - default, 1 - use two password scenario (return password2)
    //!
    //! \param result       [out]    Resulting authorization info: see innbill_login_result2 structs
    //!
    //! \return If no error occurs, returns 0. Otherwise, an error code is returned,
    //!         A specific error description can be retrieved by calling error_description(...).
    integration_api int innbill_login2_w( __in_param const wchar_t* user_name
                                       , __in_param const wchar_t* password
                                       , __in_param int exp_time
                                       , __in_param const wchar_t* ip
                                       , __in_param int flags
                                       , __out_param innbill_login_result2_w* result);

    integration_api int innbill_login2_a( __in_param const char* user_name
                                       , __in_param const char* password
                                       , __in_param int exp_time
                                       , __in_param const char* ip
                                       , __in_param int flags
                                       , __out_param innbill_login_result2_a* result);

    integration_api int innbill_login2_u( __in_param const unsigned char* user_name
                                       , __in_param const unsigned char* password
                                       , __in_param int exp_time
                                       , __in_param const unsigned char* ip
                                       , __in_param int flags
                                       , __out_param innbill_login_result2_u* result);

    //! \brief Should be called when the user logged out.
    //!
    //! \param user_id           [in] The user ID in Billing DB.
    //! \param user_name         [in] The user name (ignored if user_id is not equal to zero), 
    //                                    you need to specify one of the 'user_id' or 'user_name' parameters

    //! \param ip                [in] ip address of user
    //! \param time_elapsed      [in] The elapsed time(minutes).
    //!
    //! \return If no error occurs, returns 0. Otherwise, an error code is returned,
    //!         A specific error description can be retrieved by calling error_description(...).
    integration_api int innbill_logout2_w( __in_param uint_64 user_id
                                        , __in_param const wchar_t* user_name
                                        , __in_param const wchar_t* ip
                                        , __in_param uint_64 time_elapsed);

    integration_api int innbill_logout2_a( __in_param uint_64 user_id
                                        , __in_param const char* user_name
                                        , __in_param const char* ip
                                        , __in_param uint_64 time_elapsed);

    integration_api int innbill_logout2_u( __in_param uint_64 user_id
                                        , __in_param const unsigned char* user_name
                                        , __in_param const unsigned char* ip
                                        , __in_param uint_64 time_elapsed);

    //! \}

    //////////////////////////////////////////////////////////////////////////

    //! \defgroup status_group Status Checking
    //! \{

    //! \brief Checks user status. (for time based services)
    //!
    //! \param user_id           [in] The user ID in Billing DB.
    //! \param user_name         [in] The user name (ignored if user_id is not equal to zero), 
    //                                    you need to specify one of the 'user_id' or 'user_name' parameters

    //! \param result           [out] See description for innbill_check_user_status_result
    //!
    //! \return If no error occurs, returns 0.
    //!         Otherwise, an error code is returned, A specific error
    //!         description can be retrieved by calling error_description(...).
    integration_api int innbill_check_user_status2_w( __in_param uint_64 user_id
                                                    , __in_param const wchar_t* user_name
                                                    , __out_param innbill_check_user_status_result* result);

    integration_api int innbill_check_user_status2_a( __in_param uint_64 user_id
                                                    , __in_param const char* user_name
                                                    , __out_param innbill_check_user_status_result* result);

    integration_api int innbill_check_user_status2_u( __in_param uint_64 user_id
                                                    , __in_param const unsigned char* user_name
                                                    , __out_param innbill_check_user_status_result* result);

    //! \}


    //////////////////////////////////////////////////////////////////////////

    //! \defgroup selling_group Item Selling
    //! \{

    //! \brief Called when user buys item in a service(game).
    //!
    //! \param user_id                  [in] The user ID in Billing DB.
    //! \param user_name                [in] The user name (ignored if user_id is not equal to zero), 
    //!                                 you need to specify one of the 'user_id' or 'user_name' parameters
    //!                                 if user_id is 0 and the user_name is NULL, then URL still generated, but user should provide
    //!                                 user_name and password to authenticate at the web site.

    //! \param item_id                  [in] The item ID.
    //! \param item_price               [in] The item price or overall price(depend on item_price_type).
    //! \param item_price_type          [in] The type of previous param (item_price)
    //!                                      0 - item_price is an item price and must be checked within the function.
    //!                                      1 - item_price is an item price and mustn't be checked within the function.
    //!                                      2 - item_price is an overall price and must be checked within the function.
    //!                                      3 - item_price is an overall price and mustn't be checked within the function.
    //! \param item_count               [in] The item count.
    //! \param service_transaction_id   [in] The service transaction ID.
    //! \param server_name              [in] The server name.
    //! \param character_name           [in] The character name.
    //! \param ip                       [in] IP address
    //! \param billing_transaction_id   [out] The billing transaction ID.
    //! \param user_balance             [out] The current amount of money.
    //!
    //! \return If no error occurs, returns 0.
    //!         Otherwise, an error code is returned, A specific error
    //!         description can be retrieved by calling error_description(...).
    integration_api int innbill_buy_item2_w( __in_param uint_64 user_id
                                            , __in_param const wchar_t* user_name
                                            , __in_param const wchar_t* item_id
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const wchar_t* server_name
                                            , __in_param const wchar_t* character_name
                                            , __in_param const wchar_t* ip
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int innbill_buy_item2_a( __in_param uint_64 user_id
                                            , __in_param const char* user_name
                                            , __in_param const char* item_id
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const char* server_name
                                            , __in_param const char* character_name
                                            , __in_param const char* ip
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    integration_api int innbill_buy_item2_u( __in_param uint_64 user_id
                                            , __in_param const unsigned char* user_name
                                            , __in_param const unsigned char* item_id
                                            , __in_param double item_price
                                            , __in_param int item_price_type
                                            , __in_param int item_count
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param const unsigned char* server_name
                                            , __in_param const unsigned char* character_name
                                            , __in_param const unsigned char* ip
                                            , __out_param uint_64* billing_transaction_id
                                            , __out_param double* user_balance);

    //////////////////////////////////////////////////////////////////////////

    //! \brief Cancels transaction.
    //!
    //! \param user_id                  [in] The user ID.
    //! \param service_transaction_id   [in] The service transaction ID.
    //! \param billing_transaction_id   [in] The billing transaction ID.
    //!
    //! \return If no error occurs, returns 0.
    //!         Otherwise, an error code is returned, A specific error
    //!         description can be retrieved by calling error_description(...).

    integration_api int innbill_cancel_buy_item2_w( __in_param uint_64 user_id
                                            , __in_param const wchar_t* user_name
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param uint_64 billing_transaction_id);

    integration_api int innbill_cancel_buy_item2_a( __in_param uint_64 user_id
                                            , __in_param const char* user_name
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param uint_64 billing_transaction_id);

    integration_api int innbill_cancel_buy_item2_u( __in_param uint_64 user_id
                                            , __in_param const unsigned char* user_name
                                            , __in_param uint_64 service_transaction_id
                                            , __in_param uint_64 billing_transaction_id);

    //! \}

    //! \defgroup api_group Web/URL (single sign on)
    //! \{

    //! \brief  Returns URL that can be used to authenticate at billing web site
    //!
    //! \param user_id           [in] The user ID in Billing DB.
    //! \param user_name         [in] The user name (ignored if user_id is not equal to zero), 
    //!                               you need to specify one of the 'user_id' or 'user_name' parameters
    //!                               
    //! \param url_type          [in] The type of URL requested, possible values are:
    //!                                   1 - billing home page
    //!                                   2 - service/game home page
    //!                                   3 - user profile page
    //!                                   4 - payment form
    //!                                   5 - web shop
    //!                                   6 - support page
    //!                                   7 - report cheater form

    //! \param browser_type      [in] The type of browser, possible values are:
    //!                                   1 - default user browser
    //!                                   2 - in game browser (embedded)
    
    //! \param auth_type         [in] The authentication flag, possible values are:
    //!                                   1 - do not authenticate user automatically
    //!                                   2 - generate single sign on ticket and login user automatically

    //! \param world_id          [in] world unique identifier
    //! \param character_id      [in] user character unique identififer

    //! \param ip                [in] The ip of user.
    //! \param result            [out] Result struct contains an URL value
    //! \return If no error occurs, returns 0.
    //!         Otherwise, an error code is returned, A specific error
    //!         description can be retrieved by calling error_description(...).
   
	// REVIEW передача user_name бе?user_id больше не поддерживает? ИС'ом. нужн?переработать функционал по?эт?требование ?исправит?документацию
    integration_api int innbill_get_web_auth_url_a( __in_param uint_64 user_id
                                                , __in_param const char* user_name
                                                , __in_param int url_type
                                                , __in_param int browser_type
                                                , __in_param int auth_type
                                                , __in_param const char* world_id
                                                , __in_param const char* character_id
                                                , __in_param const char* ip
                                                , __out_param innbill_get_web_auth_url_result_a* result
                                                );

    integration_api int innbill_get_web_auth_url_u( __in_param uint_64 user_id
                                                , __in_param const unsigned char* user_name
                                                , __in_param int url_type
                                                , __in_param int browser_type
                                                , __in_param int auth_type
                                                , __in_param const unsigned char* world_id
                                                , __in_param const unsigned char* character_id
                                                , __in_param const unsigned char* ip
                                                , __out_param innbill_get_web_auth_url_result_u* result
                                                );

    integration_api int innbill_get_web_auth_url_w( __in_param uint_64 user_id
                                                , __in_param const wchar_t* user_name
                                                , __in_param int url_type
                                                , __in_param int browser_type
                                                , __in_param int auth_type
                                                , __in_param const wchar_t* world_id
                                                , __in_param const wchar_t* character_id
                                                , __in_param const wchar_t* ip
                                                , __out_param innbill_get_web_auth_url_result_w* result
                                                );
    //! \}

    //! \defgroup ban API
    //! \{

    //! \brief Ban user account (disable access into the game)
    //!
    //! \param user_id           [in] The user ID in Billing DB.
    //! \param user_name         [in] The user name (ignored if user_id is not equal to zero), 
    //!                               you need to specify one of the 'user_id' or 'user_name' parameters
    //! \param period_of_time    [in] Period of time in seconds to ban user, 0 - means permanent ban

    //! \param internal_reason   [in] internal and detailed reason of ban (doesn't shown to the user)
    //! \param public_reason     [in] public reason of ban, visible to user
    //! \param ip                [in] The ip of user.
    //! \return If no error occurs, returns 0.
    //!         Otherwise, an error code is returned, A specific error
    //!         description can be retrieved by calling error_description(...).
   
    integration_api int innbill_ban_user_a( __in_param uint_64 user_id
                                                , __in_param const char* user_name
                                                , __in_param const int period_of_time
                                                , __in_param const char* internal_reason
                                                , __in_param const char* public_reason
                                                , __in_param const char* ip
                                                );

    integration_api int innbill_ban_user_u( __in_param uint_64 user_id
                                                , __in_param const unsigned char* user_name
                                                , __in_param const int period_of_time
                                                , __in_param const unsigned char* internal_reason
                                                , __in_param const unsigned char* public_reason
                                                , __in_param const unsigned char* ip
                                                );

    integration_api int innbill_ban_user_w( __in_param uint_64 user_id
                                                , __in_param const wchar_t* user_name
                                                , __in_param const int period_of_time
                                                , __in_param const wchar_t* internal_reason
                                                , __in_param const wchar_t* public_reason
                                                , __in_param const wchar_t* ip
                                                );


    //! \}

    //! \defgroup api_group Error memory handling
    //! \{

    //////////////////////////////////////////////////////////////////////////

    //! \brief  Retrieves an error description from the error code.
    //!
    //! \param  error_code [in] The error code returned from the other library functions.
    //!
    //! \return error description.
    //! \remarks function extensions are - {_a, _w, _u}:
    integration_api char* innbill_error_description_a(int error_code);

    //! \copydoc innbill_error_description_a(int error_code)
    integration_api wchar_t* innbill_error_description_w(int error_code);

    //! \copydoc innbill_error_description_a(int error_code)
    integration_api unsigned char* innbill_error_description_u(int error_code);


    //! \brief  Frees memory allocated by an error_description(...).
    //!
    //! \param  err_description [in] The pointer to the memory allocated
    //!                              by error_description(...) function.
    //! \return If no error occurs, returns 0.
    //!         Otherwise, an error code is returned, A specific error
    //!         description can be retrieved by calling error_description(...).
    //! \remarks function extensions are - {_a, _w, _u}:
    integration_api int innbill_free_error_description_a(char* err_description);

    //! \copydoc innbill_free_error_description_a(char* err_description)
    integration_api int innbill_free_error_description_u(unsigned char* err_description);

    //! \copydoc innbill_free_error_description_a(char* err_description)
    integration_api int innbill_free_error_description_w(wchar_t* err_description);

    //! \}
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //INNOVA_BILLING_INTEGRATION_API_H

