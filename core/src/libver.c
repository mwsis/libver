/* /////////////////////////////////////////////////////////////////////////
 * File:    libver.c
 *
 * Purpose: Implementation of the libver core API.
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


/** \file libver.c Implementation of the libver core API
 */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <libver/libver.h>
#include "libver.internal.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static int
scheme_wanted_(
    const char* schemes
,   const char* name
)
{
    assert(NULL != schemes);
    assert(NULL != name);

    if (0 == strcmp(schemes, LIBVER_SCHEMES_ALL))
    {
        return 1;
    }

    return 0 == strcmp(schemes, name);
}

static int
commit_hit_(
    libver_result_t*                result
,   libver_internal_hit_t const*    hit
)
{
    int const rc = libver_internal_result_set(result, hit);

    if (LIBVER_RC_SUCCESS != rc)
    {
        result->num_schemes = 0;
        result->schemes = NULL;
    }

    return rc;
}

static int
probe_scheme_(
    const char*             dir
,   const char*             schemes
,   const char*             name
,   int                     (*probe)(const char*, libver_internal_hit_t*)
,   libver_result_t*        result
,   int*                    done
)
{
    libver_internal_hit_t   hit;
    int                     rc;

    if (!scheme_wanted_(schemes, name))
    {
        return LIBVER_RC_SUCCESS;
    }

    rc = probe(dir, &hit);

    if (LIBVER_RC_NO_MATCH == rc)
    {
        return LIBVER_RC_SUCCESS;
    }

    *done = 1;

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    return commit_hit_(result, &hit);
}


/* /////////////////////////////////////////////////////////////////////////
 * API
 */

int
libver_init(
    void* reserved0
)
{
    ((void)reserved0);

    /* No global state yet. */
    return 0;
}

void
libver_uninit(void)
{
    /* No global state yet. */
}

int
libver_find(
    const char*         dir
,   int                 flags
,   const char*         schemes
,   libver_result_t*    result
)
{
    int rc;
    int done = 0;

    assert(NULL != dir);
    assert('\0' != *dir);
    assert(NULL != schemes);
    assert('\0' != *schemes);

    (void)flags;

    if (NULL == result)
    {
        return LIBVER_RC_INVALID;
    }

    result->num_schemes = 0;
    result->schemes = NULL;

    rc = libver_internal_check_dir(dir);

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    rc = probe_scheme_(
            dir
        ,   schemes
        ,   LIBVER_SCHEME_CARGO
        ,   libver_backend_cargo_probe
        ,   result
        ,   &done
        );

    if (done || LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    rc = probe_scheme_(
            dir
        ,   schemes
        ,   LIBVER_SCHEME_ZIG
        ,   libver_backend_zig_probe
        ,   result
        ,   &done
        );

    if (done || LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    return LIBVER_RC_NO_MATCH;
}

void
libver_result_free(
    libver_result_t*    result
)
{
    if (NULL == result)
    {
        return;
    }

    free(result->schemes);

    result->num_schemes = 0;
    result->schemes = NULL;
}


/* ///////////////////////////// end of file //////////////////////////// */
