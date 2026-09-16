/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/unit/test.unit.libver.semver/entry.c
 *
 * Purpose: Unit-tests for SemVer parse and Synesis field mapping.
 *
 * Created: 16th September 2026
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


/** \file core/test/unit/test.unit.libver.semver/entry.c
 * Unit-tests for SemVer parse and Synesis field mapping
 */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

/* /////////////////////////////////////
 * test component header file include(s)
 */

#include "libver.internal.h"

/* /////////////////////////////////////
 * general includes
 */

/* xTests header files */
#include <xtests/terse-api.h>

/* STLSoft header files */
#include <stlsoft/stlsoft.h>

/* Standard C header files */
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * forward declarations
 */

static void TEST_parse_semver_RELEASE(void);
static void TEST_parse_semver_PRERELEASE(void);
static void TEST_parse_semver_BUILD(void);
static void TEST_parse_semver_PRERELEASE_AND_BUILD(void);
static void TEST_parse_semver_INVALID(void);
static void TEST_hit_set_version_SYNESIS_FIELDS(void);


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static void
assert_parse_(
    const char*     s
,   int         major
,   int         minor
,   int         patch
,   char const* pre
,   char const* meta
)
{
    int         maj = -1;
    int         min = -1;
    int         pat = -1;
    char const* pre_p = NULL;
    size_t      pre_n = 99;
    char const* meta_p = NULL;
    size_t      meta_n = 99;

    XTESTS_TEST_INTEGER_EQUAL(0, libver_internal_parse_semver(
            s
        ,   &maj
        ,   &min
        ,   &pat
        ,   &pre_p
        ,   &pre_n
        ,   &meta_p
        ,   &meta_n
        ));
    XTESTS_TEST_INTEGER_EQUAL(major, maj);
    XTESTS_TEST_INTEGER_EQUAL(minor, min);
    XTESTS_TEST_INTEGER_EQUAL(patch, pat);
    XTESTS_TEST_INTEGER_EQUAL((int)strlen(pre), (int)pre_n);
    XTESTS_TEST_INTEGER_EQUAL((int)strlen(meta), (int)meta_n);

    if (0 != pre_n)
    {
        XTESTS_TEST_BOOLEAN_TRUE(0 == strncmp(pre_p, pre, pre_n));
    }

    if (0 != meta_n)
    {
        XTESTS_TEST_BOOLEAN_TRUE(0 == strncmp(meta_p, meta, meta_n));
    }
}


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char* argv[])
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if (XTESTS_START_RUNNER("test.unit.libver.semver", verbosity))
    {
        XTESTS_RUN_CASE(TEST_parse_semver_RELEASE);
        XTESTS_RUN_CASE(TEST_parse_semver_PRERELEASE);
        XTESTS_RUN_CASE(TEST_parse_semver_BUILD);
        XTESTS_RUN_CASE(TEST_parse_semver_PRERELEASE_AND_BUILD);
        XTESTS_RUN_CASE(TEST_parse_semver_INVALID);
        XTESTS_RUN_CASE(TEST_hit_set_version_SYNESIS_FIELDS);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}


/* /////////////////////////////////////////////////////////////////////////
 * test implementations
 */

static void TEST_parse_semver_RELEASE(void)
{
    assert_parse_("1.2.3", 1, 2, 3, "", "");
}

static void TEST_parse_semver_PRERELEASE(void)
{
    assert_parse_("0.1.0-beta.2", 0, 1, 0, "beta.2", "");
}

static void TEST_parse_semver_BUILD(void)
{
    assert_parse_("4.0.0+gdeadbeef", 4, 0, 0, "", "gdeadbeef");
}

static void TEST_parse_semver_PRERELEASE_AND_BUILD(void)
{
    assert_parse_("1.2.3-alpha.1+9", 1, 2, 3, "alpha.1", "9");
}

static void TEST_parse_semver_INVALID(void)
{
    int         maj;
    int         min;
    int         pat;
    char const* pre;
    size_t      pre_n;
    char const* meta;
    size_t      meta_n;

    XTESTS_TEST_INTEGER_NOT_EQUAL(0, libver_internal_parse_semver(
            "1.2"
        ,   &maj, &min, &pat
        ,   &pre, &pre_n, &meta, &meta_n
        ));
    XTESTS_TEST_INTEGER_NOT_EQUAL(0, libver_internal_parse_semver(
            "1.2.3-"
        ,   &maj, &min, &pat
        ,   &pre, &pre_n, &meta, &meta_n
        ));
    XTESTS_TEST_INTEGER_NOT_EQUAL(0, libver_internal_parse_semver(
            "not-a-version"
        ,   &maj, &min, &pat
        ,   &pre, &pre_n, &meta, &meta_n
        ));
}

static void TEST_hit_set_version_SYNESIS_FIELDS(void)
{
    libver_internal_hit_t hit;

    XTESTS_TEST_INTEGER_EQUAL(
        LIBVER_RC_SUCCESS
    ,   libver_internal_hit_set_version(&hit, "zig", "x", "1.2.3")
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", hit.prerelease);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", hit.build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0xFF, hit.alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(0, hit.build);

    XTESTS_TEST_INTEGER_EQUAL(
        LIBVER_RC_SUCCESS
    ,   libver_internal_hit_set_version(&hit, "cargo", "x", "0.1.0-beta.2")
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("beta.2", hit.prerelease);
    XTESTS_TEST_INTEGER_EQUAL(0x82, hit.alphabeta);

    XTESTS_TEST_INTEGER_EQUAL(
        LIBVER_RC_SUCCESS
    ,   libver_internal_hit_set_version(&hit, "zig", "x", "1.2.3-alpha.1+9")
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("alpha.1", hit.prerelease);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("9", hit.build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0x41, hit.alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(9, hit.build);

    XTESTS_TEST_INTEGER_EQUAL(
        LIBVER_RC_SUCCESS
    ,   libver_internal_hit_set_version(&hit, "zig", "x", "4.0.0+gdeadbeef")
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("gdeadbeef", hit.build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0xFF, hit.alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(0, hit.build);

    XTESTS_TEST_INTEGER_EQUAL(
        LIBVER_RC_SUCCESS
    ,   libver_internal_hit_set_version(&hit, "zig", "x", "1.0.0-dev")
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("dev", hit.prerelease);
    XTESTS_TEST_INTEGER_EQUAL(0, hit.alphabeta);
}


/* ///////////////////////////// end of file //////////////////////////// */

