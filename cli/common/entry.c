/* /////////////////////////////////////////////////////////////////////////
 * File:    cli/common/entry.c
 *
 * Purpose: Shared CLASP entry for libver CLI frontends.
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


/** \file cli/common/entry.c Shared CLASP entry for libver CLI frontends
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

#ifdef _WIN32
# include <direct.h>
#else /* ? _WIN32 */
# include <unistd.h>
#endif /* _WIN32 */


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

#define CLI_DIR_MAX                     (4096)

/* cargo-libver may override these (Cargo.toml only; Cargo-oriented help).
 * libver defaults to every known scheme in documented precedence order.
 */
#ifndef LIBVER_CLI_SCHEMES
# define LIBVER_CLI_SCHEMES             LIBVER_SCHEMES_ALL
#endif /* !LIBVER_CLI_SCHEMES */

#ifndef LIBVER_CLI_DESCRIPTION
# define LIBVER_CLI_DESCRIPTION         DESCRIPTION
#endif /* !LIBVER_CLI_DESCRIPTION */


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
copy_slice_(
    char*           dest
,   size_t          cap
,   clasp_slice_t   slice
)
{
    if (slice.len >= cap)
    {
        return -1;
    }

    memcpy(dest, slice.ptr, slice.len);
    dest[slice.len] = '\0';

    return 0;
}

static
int
slice_eq_cstr_(
    clasp_slice_t   slice
,   const char*     s
)
{
    size_t const n = strlen(s);

    if (n != slice.len)
    {
        return 0;
    }

    return 0 == memcmp(slice.ptr, s, n);
}

static
size_t
first_value_index_(
    const clasp_arguments_t* args
)
{
    /* Ecosystem frontends named `*-libver` may receive a leading
     * `libver` token (e.g. Cargo: `cargo-libver libver [args...]`).
     */
    if (0 != args->numValues)
    {
        if (slice_eq_cstr_(args->values[0].value, "libver"))
        {
            size_t const n  =   strlen(TOOLNAME);
            size_t const sn =   sizeof("-libver") - 1;

            if (n >= sn && 0 == strcmp(&TOOLNAME[n - sn], "-libver"))
            {
                return 1;
            }
        }
    }

    return 0;
}

static
int
fill_cwd_(
    char*   buf
,   size_t  cap
)
{
#ifdef _WIN32

    if (NULL == _getcwd(buf, (int)cap))
#else /* ? _WIN32 */

    if (NULL == getcwd(buf, cap))
#endif /* _WIN32 */
    {
        return -1;
    }

    return 0;
}

static
int
resolve_dir_(
    const clasp_arguments_t*    args
,   char*                       buf
,   size_t                      cap
)
{
    size_t const    first   =   first_value_index_(args);
    size_t const    nvals   =   args->numValues - first;

    if (1 < nvals)
    {
        fprintf(
            stderr
        ,   "%.*s: unexpected argument: '%.*s'\n"
        ,   SIS_DOTSTAR(args->programName)
        ,   SIS_DOTSTAR(args->values[first + 1].value)
        );

        return -1;
    }

    if (1 == nvals)
    {
        if (0 == args->values[first].value.len)
        {
            fprintf(
                stderr
            ,   "%.*s: directory argument is empty\n"
            ,   SIS_DOTSTAR(args->programName)
            );

            return -1;
        }

        if (0 != copy_slice_(buf, cap, args->values[first].value))
        {
            fprintf(
                stderr
            ,   "%.*s: directory path is too long\n"
            ,   SIS_DOTSTAR(args->programName)
            );

            return -1;
        }

        return 0;
    }

    if (0 != fill_cwd_(buf, cap))
    {
        fprintf(
            stderr
        ,   "%.*s: failed to determine the current directory: %s\n"
        ,   SIS_DOTSTAR(args->programName)
        ,   strerror(errno)
        );

        return -1;
    }

    return 0;
}

static
const char*
rc_message_(
    int rc
)
{
    switch (rc)
    {
    case LIBVER_RC_NO_MATCH:

        if (0 == strcmp(LIBVER_CLI_SCHEMES, LIBVER_SCHEME_CARGO))
        {
            return "no Cargo.toml version";
        }

        return "no recognised project version";
    case LIBVER_RC_DIR_NOT_FOUND:

        return "directory not found";
    case LIBVER_RC_DIR_NOT_READABLE:

        return "directory not readable";
    case LIBVER_RC_PARSE:

        return "failed to parse project version marker";
    case LIBVER_RC_NO_VERSION:

        return "project marker has no usable version";
    case LIBVER_RC_NO_MEMORY:

        return "out of memory";
    case LIBVER_RC_IO:

        return "I/O failure reading project version marker";
    case LIBVER_RC_INVALID:

        return "invalid argument";
    default:

        return "version discovery failed";
    }
}

static
int
exit_from_rc_(
    int rc
)
{
    if (LIBVER_RC_SUCCESS == rc)
    {
        return EXIT_SUCCESS;
    }

    if (rc < 0)
    {
        return EXIT_FAILURE;
    }

    return rc;
}

static
void
print_hit_(
    const libver_scheme_result_t* hit
)
{
    printf("scheme:  %s\n", hit->scheme);
    printf("version: %s\n", hit->version);
    printf("source:  %s\n", hit->source);
}

static
int
run(
    clasp_arguments_t const*        args
,   clasp_specification_t const*    specifications
)
{
    clasp_argument_t const* firstUnusedFlagOrOption;
    int                     flags = 0;
    char                    dir[CLI_DIR_MAX];
    libver_result_t         result;
    int                     ir;
    int                     rc;

    /* Reference dependent libraries so they are pulled in when linked. */
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
        ,   LIBVER_CLI_DESCRIPTION
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

    if (0 != resolve_dir_(args, dir, sizeof(dir)))
    {
        return EXIT_FAILURE;
    }

    ir = libver_init(NULL);

    if (0 != ir)
    {
        fprintf(
            stderr
        ,   "%.*s: failed to initialise libver: %d\n"
        ,   SIS_DOTSTAR(args->programName)
        ,   ir
        );

        return EXIT_FAILURE;
    }

    memset(&result, 0, sizeof(result));

    rc = libver_find(dir, 0, LIBVER_CLI_SCHEMES, &result);

    if (LIBVER_RC_SUCCESS == rc)
    {
        print_hit_(&result.schemes[0]);
    }
    else
    {
        fprintf(
            stderr
        ,   "%.*s: %s: '%s'\n"
        ,   SIS_DOTSTAR(args->programName)
        ,   rc_message_(rc)
        ,   dir
        );
    }

    libver_result_free(&result);
    libver_uninit();

    return exit_from_rc_(rc);
}


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char* argv[])
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
