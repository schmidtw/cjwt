/**
 * Copyright 2017-2019 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef __CJWT_H__
#define __CJWT_H__

#include <stdint.h>
#include <time.h>
#include <cJSON.h>

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define OPT_ALLOW_ALG_NONE              (1<<0)
#define OPT_ALLOW_ANY_TIME              (1<<1)
#define OPT_ALLOW_ALG_NONE_IGNORE_SIG   (1<<2)


/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef enum {
    alg_none = 0,
    alg_es256,
    alg_es384,
    alg_es512,
    alg_hs256,
    alg_hs384,
    alg_hs512,
    alg_ps256,
    alg_ps384,
    alg_ps512,
    alg_rs256,
    alg_rs384,
    alg_rs512,
    num_algorithms
} cjwt_alg_t;

typedef struct {
    cjwt_alg_t  alg;

    /* Unsupported:
     *  jku
     *  jwk
     *  x5u
     *  x5c
     *  x5t
     *  x5ts256
     *  type
     *  cty
     *  crit
     */
} cjwt_header_t;

typedef struct {
    cjwt_header_t header;

    char *iss;
    char *sub;
    char *jti;

    size_t   aud_count;
    char   **aud_names;

    struct timespec *exp;
    struct timespec *nbf;
    struct timespec *iat;

    cJSON *private_claims;
} cjwt_t;

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/**
 *  The function to use to decode and validate a JWT.
 *
 *  @note This function allocates memory associated with the output jwt that
 *        must be freed.  cjwt_destroy() must be called to destroy the object
 *        when we are done with it.
 *
 *  @note This function does not allow alg_none to pass unless the
 *        OPT_ALLOW_ALG_NONE is passed.  In this case the signature *MUST*
 *        be empty to pass.  Example: "<header>.<payload."
 *
 *        If you must allow a alg_none and a signature to be present an
 *        additional option must be set: OPT_ALLOW_ALG_NONE_IGNORE_SIG.
 *
 *        Example of setting both:
 *          OPT_ALLOW_ALG_NONE|OPT_ALLOW_ALG_NONE_IGNORE_SIG
 *
 *  @note If present, the nbf and exp claims are enforced against the 'now'
 *        parameter unless the OPT_ALLOW_ANY_TIME option flag is set.
 *          Valid if: (nbf - skew) < now < (exp - skew)
 *        Overflow is detected & reported as ETIME.
 *
 *  @note If the function can't validate, or the secrets do not validate, no
 *        cjwt_t structure is returned as a safety precaution.
 *
 *  @param encoded [IN]  the incoming encoded JWT (MUST be '\0' terminated string)
 *  @param options [IN]  a bitmask of the options
 *  @param jwt     [OUT] the resulting JWT if found to be valid,
 *                       set to NULL if not successful
 *  @param key     [IN]  the public key to use for validating the signature
 *  @param key_len [IN]  the length of the key in bytes
 *  @param now     [IN]  the time to use to validate the JWT
 *  @param skew    [IN]  the allowable clock skew (in seconds) to permit when
 *                       validating the time window.
 *
 *  @retval  0       successful
 *  @retval  EINVAL  invalid jwt format or mismatched key
 *  @retval  ENOMEM  unable to allocate needed memory
 *  @retval  ENOTSUP unsupported algorithm
 *  @retval  ETIME   the JWT validity window has expired
 */
int cjwt_decode( const char *encoded, unsigned int options, cjwt_t **jwt,
                 const uint8_t *key, size_t key_len, time_t now, time_t skew );

/**
 *  The function to free cjwt object.
 *
 *  @note Cleanup funtion for corresponding cjwt
 *
 *  @param jwt  [IN] the to be freed cjwt
 *
 *  @retval   0 successful
 */
int cjwt_destroy( cjwt_t **jwt );

#endif
