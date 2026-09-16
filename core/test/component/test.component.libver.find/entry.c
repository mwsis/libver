/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/component/test.component.libver.find/entry.c
 *
 * Purpose: Component tests for libver_find() against fixture trees.
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


/** \file core/test/component/test.component.libver.find/entry.c
 * Component tests for libver_find() against fixture trees
 */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

/* /////////////////////////////////////
 * test component header file include(s)
 */

#include <libver/libver.h>

/* /////////////////////////////////////
 * general includes
 */

/* xTests header files */
#include <xtests/terse-api.h>

/* STLSoft header files */
#include <stlsoft/stlsoft.h>

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
# include <sys/stat.h>
# include <unistd.h>
#endif /* !_WIN32 */


/* /////////////////////////////////////////////////////////////////////////
 * constants
 */

#ifndef LIBVER_FIXTURES_DIR
# error LIBVER_FIXTURES_DIR must be defined
#endif /* !LIBVER_FIXTURES_DIR */


/* /////////////////////////////////////////////////////////////////////////
 * forward declarations
 */

static void TEST_libver_find_CARGO_ONLY(void);
static void TEST_libver_find_ZIG_ONLY(void);
static void TEST_libver_find_CARGO_AND_ZIG_CARGO_WINS(void);
static void TEST_libver_find_CARGO_AND_ZIG_FILTER_ZIG(void);
static void TEST_libver_find_ZIG_LEXICAL(void);
static void TEST_libver_find_CARGO_PRERELEASE(void);
static void TEST_libver_find_ZIG_PRERELEASE_AND_BUILD(void);
static void TEST_libver_find_ZIG_BUILD_METADATA(void);
static void TEST_libver_find_PYTHON_PYPROJECT(void);
static void TEST_libver_find_PYTHON_PRERELEASE(void);
static void TEST_libver_find_PYTHON_LEGACY_AGREE(void);
static void TEST_libver_find_PYTHON_LEGACY_DRIFT(void);
static void TEST_libver_find_PYTHON_PYPROJECT_AND_LEGACY(void);
static void TEST_libver_find_PYTHON_PKG_INIT(void);
static void TEST_libver_find_PYTHON_NESTED_IGNORED(void);
static void TEST_libver_find_CARGO_AND_PYTHON_CARGO_WINS(void);
static void TEST_libver_find_CARGO_AND_PYTHON_FILTER_PYTHON(void);
static void TEST_libver_find_NO_MATCH(void);
static void TEST_libver_find_DIR_NOT_FOUND(void);
static void TEST_libver_find_CARGO_FILTER_ON_ZIG_ONLY(void);
static void TEST_libver_find_PYTHON_FILTER_ON_CARGO_ONLY(void);
#ifndef _WIN32
static void TEST_libver_find_UNREADABLE_DIR(void);
#endif /* !_WIN32 */


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static void
fixture_dir_(
    char*       buf
,   size_t      cap
,   const char* leaf
)
{
    int const n = snprintf(buf, cap, "%s/%s", LIBVER_FIXTURES_DIR, leaf);

    XTESTS_TEST_BOOLEAN_TRUE(n > 0 && (size_t)n < cap);
}

static void
expect_source_(
    const char* dir
,   const char* file
,   const char* actual
)
{
    char expect[4096];
    int const n = snprintf(expect, sizeof(expect), "%s/%s", dir, file);

    XTESTS_TEST_BOOLEAN_TRUE(n > 0 && (size_t)n < sizeof(expect));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(expect, actual);
}

static void
assert_scheme_(
    libver_result_t const*  result
,   const char*             scheme
,   const char*             version
,   int                     major
,   int                     minor
,   int                     patch
)
{
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result->num_schemes));
    XTESTS_REQUIRE(XTESTS_TEST_POINTER_NOT_EQUAL(NULL, result->schemes));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(scheme, result->schemes[0].scheme);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(version, result->schemes[0].version);
    XTESTS_TEST_INTEGER_EQUAL(major, result->schemes[0].major);
    XTESTS_TEST_INTEGER_EQUAL(minor, result->schemes[0].minor);
    XTESTS_TEST_INTEGER_EQUAL(patch, result->schemes[0].patch);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", result->schemes[0].prerelease);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", result->schemes[0].build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0xFF, result->schemes[0].alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(0, result->schemes[0].build);
}

static void
expect_no_warnings_(
    libver_result_t const* result
)
{
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result->num_warnings);
    XTESTS_TEST_POINTER_EQUAL(NULL, result->warnings);
}


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char* argv[])
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if (XTESTS_START_RUNNER("test.component.libver.find", verbosity))
    {
        XTESTS_RUN_CASE(TEST_libver_find_CARGO_ONLY);
        XTESTS_RUN_CASE(TEST_libver_find_ZIG_ONLY);
        XTESTS_RUN_CASE(TEST_libver_find_CARGO_AND_ZIG_CARGO_WINS);
        XTESTS_RUN_CASE(TEST_libver_find_CARGO_AND_ZIG_FILTER_ZIG);
        XTESTS_RUN_CASE(TEST_libver_find_ZIG_LEXICAL);
        XTESTS_RUN_CASE(TEST_libver_find_CARGO_PRERELEASE);
        XTESTS_RUN_CASE(TEST_libver_find_ZIG_PRERELEASE_AND_BUILD);
        XTESTS_RUN_CASE(TEST_libver_find_ZIG_BUILD_METADATA);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_PYPROJECT);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_PRERELEASE);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_LEGACY_AGREE);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_LEGACY_DRIFT);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_PYPROJECT_AND_LEGACY);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_PKG_INIT);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_NESTED_IGNORED);
        XTESTS_RUN_CASE(TEST_libver_find_CARGO_AND_PYTHON_CARGO_WINS);
        XTESTS_RUN_CASE(TEST_libver_find_CARGO_AND_PYTHON_FILTER_PYTHON);
        XTESTS_RUN_CASE(TEST_libver_find_NO_MATCH);
        XTESTS_RUN_CASE(TEST_libver_find_DIR_NOT_FOUND);
        XTESTS_RUN_CASE(TEST_libver_find_CARGO_FILTER_ON_ZIG_ONLY);
        XTESTS_RUN_CASE(TEST_libver_find_PYTHON_FILTER_ON_CARGO_ONLY);
#ifndef _WIN32
        XTESTS_RUN_CASE(TEST_libver_find_UNREADABLE_DIR);
#endif /* !_WIN32 */

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}


/* /////////////////////////////////////////////////////////////////////////
 * test implementations
 */

static void TEST_libver_find_CARGO_ONLY(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "cargo-only");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_CARGO, "1.2.3", 1, 2, 3);
    expect_source_(dir, "Cargo.toml", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);

    libver_uninit();
}

static void TEST_libver_find_ZIG_ONLY(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "zig-only");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_ZIG, "0.4.5", 0, 4, 5);
    expect_source_(dir, "build.zig.zon", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_CARGO_AND_ZIG_CARGO_WINS(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "cargo-and-zig");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_CARGO, "3.1.4", 3, 1, 4);
    expect_source_(dir, "Cargo.toml", result.schemes[0].source);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_warnings));
    XTESTS_REQUIRE(XTESTS_TEST_POINTER_NOT_EQUAL(NULL, result.warnings));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(
        LIBVER_WARNING_OTHER_ECOSYSTEM
    ,   result.warnings[0].kind
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_ZIG, result.warnings[0].scheme);
    expect_source_(dir, "build.zig.zon", result.warnings[0].source);
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "zig"));
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "cargo"));

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_CARGO_AND_ZIG_FILTER_ZIG(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "cargo-and-zig");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEME_ZIG, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_ZIG, "9.8.7", 9, 8, 7);
    expect_source_(dir, "build.zig.zon", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_ZIG_LEXICAL(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "zig-lexical");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_ZIG, "2.0.1", 2, 0, 1);
    expect_source_(dir, "build.zig.zon", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_CARGO_PRERELEASE(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "cargo-prerelease");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_schemes));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_CARGO, result.schemes[0].scheme);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("0.1.0-beta.2", result.schemes[0].version);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].major);
    XTESTS_TEST_INTEGER_EQUAL(1, result.schemes[0].minor);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].patch);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("beta.2", result.schemes[0].prerelease);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", result.schemes[0].build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0x82, result.schemes[0].alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].build);
    expect_source_(dir, "Cargo.toml", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_ZIG_PRERELEASE_AND_BUILD(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "zig-prerelease");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_schemes));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_ZIG, result.schemes[0].scheme);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("1.2.3-alpha.1+9", result.schemes[0].version);
    XTESTS_TEST_INTEGER_EQUAL(1, result.schemes[0].major);
    XTESTS_TEST_INTEGER_EQUAL(2, result.schemes[0].minor);
    XTESTS_TEST_INTEGER_EQUAL(3, result.schemes[0].patch);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("alpha.1", result.schemes[0].prerelease);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("9", result.schemes[0].build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0x41, result.schemes[0].alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(9, result.schemes[0].build);
    expect_source_(dir, "build.zig.zon", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_ZIG_BUILD_METADATA(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "zig-build");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_schemes));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_ZIG, result.schemes[0].scheme);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("4.0.0+gdeadbeef", result.schemes[0].version);
    XTESTS_TEST_INTEGER_EQUAL(4, result.schemes[0].major);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].minor);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].patch);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", result.schemes[0].prerelease);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("gdeadbeef", result.schemes[0].build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0xFF, result.schemes[0].alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].build);
    expect_source_(dir, "build.zig.zon", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_PYPROJECT(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "python-pyproject");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_PYTHON, "1.4.1", 1, 4, 1);
    expect_source_(dir, "pyproject.toml", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_PRERELEASE(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "python-prerelease");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_schemes));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_PYTHON, result.schemes[0].scheme);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("0.2.0-rc.1", result.schemes[0].version);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].major);
    XTESTS_TEST_INTEGER_EQUAL(2, result.schemes[0].minor);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].patch);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("rc.1", result.schemes[0].prerelease);
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("", result.schemes[0].build_metadata);
    XTESTS_TEST_INTEGER_EQUAL(0xC1, result.schemes[0].alphabeta);
    XTESTS_TEST_INTEGER_EQUAL(0, result.schemes[0].build);
    expect_source_(dir, "pyproject.toml", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_LEGACY_AGREE(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "python-legacy-agree");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_PYTHON, "2.0.0", 2, 0, 0);
    expect_source_(dir, "setup.py", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_LEGACY_DRIFT(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "python-legacy-drift");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_PYTHON, "2.0.0", 2, 0, 0);
    expect_source_(dir, "setup.py", result.schemes[0].source);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_warnings));
    XTESTS_REQUIRE(XTESTS_TEST_POINTER_NOT_EQUAL(NULL, result.warnings));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(
        LIBVER_WARNING_INCONSISTENT_SOURCES
    ,   result.warnings[0].kind
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_PYTHON, result.warnings[0].scheme);
    expect_source_(dir, "pkg/__init__.py", result.warnings[0].source);
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "2.1.0"));
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "2.0.0"));

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_PYPROJECT_AND_LEGACY(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "python-pyproject-and-legacy");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_PYTHON, "3.0.0", 3, 0, 0);
    expect_source_(dir, "pyproject.toml", result.schemes[0].source);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_warnings));
    XTESTS_REQUIRE(XTESTS_TEST_POINTER_NOT_EQUAL(NULL, result.warnings));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(
        LIBVER_WARNING_INCONSISTENT_SOURCES
    ,   result.warnings[0].kind
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_PYTHON, result.warnings[0].scheme);
    expect_source_(dir, "setup.py", result.warnings[0].source);
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "1.0.0"));
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "3.0.0"));

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_PKG_INIT(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "python-pkg-init");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_PYTHON, "4.5.6", 4, 5, 6);
    expect_source_(dir, "pkg/__init__.py", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_NESTED_IGNORED(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "python-nested-ignored");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_NO_MATCH, r);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_CARGO_AND_PYTHON_CARGO_WINS(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "cargo-and-python");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_CARGO, "3.1.4", 3, 1, 4);
    expect_source_(dir, "Cargo.toml", result.schemes[0].source);
    XTESTS_REQUIRE(XTESTS_TEST_INTEGER_EQUAL(1, (int)result.num_warnings));
    XTESTS_REQUIRE(XTESTS_TEST_POINTER_NOT_EQUAL(NULL, result.warnings));
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(
        LIBVER_WARNING_OTHER_ECOSYSTEM
    ,   result.warnings[0].kind
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL(LIBVER_SCHEME_PYTHON, result.warnings[0].scheme);
    expect_source_(dir, "pyproject.toml", result.warnings[0].source);
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "python"));
    XTESTS_TEST_BOOLEAN_TRUE(NULL != strstr(result.warnings[0].message, "cargo"));

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_CARGO_AND_PYTHON_FILTER_PYTHON(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "cargo-and-python");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEME_PYTHON, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_SUCCESS, r);
    assert_scheme_(&result, LIBVER_SCHEME_PYTHON, "8.8.8", 8, 8, 8);
    expect_source_(dir, "pyproject.toml", result.schemes[0].source);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_NO_MATCH(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "empty");
    memset(&result, 0x5a, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_NO_MATCH, r);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_DIR_NOT_FOUND(void)
{
    libver_result_t result;
    int             r;

    memset(&result, 0x5a, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(
            "/no/such/libver/fixture/dir"
        ,   0
        ,   LIBVER_SCHEMES_ALL
        ,   &result
        );

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_DIR_NOT_FOUND, r);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_CARGO_FILTER_ON_ZIG_ONLY(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "zig-only");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEME_CARGO, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_NO_MATCH, r);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

static void TEST_libver_find_PYTHON_FILTER_ON_CARGO_ONLY(void)
{
    char            dir[4096];
    libver_result_t result;
    int             r;

    fixture_dir_(dir, sizeof(dir), "cargo-only");
    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEME_PYTHON, &result);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_NO_MATCH, r);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}

#ifndef _WIN32
static void TEST_libver_find_UNREADABLE_DIR(void)
{
    char            tmpl[] = "/tmp/libver-unreadable-XXXXXX";
    char*           dir;
    libver_result_t result;
    int             r;

    dir = mkdtemp(tmpl);

    if (NULL == dir)
    {
        XTESTS_TEST_FAIL("mkdtemp failed");

        return;
    }

    if (0 != chmod(dir, 0))
    {
        rmdir(dir);
        XTESTS_TEST_FAIL("chmod failed");

        return;
    }

    if (0 == access(dir, R_OK | X_OK))
    {
        /* Still reachable (e.g. root): do not fail the suite. */
        chmod(dir, 0700);
        rmdir(dir);

        return;
    }

    memset(&result, 0, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init(NULL));

    r = libver_find(dir, 0, LIBVER_SCHEMES_ALL, &result);

    chmod(dir, 0700);
    rmdir(dir);

    XTESTS_TEST_INTEGER_EQUAL(LIBVER_RC_DIR_NOT_READABLE, r);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);
    expect_no_warnings_(&result);

    libver_result_free(&result);
    libver_uninit();
}
#endif /* !_WIN32 */


/* ///////////////////////////// end of file //////////////////////////// */
