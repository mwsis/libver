/* /////////////////////////////////////////////////////////////////////////
 * File:    util.c
 *
 * Purpose: Shared path, file, SemVer, and result helpers for libver.
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


/** \file util.c Shared path, file, SemVer, and result helpers for libver
 */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include "libver.internal.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef _WIN32
# include <unistd.h>
#endif /* !_WIN32 */

#ifndef S_ISDIR
# define S_ISDIR(m)                                         (((m) & S_IFMT) == S_IFDIR)
#endif /* !S_ISDIR */
#ifndef S_ISREG
# define S_ISREG(m)                                         (((m) & S_IFMT) == S_IFREG)
#endif /* !S_ISREG */


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static int
ends_with_sep_(
    const char* s
)
{
    size_t const n = strlen(s);

    if (0 == n)
    {
        return 0;
    }

    return '/' == s[n - 1] || '\\' == s[n - 1];
}

static int
parse_uint_(
    const char**    pp
,   int*            out
)
{
    const char* p = *pp;
    long        v = 0;

    if (!isdigit((unsigned char)*p))
    {
        return 1;
    }

    while (isdigit((unsigned char)*p))
    {
        v = v * 10 + (*p - '0');

        if (v > INT_MAX)
        {
            return 1;
        }

        ++p;
    }

    *out = (int)v;
    *pp = p;

    return 0;
}

static int
skip_prerelease_or_build_(
    const char**    pp
,   int             plus_ok
)
{
    const char* p = *pp;

    if ('\0' == *p)
    {
        return 1;
    }

    for (; '\0' != *p; ++p)
    {
        unsigned char const c = (unsigned char)*p;

        if ('.' == c || '-' == c || '_' == c || isalnum(c))
        {
            continue;
        }

        if (plus_ok && '+' == c)
        {
            break;
        }

        return 1;
    }

    *pp = p;

    return 0;
}


/* /////////////////////////////////////////////////////////////////////////
 * API (internal)
 */

int
libver_internal_check_dir(
    const char* dir
)
{
    struct stat st;

    assert(NULL != dir);
    assert('\0' != *dir);

    if (0 != stat(dir, &st))
    {
        if (ENOENT == errno)
        {
            return LIBVER_RC_DIR_NOT_FOUND;
        }

        return LIBVER_RC_DIR_NOT_READABLE;
    }

    if (!S_ISDIR(st.st_mode))
    {
        return LIBVER_RC_DIR_NOT_READABLE;
    }

#ifndef _WIN32

    if (0 != access(dir, R_OK | X_OK))
    {
        return LIBVER_RC_DIR_NOT_READABLE;
    }
#endif /* !_WIN32 */

    return LIBVER_RC_SUCCESS;
}

int
libver_internal_join_path(
    char*       dest
,   size_t      dest_cap
,   const char* dir
,   const char* name
)
{
    int n;

    assert(NULL != dest);
    assert(NULL != dir);
    assert(NULL != name);
    assert(0 != dest_cap);

    if (ends_with_sep_(dir))
    {
        n = snprintf(dest, dest_cap, "%s%s", dir, name);
    }
    else
    {
        n = snprintf(dest, dest_cap, "%s/%s", dir, name);
    }

    if (n < 0 || (size_t)n >= dest_cap)
    {
        return 1;
    }

    return 0;
}

int
libver_internal_regular_file_in_dir(
    const char* dir
,   const char* name
,   char*       path_out
,   size_t      path_cap
)
{
    struct stat st;

    assert(NULL != dir);
    assert(NULL != name);
    assert(NULL != path_out);

    if (0 != libver_internal_join_path(path_out, path_cap, dir, name))
    {
        return LIBVER_RC_IO;
    }

    if (0 != stat(path_out, &st))
    {
        if (ENOENT == errno)
        {
            return LIBVER_RC_NO_MATCH;
        }

        return LIBVER_RC_IO;
    }

    if (!S_ISREG(st.st_mode))
    {
        return LIBVER_RC_PARSE;
    }

    return LIBVER_RC_SUCCESS;
}

int
libver_internal_read_file(
    const char* path
,   char**      text
,   size_t*     len
)
{
    FILE*   fp;
    long    size;
    char*   buf;
    size_t  nread;

    assert(NULL != path);
    assert(NULL != text);

    *text = NULL;

    if (NULL != len)
    {
        *len = 0;
    }

    fp = fopen(path, "rb");

    if (NULL == fp)
    {
        return LIBVER_RC_IO;
    }

    if (0 != fseek(fp, 0, SEEK_END))
    {
        fclose(fp);

        return LIBVER_RC_IO;
    }

    size = ftell(fp);

    if (size < 0)
    {
        fclose(fp);

        return LIBVER_RC_IO;
    }

    if (size > 1024 * 1024)
    {
        fclose(fp);

        return LIBVER_RC_PARSE;
    }

    if (0 != fseek(fp, 0, SEEK_SET))
    {
        fclose(fp);

        return LIBVER_RC_IO;
    }

    buf = (char*)malloc((size_t)size + 1);

    if (NULL == buf)
    {
        fclose(fp);

        return LIBVER_RC_NO_MEMORY;
    }

    nread = fread(buf, 1, (size_t)size, fp);

    if (nread != (size_t)size && 0 != ferror(fp))
    {
        free(buf);
        fclose(fp);

        return LIBVER_RC_IO;
    }

    fclose(fp);

    buf[nread] = '\0';
    *text = buf;

    if (NULL != len)
    {
        *len = nread;
    }

    return LIBVER_RC_SUCCESS;
}

int
libver_internal_parse_semver(
    const char* s
,   int*        major
,   int*        minor
,   int*        patch
)
{
    const char* p;

    assert(NULL != s);
    assert(NULL != major);
    assert(NULL != minor);
    assert(NULL != patch);

    p = s;

    if (0 != parse_uint_(&p, major))
    {
        return 1;
    }

    if ('.' != *p)
    {
        return 1;
    }

    ++p;

    if (0 != parse_uint_(&p, minor))
    {
        return 1;
    }

    if ('.' != *p)
    {
        return 1;
    }

    ++p;

    if (0 != parse_uint_(&p, patch))
    {
        return 1;
    }

    if ('-' == *p)
    {
        ++p;

        if (0 != skip_prerelease_or_build_(&p, 1))
        {
            return 1;
        }
    }

    if ('+' == *p)
    {
        ++p;

        if (0 != skip_prerelease_or_build_(&p, 0))
        {
            return 1;
        }
    }

    return '\0' == *p ? 0 : 1;
}

int
libver_internal_hit_set_version(
    libver_internal_hit_t*  hit
,   const char*             scheme
,   const char*             source
,   const char*             version
)
{
    size_t source_len;
    size_t version_len;

    assert(NULL != hit);
    assert(NULL != scheme);
    assert(NULL != source);
    assert(NULL != version);

    source_len = strlen(source);
    version_len = strlen(version);

    if (source_len >= LIBVER_INTERNAL_PATH_MAX ||
        version_len >= LIBVER_INTERNAL_VERSION_MAX ||
        0 == version_len)
    {
        return LIBVER_RC_NO_VERSION;
    }

    if (0 != libver_internal_parse_semver(
                version
            ,   &hit->major
            ,   &hit->minor
            ,   &hit->patch
            ))
    {
        return LIBVER_RC_NO_VERSION;
    }

    hit->scheme = scheme;
    hit->alphabeta = 0;
    hit->build = 0;

    memcpy(hit->source, source, source_len + 1);
    memcpy(hit->version, version, version_len + 1);

    return LIBVER_RC_SUCCESS;
}

int
libver_internal_result_set(
    libver_result_t*                result
,   libver_internal_hit_t const*    hit
)
{
    size_t                  scheme_len;
    size_t                  version_len;
    size_t                  source_len;
    size_t                  bytes;
    char*                   block;
    libver_scheme_result_t* sr;
    char*                   p;

    assert(NULL != result);
    assert(NULL != hit);
    assert(NULL != hit->scheme);

    scheme_len = strlen(hit->scheme);
    version_len = strlen(hit->version);
    source_len = strlen(hit->source);

    bytes = sizeof(*sr) + scheme_len + 1 + version_len + 1 + source_len + 1;
    block = (char*)malloc(bytes);

    if (NULL == block)
    {
        return LIBVER_RC_NO_MEMORY;
    }

    sr = (libver_scheme_result_t*)(void*)block;
    p = (char*)(sr + 1);

    memcpy(p, hit->scheme, scheme_len + 1);
    sr->scheme = p;
    p += scheme_len + 1;

    memcpy(p, hit->version, version_len + 1);
    sr->version = p;
    p += version_len + 1;

    memcpy(p, hit->source, source_len + 1);
    sr->source = p;

    sr->major = hit->major;
    sr->minor = hit->minor;
    sr->patch = hit->patch;
    sr->alphabeta = hit->alphabeta;
    sr->build = hit->build;

    result->num_schemes = 1;
    result->schemes = sr;

    return LIBVER_RC_SUCCESS;
}


/* ///////////////////////////// end of file //////////////////////////// */
