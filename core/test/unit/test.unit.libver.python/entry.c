/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/unit/test.unit.libver.python/entry.c
 *
 * Purpose: Unit-tests for Python version-literal extraction (no fixtures).
 *
 * Created: 17th September 2026
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


/** \file core/test/unit/test.unit.libver.python/entry.c
 * Unit-tests for Python version-literal extraction
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

static void TEST_python_extract_VERSION_EQ(void);
static void TEST_python_extract_DUNDER_VERSION(void);
static void TEST_python_extract_FIRST_WINS(void);
static void TEST_python_extract_NO_MATCH(void);
static void TEST_python_extract_NOT_VERSION_INFO(void);


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char* argv[])
{
    int retCode = EXIT_SUCCESS;
    int verbosity = 2;

    XTESTS_COMMANDLINE_PARSEVERBOSITY(argc, argv, &verbosity);

    if (XTESTS_START_RUNNER("test.unit.libver.python", verbosity))
    {
        XTESTS_RUN_CASE(TEST_python_extract_VERSION_EQ);
        XTESTS_RUN_CASE(TEST_python_extract_DUNDER_VERSION);
        XTESTS_RUN_CASE(TEST_python_extract_FIRST_WINS);
        XTESTS_RUN_CASE(TEST_python_extract_NO_MATCH);
        XTESTS_RUN_CASE(TEST_python_extract_NOT_VERSION_INFO);

        XTESTS_PRINT_RESULTS();

        XTESTS_END_RUNNER_UPDATE_EXITCODE(&retCode);
    }

    return retCode;
}


/* /////////////////////////////////////////////////////////////////////////
 * test implementations
 */

static void TEST_python_extract_VERSION_EQ(void)
{
    char out[64];

    XTESTS_TEST_INTEGER_EQUAL(
        0
    ,   libver_internal_python_extract_literal(
            "    version = \"1.2.3\"\n"
        ,   out
        ,   sizeof(out)
        )
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("1.2.3", out);
}

static void TEST_python_extract_DUNDER_VERSION(void)
{
    char out[64];

    XTESTS_TEST_INTEGER_EQUAL(
        0
    ,   libver_internal_python_extract_literal(
            "__version__ = '0.1.0'\n"
        ,   out
        ,   sizeof(out)
        )
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("0.1.0", out);
}

static void TEST_python_extract_FIRST_WINS(void)
{
    char out[64];

    XTESTS_TEST_INTEGER_EQUAL(
        0
    ,   libver_internal_python_extract_literal(
            "version = \"1.0.0\"\n__version__ = \"2.0.0\"\n"
        ,   out
        ,   sizeof(out)
        )
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("1.0.0", out);
}

static void TEST_python_extract_NO_MATCH(void)
{
    char out[64];

    out[0] = 'x';
    out[1] = '\0';

    XTESTS_TEST_INTEGER_NOT_EQUAL(
        0
    ,   libver_internal_python_extract_literal(
            "# no version here\nname = \"pkg\"\n"
        ,   out
        ,   sizeof(out)
        )
    );
}

static void TEST_python_extract_NOT_VERSION_INFO(void)
{
    char out[64];

    XTESTS_TEST_INTEGER_EQUAL(
        0
    ,   libver_internal_python_extract_literal(
            "version_info = (1, 2, 3)\n__version__ = \"1.2.3\"\n"
        ,   out
        ,   sizeof(out)
        )
    );
    XTESTS_TEST_MULTIBYTE_STRING_EQUAL("1.2.3", out);
}


/* ///////////////////////////// end of file //////////////////////////// */
