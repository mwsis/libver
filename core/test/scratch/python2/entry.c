/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/scratch/python2/entry.c
 *
 * Purpose: Scratch program: recursively find setup.py / __init__.py under a
 *          directory (via recls) and extract version= / __version__ literals.
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


/** \file core/test/scratch/python2/entry.c Scratch: Py2 setup.py / __init__.py versions
 *
 * Usage: test.scratch.python2 <directory>
 *
 * Recursively finds files named setup.py or __init__.py (recls), then looks
 * for leading `version=…` and `__version__=…` string literals. Prints one
 * version when all agree; otherwise lists every hit and exits non-zero.
 */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <recls/recls.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * constants and definitions
 */

#define HIT_CAP_        64
#define VER_CAP_        128
#define PATH_CAP_       1024


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

typedef struct hit_t_
{
    char    path[PATH_CAP_];
    char    kind[16];
    char    value[VER_CAP_];
} hit_t_;


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

static int
parse_quoted_(
    char const* p
,   char*       out
,   size_t      out_cap
)
{
    char        q;
    char const* open;
    char const* close;
    size_t      n;

    p = skip_ws_(p);

    if ('\'' != *p && '"' != *p)
    {
        return 1;
    }

    q = *p;
    open = p + 1;
    close = strchr(open, q);

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

/* On success, writes kind ("version" or "__version__") and value; returns 0. */
static int
try_extract_specifier_(
    char const* line
,   char*       kind
,   size_t      kind_cap
,   char*       value
,   size_t      value_cap
)
{
    char const* p = skip_ws_(line);
    char const* after_name;
    char const* name;
    size_t      name_len;

    if (0 == strncmp(p, "__version__", 11))
    {
        name = "__version__";
        name_len = 11;
        after_name = p + 11;
    }
    else if (0 == strncmp(p, "version", 7) &&
             !isalnum((unsigned char)p[7]) &&
             '_' != p[7])
    {
        name = "version";
        name_len = 7;
        after_name = p + 7;
    }
    else
    {
        return 1;
    }

    after_name = skip_ws_(after_name);

    if ('=' != *after_name)
    {
        return 1;
    }

    if (name_len + 1 > kind_cap)
    {
        return 1;
    }

    memcpy(kind, name, name_len);
    kind[name_len] = '\0';

    return parse_quoted_(after_name + 1, value, value_cap);
}

static int
scan_file_(
    char const* path
,   hit_t_*     hits
,   size_t      hit_cap
,   size_t*     hit_count
)
{
    FILE*   fp;
    char    line[1024];
    char    kind[16];
    char    value[VER_CAP_];

    fp = fopen(path, "rb");

    if (NULL == fp)
    {
        fprintf(stderr, "%s: cannot open\n", path);

        return 1;
    }

    while (NULL != fgets(line, (int)sizeof(line), fp))
    {
        if (0 != try_extract_specifier_(line, kind, sizeof(kind), value, sizeof(value)))
        {
            continue;
        }

        if (*hit_count >= hit_cap)
        {
            fprintf(stderr, "%s: too many version hits (cap %lu)\n", path, (unsigned long)hit_cap);
            fclose(fp);

            return 1;
        }

        {
            hit_t_* h = &hits[*hit_count];
            size_t  path_len = strlen(path);

            if (path_len + 1 > sizeof(h->path))
            {
                fprintf(stderr, "%s: path too long\n", path);
                fclose(fp);

                return 1;
            }

            memcpy(h->path, path, path_len + 1);
            memcpy(h->kind, kind, strlen(kind) + 1);
            memcpy(h->value, value, strlen(value) + 1);
            ++(*hit_count);
        }
    }

    fclose(fp);

    return 0;
}


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char** argv)
{
    char const*     search_dir;
    char const*     patterns = "__init__.py|setup.py";
    recls_uint32_t  flags = RECLS_F_RECURSIVE | RECLS_F_FILES;
    hrecls_t        hSrch;
    recls_rc_t      rc;
    hit_t_          hits[HIT_CAP_];
    size_t          hit_count = 0;
    size_t          i;
    int             drifted = 0;

    if (2 != argc)
    {
        fprintf(stderr, "usage: %s <directory>\n", argv[0]);

        return EXIT_FAILURE;
    }

    search_dir = argv[1];
    rc = Recls_Search(search_dir, patterns, flags, &hSrch);

    if (RECLS_RC_NO_MORE_DATA == rc)
    {
        fprintf(stderr, "%s: no setup.py / __init__.py found\n", search_dir);

        return EXIT_FAILURE;
    }
    else if (RECLS_FAILED(rc))
    {
        fprintf(
            stderr
        ,   "%s: recls search failed: %.*s\n"
        ,   search_dir
        ,   (int)Recls_GetSearchCodeStringLength(rc)
        ,   Recls_GetSearchCodeString(rc)
        );

        return EXIT_FAILURE;
    }

    {
        recls_info_t entry;

        Recls_GetDetails(hSrch, &entry);

        do
        {
            char path_buf[PATH_CAP_];
            int  path_len = (int)(entry->path.end - entry->path.begin);

            if (path_len <= 0 || (size_t)path_len + 1 > sizeof(path_buf))
            {
                fprintf(stderr, "path too long or empty\n");
                Recls_CloseDetails(entry);
                Recls_SearchClose(hSrch);

                return EXIT_FAILURE;
            }

            memcpy(path_buf, entry->path.begin, (size_t)path_len);
            path_buf[path_len] = '\0';

            if (0 != scan_file_(path_buf, hits, HIT_CAP_, &hit_count))
            {
                Recls_CloseDetails(entry);
                Recls_SearchClose(hSrch);

                return EXIT_FAILURE;
            }

            Recls_CloseDetails(entry);
        } while (RECLS_RC_OK == (rc = Recls_GetNextDetails(hSrch, &entry)));

        Recls_SearchClose(hSrch);

        if (RECLS_RC_NO_MORE_DATA != rc && RECLS_FAILED(rc))
        {
            fprintf(
                stderr
            ,   "%s: recls iteration failed: %.*s\n"
            ,   search_dir
            ,   (int)Recls_GetSearchCodeStringLength(rc)
            ,   Recls_GetSearchCodeString(rc)
            );

            return EXIT_FAILURE;
        }
    }

    if (0 == hit_count)
    {
        fprintf(stderr, "%s: no version= / __version__ literals found\n", search_dir);

        return EXIT_FAILURE;
    }

    for (i = 1; i != hit_count; ++i)
    {
        if (0 != strcmp(hits[0].value, hits[i].value))
        {
            drifted = 1;

            break;
        }
    }

    if (!drifted)
    {
        fprintf(stdout, "%s\n", hits[0].value);

        return EXIT_SUCCESS;
    }

    fprintf(stderr, "version drift:\n");

    for (i = 0; i != hit_count; ++i)
    {
        fprintf(
            stderr
        ,   "  %s: %s='%s'\n"
        ,   hits[i].path
        ,   hits[i].kind
        ,   hits[i].value
        );
    }

    return EXIT_FAILURE;
}


/* ///////////////////////////// end of file //////////////////////////// */
