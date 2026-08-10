/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/unit/test.unit.libver.find/entry.c
 *
 * Purpose: Smoke unit-tests for the stub libver_find() API.
 *
 * Created: 10th August 2026
 * Updated: 10th August 2026
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


/** \file core/test/unit/test.unit.libver.find/entry.c Smoke unit-tests for libver_find()
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
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * forward declarations
 */

static void test_init_uninit(void);
static void test_find_stub_returns_no_schemes(void);
static void test_find_null_result(void);


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char** argv)
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if (XTESTS_START_RUNNER("test.unit.libver.find", verbosity))
    {
        XTESTS_RUN_CASE(test_init_uninit);
        XTESTS_RUN_CASE(test_find_stub_returns_no_schemes);
        XTESTS_RUN_CASE(test_find_null_result);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}


/* /////////////////////////////////////////////////////////////////////////
 * test implementations
 */

static void test_init_uninit(void)
{
    XTESTS_TEST_INTEGER_EQUAL(0, libver_init());

    libver_uninit();
}

static void test_find_stub_returns_no_schemes(void)
{
    libver_result_t result;
    int             r;

    memset(&result, 0x5a, sizeof(result));

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init());

    r = libver_find(".", 0, "*", &result);

    /* Stub contract: non-zero status, empty scheme list. */
    XTESTS_TEST_INTEGER_NOT_EQUAL(0, r);
    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);

    libver_result_free(&result);

    XTESTS_TEST_INTEGER_EQUAL(0, (int)result.num_schemes);
    XTESTS_TEST_POINTER_EQUAL(NULL, result.schemes);

    libver_uninit();
}

static void test_find_null_result(void)
{
    int r;

    XTESTS_TEST_INTEGER_EQUAL(0, libver_init());

    /* Current implementation guards a NULL result; documented as a
     * precondition violation — exercise the defensive return.
     */
    r = libver_find(".", 0, "*", NULL);

    XTESTS_TEST_INTEGER_EQUAL(-1, r);

    libver_uninit();
}


/* ///////////////////////////// end of file //////////////////////////// */
