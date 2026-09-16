/* /////////////////////////////////////////////////////////////////////////
 * File:    cli/common/entry.c
 *
 * Purpose: Shared CLASP entry stub for libver CLI frontends.
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


/** \file cli/common/entry.c Shared CLASP entry stub for libver CLI frontends
 *
 * Compile with -DLIBVER_TOOL_NAME=\"libver\" (or \"cargo-libver\", etc.).
 */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <libver/libver.h>

#include <sistools/common/usage.h>

#include <clasp/clasp.h>

#include <cstring/cstring.h>

#include <collect-c/circq.h>

#include <platformstl/filesystem/path_functions.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef LIBVER_TOOL_NAME
# error LIBVER_TOOL_NAME must be defined for this translation unit
#endif

#define TOOLNAME                        LIBVER_TOOL_NAME
#define PROGRAM_VER_MAJOR               LIBVER_VER_MAJOR
#define PROGRAM_VER_MINOR               LIBVER_VER_MINOR
#define PROGRAM_VER_PATCH               LIBVER_VER_PATCH
#define PROGRAM_VER_ALPHABETA           LIBVER_VER_ALPHABETA
#define SUMMARY                         "Synesis Information Systems"
#define COPYRIGHT                       "Copyright (c) 2026 Synesis Information Systems"
#define DESCRIPTION                     "Discover the definitive version of a software project"
#define USAGE                           TOOLNAME " [ ... flags/options ... ] [ <directory> ]"

#define SIS_DOTSTAR(slice)              (int)(slice).len, (slice).ptr


/* /////////////////////////////////////////////////////////////////////////
 * constants
 */

static clasp_specification_t const Specifications[] =
{
    CLASP_GAP_SECTION("standard flags:"),

    CLASP_FLAG(NULL, "--help", "displays this help and terminates"),
    CLASP_FLAG(NULL, "--version", "displays version information and terminates"),

    CLASP_SPECIFICATION_ARRAY_TERMINATOR
};


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static
int
run(
    clasp_arguments_t const*        args
,   clasp_specification_t const*    specifications
)
{
    clasp_argument_t const* firstUnusedFlagOrOption;
    int                     flags = 0;

    /* Reference dependent libraries so stubs pull them in when linked. */
    ((void)cstring_create);
    ((void)collect_c_cq_version);

    if (clasp_flagIsSpecified(args, "--help"))
    {
        stcc_show_help(
            args
        ,   specifications
        ,   stdout
        ,   TOOLNAME
        ,   SUMMARY
        ,   COPYRIGHT
        ,   DESCRIPTION
        ,   USAGE
        ,   PROGRAM_VER_MAJOR
        ,   PROGRAM_VER_MINOR
        ,   PROGRAM_VER_PATCH
        ,   PROGRAM_VER_ALPHABETA
        );

        return EXIT_SUCCESS;
    }

    if (clasp_flagIsSpecified(args, "--version"))
    {
        stcc_show_version(
            stdout
        ,   TOOLNAME
        ,   PROGRAM_VER_MAJOR
        ,   PROGRAM_VER_MINOR
        ,   PROGRAM_VER_PATCH
        ,   PROGRAM_VER_ALPHABETA
        );

        return EXIT_SUCCESS;
    }

    clasp_checkAllFlags(args, specifications, &flags);

    if (0 != clasp_reportUnusedFlagsAndOptions(args, &firstUnusedFlagOrOption, 0))
    {
        fprintf(
            stderr
        ,   "%.*s: unrecognised flag/option: '%.*s'\n"
        ,   SIS_DOTSTAR(args->programName)
        ,   SIS_DOTSTAR(firstUnusedFlagOrOption->resolvedName)
        );

        return EXIT_FAILURE;
    }

    fprintf(
        stderr
    ,   "%.*s: version discovery is not implemented yet\n"
    ,   SIS_DOTSTAR(args->programName)
    );

    return EXIT_FAILURE;
}


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char** argv)
{
    stlsoft_C_string_slice_m_t const    programName     = platformstl_C_get_executable_name_from_path(argv[0]);
    unsigned                            flags           = 0;
    clasp_specification_t const*        specifications  = Specifications;
    clasp_diagnostic_context_t const*   ctxt            = NULL;
    clasp_arguments_t const*            args            = NULL;
    int                                 r;

    r = clasp_parseArguments(
            flags
        ,   argc
        ,   argv
        ,   specifications
        ,   ctxt
        ,   &args
        );

    if (0 != r)
    {
        fprintf(
            stderr
        ,   "%.*s: failed to initialise the command-line parsing libraries: %s\n"
        ,   SIS_DOTSTAR(programName)
        ,   strerror(r)
        );

        return EXIT_FAILURE;
    }
    else
    {
        int const xc = run(args, specifications);

        clasp_releaseArguments(args);

        return xc;
    }
}


/* ///////////////////////////// end of file //////////////////////////// */
