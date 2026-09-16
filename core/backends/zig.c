/* /////////////////////////////////////////////////////////////////////////
 * File:    zig.c
 *
 * Purpose: Zig backend: top-level .version from build.zig.zon (ZON, not
 *          TOML) via a small lexical scan.
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


/** \file zig.c build.zig.zon top-level .version lexical extraction
 */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include "libver.internal.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

struct zon_scan_t
{
    const char* p;
    const char* end;
};
typedef struct zon_scan_t                                   zon_scan_t;


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static int
zon_at_end_(
    zon_scan_t const* sc
)
{
    return sc->p >= sc->end;
}

static char
zon_peek_(
    zon_scan_t const* sc
)
{
    return zon_at_end_(sc) ? '\0' : *sc->p;
}

static int
zon_starts_comment_(
    zon_scan_t const* sc
)
{
    return sc->p + 1 < sc->end && '/' == sc->p[0] && '/' == sc->p[1];
}

static void
zon_skip_line_(
    zon_scan_t* sc
)
{
    while (!zon_at_end_(sc) && '\n' != *sc->p)
    {
        ++sc->p;
    }

    if (!zon_at_end_(sc))
    {
        ++sc->p;
    }
}

static void
zon_skip_ws_comments_(
    zon_scan_t* sc
)
{
    for (;;)
    {
        if (zon_at_end_(sc))
        {
            return;
        }

        if (isspace((unsigned char)*sc->p))
        {
            ++sc->p;

            continue;
        }

        if (zon_starts_comment_(sc))
        {
            zon_skip_line_(sc);

            continue;
        }

        return;
    }
}

static int
zon_hex_val_(
    char c
)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }

    if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }

    if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }

    return -1;
}

static int
zon_emit_(
    char*   out
,   size_t  out_cap
,   size_t* n
,   char    ch
)
{
    if (NULL == out)
    {
        ++*n;

        return 0;
    }

    if (*n + 1 >= out_cap)
    {
        return 1;
    }

    out[*n] = ch;
    ++*n;

    return 0;
}

/* Consume a Zig `"..."` string. When @a out is NULL the value is skipped.
 */
static int
zon_take_string_(
    zon_scan_t* sc
,   char*       out
,   size_t      out_cap
)
{
    size_t n = 0;

    if ('"' != zon_peek_(sc))
    {
        return 1;
    }

    ++sc->p;

    while (!zon_at_end_(sc))
    {
        char c = *sc->p++;

        if ('"' == c)
        {
            if (NULL != out)
            {
                out[n] = '\0';
            }

            return 0;
        }

        if ('\n' == c || '\r' == c)
        {
            return 1;
        }

        if ('\\' != c)
        {
            if (0 != zon_emit_(out, out_cap, &n, c))
            {
                return 1;
            }

            continue;
        }

        if (zon_at_end_(sc))
        {
            return 1;
        }

        c = *sc->p++;

        switch (c)
        {
            case '"':
            case '\\':
            case '\'':

                if (0 != zon_emit_(out, out_cap, &n, c))
                {
                    return 1;
                }
                break;
            case 'n':

                if (0 != zon_emit_(out, out_cap, &n, '\n'))
                {
                    return 1;
                }
                break;
            case 'r':

                if (0 != zon_emit_(out, out_cap, &n, '\r'))
                {
                    return 1;
                }
                break;
            case 't':

                if (0 != zon_emit_(out, out_cap, &n, '\t'))
                {
                    return 1;
                }
                break;
            case '0':

                if (0 != zon_emit_(out, out_cap, &n, '\0'))
                {
                    return 1;
                }
                break;
            case 'x':
            {
                int hi;
                int lo;

                if (sc->p + 1 >= sc->end)
                {
                    return 1;
                }

                hi = zon_hex_val_(sc->p[0]);
                lo = zon_hex_val_(sc->p[1]);
                sc->p += 2;

                if (hi < 0 || lo < 0)
                {
                    return 1;
                }

                if (0 != zon_emit_(out, out_cap, &n, (char)((hi << 4) | lo)))
                {
                    return 1;
                }
                break;
            }
            case 'u':
            {
                unsigned    cp = 0;
                int         digits = 0;

                if ('{' != zon_peek_(sc))
                {
                    return 1;
                }

                ++sc->p;

                while (!zon_at_end_(sc) && '}' != zon_peek_(sc))
                {
                    int const hv = zon_hex_val_(zon_peek_(sc));

                    if (hv < 0 || digits >= 6)
                    {
                        return 1;
                    }

                    cp = (cp << 4) + (unsigned)hv;
                    ++sc->p;
                    ++digits;
                }

                if (0 == digits || '}' != zon_peek_(sc))
                {
                    return 1;
                }

                ++sc->p;

                if (cp > 0x7Fu)
                {
                    /* Version strings are ASCII SemVer; reject non-ASCII. */
                    return 1;
                }

                if (0 != zon_emit_(out, out_cap, &n, (char)cp))
                {
                    return 1;
                }
                break;
            }
            default:

                return 1;
        }
    }

    return 1;
}

static int
zon_skip_string_(
    zon_scan_t* sc
)
{
    return zon_take_string_(sc, NULL, 0);
}

static int
zon_skip_char_lit_(
    zon_scan_t* sc
)
{
    if ('\'' != zon_peek_(sc))
    {
        return 1;
    }

    ++sc->p;

    if (zon_at_end_(sc))
    {
        return 1;
    }

    if ('\\' == *sc->p)
    {
        ++sc->p;

        if (zon_at_end_(sc))
        {
            return 1;
        }
    }

    ++sc->p;

    if ('\'' != zon_peek_(sc))
    {
        return 1;
    }

    ++sc->p;

    return 0;
}

static int zon_skip_value_(zon_scan_t* sc);

static int
zon_skip_aggregate_(
    zon_scan_t* sc
)
{
    int depth = 1;

    if ('{' != zon_peek_(sc))
    {
        return 1;
    }

    ++sc->p;

    while (!zon_at_end_(sc) && depth > 0)
    {
        zon_skip_ws_comments_(sc);

        if (zon_at_end_(sc))
        {
            return 1;
        }

        switch (zon_peek_(sc))
        {
            case '"':

                if (0 != zon_skip_string_(sc))
                {
                    return 1;
                }
                break;
            case '\'':

                if (0 != zon_skip_char_lit_(sc))
                {
                    return 1;
                }
                break;
            case '{':

                ++sc->p;
                ++depth;
                break;
            case '}':

                ++sc->p;
                --depth;
                break;
            default:

                ++sc->p;
                break;
        }
    }

    return 0 == depth ? 0 : 1;
}

static int
zon_skip_atom_(
    zon_scan_t* sc
)
{
    if (zon_at_end_(sc))
    {
        return 1;
    }

    while (!zon_at_end_(sc))
    {
        char const c = *sc->p;

        if (isspace((unsigned char)c) || ',' == c || '}' == c)
        {
            break;
        }

        if ('/' == c && sc->p + 1 < sc->end && '/' == sc->p[1])
        {
            break;
        }

        ++sc->p;
    }

    return 0;
}

static int
zon_skip_value_(
    zon_scan_t* sc
)
{
    zon_skip_ws_comments_(sc);

    if (zon_at_end_(sc))
    {
        return 1;
    }

    if ('"' == zon_peek_(sc))
    {
        return zon_skip_string_(sc);
    }

    if ('\'' == zon_peek_(sc))
    {
        return zon_skip_char_lit_(sc);
    }

    if ('.' == zon_peek_(sc))
    {
        ++sc->p;
        zon_skip_ws_comments_(sc);

        if ('{' == zon_peek_(sc))
        {
            return zon_skip_aggregate_(sc);
        }

        return zon_skip_atom_(sc);
    }

    if ('{' == zon_peek_(sc))
    {
        return zon_skip_aggregate_(sc);
    }

    return zon_skip_atom_(sc);
}

static int
zon_take_ident_(
    zon_scan_t* sc
,   char*       out
,   size_t      out_cap
)
{
    size_t n = 0;

    if (zon_at_end_(sc))
    {
        return 1;
    }

    if (!(isalpha((unsigned char)*sc->p) || '_' == *sc->p))
    {
        return 1;
    }

    while (!zon_at_end_(sc) &&
           (isalnum((unsigned char)*sc->p) || '_' == *sc->p))
    {
        if (n + 1 >= out_cap)
        {
            return 1;
        }

        out[n++] = *sc->p++;
    }

    out[n] = '\0';

    return 0 == n ? 1 : 0;
}

static int
zon_extract_top_level_version_(
    const char* text
,   size_t      len
,   char*       out
,   size_t      out_cap
)
{
    zon_scan_t  sc;
    char        ident[64];
    int         saw_version = 0;

    sc.p = text;
    sc.end = text + len;

    zon_skip_ws_comments_(&sc);

    if ('.' != zon_peek_(&sc))
    {
        return LIBVER_RC_PARSE;
    }

    ++sc.p;
    zon_skip_ws_comments_(&sc);

    if ('{' != zon_peek_(&sc))
    {
        return LIBVER_RC_PARSE;
    }

    ++sc.p;

    for (;;)
    {
        zon_skip_ws_comments_(&sc);

        if ('}' == zon_peek_(&sc))
        {
            ++sc.p;

            break;
        }

        if ('.' != zon_peek_(&sc))
        {
            return LIBVER_RC_PARSE;
        }

        ++sc.p;

        if (0 != zon_take_ident_(&sc, ident, sizeof(ident)))
        {
            return LIBVER_RC_PARSE;
        }

        zon_skip_ws_comments_(&sc);

        if ('=' != zon_peek_(&sc))
        {
            return LIBVER_RC_PARSE;
        }

        ++sc.p;
        zon_skip_ws_comments_(&sc);

        if (0 == strcmp(ident, "version"))
        {
            if (saw_version)
            {
                return LIBVER_RC_PARSE;
            }

            if (0 != zon_take_string_(&sc, out, out_cap))
            {
                return LIBVER_RC_NO_VERSION;
            }

            saw_version = 1;
        }
        else if (0 != zon_skip_value_(&sc))
        {
            return LIBVER_RC_PARSE;
        }

        zon_skip_ws_comments_(&sc);

        if (',' == zon_peek_(&sc))
        {
            ++sc.p;
        }
    }

    return saw_version ? LIBVER_RC_SUCCESS : LIBVER_RC_NO_VERSION;
}


/* /////////////////////////////////////////////////////////////////////////
 * API (internal)
 */

int
libver_backend_zig_probe(
    const char*             dir
,   libver_internal_hit_t*  hit
)
{
    char    path[LIBVER_INTERNAL_PATH_MAX];
    char    version[LIBVER_INTERNAL_VERSION_MAX];
    char*   text = NULL;
    size_t  len = 0;
    int     rc;

    assert(NULL != dir);
    assert(NULL != hit);

    rc = libver_internal_regular_file_in_dir(
            dir
        ,   "build.zig.zon"
        ,   path
        ,   sizeof(path)
        );

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    rc = libver_internal_read_file(path, &text, &len);

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    rc = zon_extract_top_level_version_(
            text
        ,   len
        ,   version
        ,   sizeof(version)
        );

    free(text);

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    return libver_internal_hit_set_version(
            hit
        ,   LIBVER_SCHEME_ZIG
        ,   path
        ,   version
        );
}


/* ///////////////////////////// end of file //////////////////////////// */
