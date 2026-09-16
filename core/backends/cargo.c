/* /////////////////////////////////////////////////////////////////////////
 * File:    cargo.c
 *
 * Purpose: Cargo.toml backend: read [package].version via tomlc17.
 *
 * Created: 16th September 2026
 * Updated: 17th September 2026
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


/** \file cargo.c Cargo.toml [package].version extraction
 */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include "libver.internal.h"

#include <tomlc17.h>

#include <assert.h>


/* /////////////////////////////////////////////////////////////////////////
 * API (internal)
 */

int
libver_backend_cargo_probe(
    char const*                 dir
,   libver_internal_hit_t*      hit
,   libver_internal_warning_t*  warnings
,   size_t                      warnings_cap
,   size_t*                     num_warnings
)
{
    char            path[LIBVER_INTERNAL_PATH_MAX];
    int             rc;
    toml_result_t   parsed;
    toml_datum_t    version;

    assert(NULL != dir);
    assert(NULL != hit);

    (void)warnings;
    (void)warnings_cap;

    if (NULL != num_warnings)
    {
        *num_warnings = 0;
    }

    rc = libver_internal_regular_file_in_dir(
            dir
        ,   "Cargo.toml"
        ,   path
        ,   sizeof(path)
        );

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    parsed = toml_parse_file_ex(path);

    if (!parsed.ok)
    {
        toml_free(parsed);

        return LIBVER_RC_PARSE;
    }

    version = toml_seek(parsed.toptab, "package.version");

    if (TOML_STRING != version.type || NULL == version.u.s)
    {
        toml_free(parsed);

        return LIBVER_RC_NO_VERSION;
    }

    rc = libver_internal_hit_set_version(
            hit
        ,   LIBVER_SCHEME_CARGO
        ,   path
        ,   version.u.s
        );

    toml_free(parsed);

    return rc;
}


/* ///////////////////////////// end of file //////////////////////////// */
