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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

typedef struct scheme_desc_t
{
    char const* name;
    char const* marker;
    int         (*probe)(char const*, libver_internal_hit_t*);
} scheme_desc_t;


/* /////////////////////////////////////////////////////////////////////////
 * constants
 */

static scheme_desc_t const k_schemes[] =
{
    { LIBVER_SCHEME_CARGO, "Cargo.toml", libver_backend_cargo_probe },
    { LIBVER_SCHEME_ZIG, "build.zig.zon", libver_backend_zig_probe },
};


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static int
scheme_wanted_(
    char const* schemes
,   char const* name
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
marker_present_(
    char const* dir
,   char const* marker
,   char*       path
,   size_t      path_cap
)
{
    int const rc = libver_internal_regular_file_in_dir(
                        dir
                    ,   marker
                    ,   path
                    ,   path_cap
                    );

    return LIBVER_RC_SUCCESS == rc || LIBVER_RC_PARSE == rc;
}

static void
fill_other_ecosystem_warning_(
    libver_internal_warning_t*  w
,   char const*                 other_scheme
,   char const*                 other_source
,   char const*                 winner_scheme
)
{
    size_t source_len;

    assert(NULL != w);
    assert(NULL != other_scheme);
    assert(NULL != other_source);
    assert(NULL != winner_scheme);

    source_len = strlen(other_source);

    if (source_len >= LIBVER_INTERNAL_PATH_MAX)
    {
        source_len = LIBVER_INTERNAL_PATH_MAX - 1;
    }

    w->kind = LIBVER_WARNING_OTHER_ECOSYSTEM;
    w->scheme = other_scheme;
    memcpy(w->source, other_source, source_len);
    w->source[source_len] = '\0';
    snprintf(
        w->message
    ,   sizeof(w->message)
    ,   "also found %s at '%s' (using %s by precedence)"
    ,   other_scheme
    ,   w->source
    ,   winner_scheme
    );
}

static void
clear_result_(
    libver_result_t* result
)
{
    result->num_schemes = 0;
    result->schemes = NULL;
    result->num_warnings = 0;
    result->warnings = NULL;
}

static int
commit_hit_(
    libver_result_t*                    result
,   libver_internal_hit_t const*        hit
,   libver_internal_warning_t const*    warnings
,   size_t                              num_warnings
)
{
    int const rc = libver_internal_result_set(
                        result
                    ,   hit
                    ,   warnings
                    ,   num_warnings
                    );

    if (LIBVER_RC_SUCCESS != rc)
    {
        clear_result_(result);
    }

    return rc;
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
    char const*         dir
,   int                 flags
,   char const*         schemes
,   libver_result_t*    result
)
{
    int                         rc;
    size_t                      i;
    int                         have_winner = 0;
    libver_internal_hit_t       winner;
    libver_internal_warning_t   warns[LIBVER_INTERNAL_MAX_SCHEMES];
    size_t                      nwarns = 0;

    assert(NULL != dir);
    assert('\0' != *dir);
    assert(NULL != schemes);
    assert('\0' != *schemes);

    (void)flags;

    if (NULL == result)
    {
        return LIBVER_RC_INVALID;
    }

    clear_result_(result);

    rc = libver_internal_check_dir(dir);

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    for (i = 0; i < sizeof(k_schemes) / sizeof(k_schemes[0]); ++i)
    {
        scheme_desc_t const* spec = &k_schemes[i];

        if (!scheme_wanted_(schemes, spec->name))
        {
            continue;
        }

        if (!have_winner)
        {
            rc = spec->probe(dir, &winner);

            if (LIBVER_RC_NO_MATCH == rc)
            {
                continue;
            }

            if (LIBVER_RC_SUCCESS != rc)
            {
                return rc;
            }

            have_winner = 1;
        }
        else if (nwarns < LIBVER_INTERNAL_MAX_SCHEMES)
        {
            char path[LIBVER_INTERNAL_PATH_MAX];

            if (marker_present_(dir, spec->marker, path, sizeof(path)))
            {
                fill_other_ecosystem_warning_(
                    &warns[nwarns]
                ,   spec->name
                ,   path
                ,   winner.scheme
                );
                ++nwarns;
            }
        }
    }

    if (!have_winner)
    {
        return LIBVER_RC_NO_MATCH;
    }

    return commit_hit_(result, &winner, warns, nwarns);
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
    free(result->warnings);

    clear_result_(result);
}


/* ///////////////////////////// end of file //////////////////////////// */
