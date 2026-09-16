/* /////////////////////////////////////////////////////////////////////////
 * File:    libver/libver.h
 *
 * Purpose: Definition of the libver core API.
 *
 * Created: 10th August 2026
 * Updated: 16th September 2026
 *
 * Home:    http://synesis.com.au/software/
 *
 * Copyright (c) 2026, Matthew Wilson and Synesis Information Systems
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the names of Matthew Wilson and Synesis Information Systems nor
 *   the names of any contributors may be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


/** \file libver/libver.h Definition of the libver core API
 */

#ifndef LIBVER_INCL_LIBVER_H_LIBVER
#define LIBVER_INCL_LIBVER_H_LIBVER


/* /////////////////////////////////////////////////////////////////////////
 * version
 */

/** \def LIBVER_VER_MAJOR
 * The major version number of Synesis Information Systems' libver.
 */

/** \def LIBVER_VER_MINOR
 * The minor version number of Synesis Information Systems' libver.
 */

/** \def LIBVER_VER_PATCH
 * The patch version number of Synesis Information Systems' libver.
 */

/** \def LIBVER_VER_ALPHABETA
 * The alpha/beta/rc designator of Synesis Information Systems' libver,
 * where alpha versions are from 0x41, beta versions are from 0x81, release
 * candidates are from 0xC1, and release versions are 0xFF.
 */

/** \def LIBVER_VER
 * The current composite version number of Synesis Information Systems'
 * libver.
 */


#define LIBVER_VER_MAJOR        0
#define LIBVER_VER_MINOR        0
#define LIBVER_VER_PATCH        2
#define LIBVER_VER_ALPHABETA    0xFF
#define LIBVER_VER_REVISION     LIBVER_VER_PATCH

#define LIBVER_VER \
    (0\
        |   (   LIBVER_VER_MAJOR       << 24   ) \
        |   (   LIBVER_VER_MINOR       << 16   ) \
        |   (   LIBVER_VER_PATCH       <<  8   ) \
        |   (   LIBVER_VER_ALPHABETA   <<  0   ) \
    )

#ifndef SYNSOFT_DOCUMENTATION_SKIP_SECTION
# define LIBVER_VER_REVISION    LIBVER_VER_PATCH
#endif /* !SYNSOFT_DOCUMENTATION_SKIP_SECTION */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

 #include <stddef.h>


 /* /////////////////////////////////////////////////////////////////////////
 * language
 */

#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

struct libver_scheme_result_t
{
    const char* scheme;
    int         major;
    int         minor;
    int         path;
    int         alphabeta;
    int         build;
    const char* version;
};
#ifndef __cplusplus
typedef struct libver_scheme_result_t                       libver_scheme_result_t;
#endif /* !__cplusplus */

typedef struct libver_result_ctxt_t libver_result_ctxt_t;


struct libver_result_t
{
    size_t                  num_schemes;
    libver_scheme_result_t* schemes;
};
#ifndef __cplusplus
typedef struct libver_result_t                              libver_result_t;
#endif /* !__cplusplus */


/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Initialises the API
 *
 * @note This function is thread-safe, and may be called multiple times.
 *   Each additional call increments a reference count, and each successful
 *   call must be matched by a corresponding call to libver_uninit().
 *
 * @return
 *   - 0 on success;
 *   - non-zero on failure, being an error code;
 */
int
libver_init(
    void*
);

/** Uninitialises the API
 *
 * @note This function is thread-safe, and may be called multiple times.
 *   Each additional call decrements a reference count, and the final call
 *   must match the initial call to libver_init().
 *
 * @return
 *   - 0 on success;
 *   - non-zero on failure, being an error code;
 *
 * @pre Behaviour is undefined if called more times than libver_init() has
 *   returned 0.
 */
void
libver_uninit(void);

/** Finds the definitive project version(s) for @a dir.
 *
 * @param dir
 *   The directory to search for project version(s);
 * @param flags
 *   The flags to use for the search;
 * @param schemes
 *   The schemes to use for the search;
 * @param result
 *   The result of the search;
 *
 * @return
 *   - 0 on success;
 *   - non-zero on failure, being an error code;
 *
 * @pre Behaviour is undefined if @a dir is NULL or empty;
 * @pre Behaviour is undefined if @a flags is not a valid flag;
 * @pre Behaviour is undefined if @a schemes is NULL or empty;
 * @pre Behaviour is undefined if @a result is NULL;
 */
int
libver_find(
    const char*         dir     /* [in] */
,   int                 flags   /* [in] */
,   const char*         schemes /* [in] */
,   libver_result_t*    result  /* [out] */
);

/** Release storage owned by @a result.
 *
 * @param result
 *   The result to release;
 *
 * @pre Behaviour is undefined if @a result is NULL.
 */
void
libver_result_free(
    libver_result_t*    result
);


/* /////////////////////////////////////////////////////////////////////////
 * language
 */

#ifdef __cplusplus
} /* extern "C" */
#endif


/* /////////////////////////////////////////////////////////////////////////
 * inclusion control
 */

#ifdef STLSOFT_CF_PRAGMA_ONCE_SUPPORT
# pragma once
#endif /* STLSOFT_CF_PRAGMA_ONCE_SUPPORT */

#endif /* !LIBVER_INCL_LIBVER_H_LIBVER */

/* ///////////////////////////// end of file //////////////////////////// */
