/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/scratch/zig/entry.c
 *
 * Purpose: Scratch program: read .version from a build.zig.zon (ZON, not
 *          TOML).
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


/** \file core/test/scratch/zig/entry.c Scratch: build.zig.zon .version (ZON)
 *
 * Usage: test.scratch.zig <path-to-build.zig.zon>
 *
 * Minimal line-oriented extract of `.version = "…"` — not a full ZON parser.
 */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static char const*
skip_ws_(
    char const* s
)
{
    while ('\0' != *s && isspace((unsigned char)*s))
    {
        ++s;
    }

    return s;
}

/* Returns 0 on success and writes a NUL-terminated copy into `out` (capacity
 * `out_cap`); non-zero if the line is not a `.version = "…"` assignment or
 * the value does not fit.
 */
static int
try_extract_version_(
    char const* line
,   char*       out
,   size_t      out_cap
)
{
    char const* p = skip_ws_(line);
    char const* open;
    char const* close;
    size_t      n;

    if (0 != strncmp(p, ".version", 8))
    {
        return 1;
    }

    p = skip_ws_(p + 8);

    if ('=' != *p)
    {
        return 1;
    }

    p = skip_ws_(p + 1);

    if ('"' != *p)
    {
        return 1;
    }

    open = p + 1;
    close = strchr(open, '"');

    if (NULL == close)
    {
        return 1;
    }

    n = (size_t)(close - open);

    if (0 == n || n + 1 > out_cap)
    {
        return 1;
    }

    memcpy(out, open, n);
    out[n] = '\0';

    return 0;
}


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char** argv)
{
    const char* path;
    FILE*       fp;
    char        line[1024];
    char        version[128];
    int         found = 0;

    if (2 != argc)
    {
        fprintf(stderr, "usage: %s <path-to-build.zig.zon>\n", argv[0]);

        return EXIT_FAILURE;
    }

    path = argv[1];
    fp = fopen(path, "rb");

    if (NULL == fp)
    {
        fprintf(stderr, "%s: cannot open\n", path);

        return EXIT_FAILURE;
    }

    while (NULL != fgets(line, (int)sizeof(line), fp))
    {
        if (0 == try_extract_version_(line, version, sizeof(version)))
        {
            found = 1;

            break;
        }
    }

    fclose(fp);

    if (!found)
    {
        fprintf(stderr, "%s: missing or invalid .version\n", path);

        return EXIT_FAILURE;
    }

    fprintf(stdout, "%s\n", version);

    return EXIT_SUCCESS;
}


/* ///////////////////////////// end of file //////////////////////////// */
