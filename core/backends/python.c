/* /////////////////////////////////////////////////////////////////////////
 * File:    python.c
 *
 * Purpose: Python backend: pyproject.toml [project].version via tomlc17,
 *          then legacy setup.py / __init__.py version literals.
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


/** \file python.c Python version extraction (pyproject.toml, then legacy)
 */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include "libver.internal.h"

#include <tomlc17.h>

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
# include <windows.h>
#else /* ? _WIN32 */
# include <dirent.h>
#endif /* _WIN32 */

#ifndef S_ISDIR
# define S_ISDIR(m)                                         (((m) & S_IFMT) == S_IFDIR)
#endif /* !S_ISDIR */


/* /////////////////////////////////////////////////////////////////////////
 * constants
 */

#define PYTHON_NAME_MAX_                                    (256)
#define PYTHON_HIT_CAP_                                     (32)
#define PYTHON_SUBDIR_CAP_                                  (256)
#define PYTHON_LINE_MAX_                                    (1024)


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

struct python_hit_t
{
    char    source[LIBVER_INTERNAL_PATH_MAX];
    char    version[LIBVER_INTERNAL_VERSION_MAX];
};
typedef struct python_hit_t                                 python_hit_t;


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

static libver_truthy_t
is_dots_(
    char const* s
)
{
    assert(NULL != s);

    switch (s[0])
    {
    case '.':

        switch (s[1])
        {
        case '.':

            switch (s[2])
            {
            case '\0':

                return LIBVER_TRUTHY_TRUE_; /* ".." */
            default:

                return LIBVER_TRUTHY_FALSE_;
            }
        case '\0':

            return LIBVER_TRUTHY_TRUE_; /* "." */
        default:

            return LIBVER_TRUTHY_FALSE_;
        }
        break;
    case '\0':
    default:

        return LIBVER_TRUTHY_FALSE_; /* anything else */
    }
}


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

static int
try_extract_specifier_(
    char const* line
,   char*       value
,   size_t      value_cap
)
{
    char const* p = skip_ws_(line);
    char const* after_name;

    if (0 == strncmp(p, "__version__", 11))
    {
        after_name = p + 11;
    }
    else if (0 == strncmp(p, "version", 7) &&
             !isalnum((unsigned char)p[7]) &&
             '_' != p[7])
    {
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

    return parse_quoted_(after_name + 1, value, value_cap);
}

int
libver_internal_python_extract_literal(
    char const* text
,   char*       out
,   size_t      out_cap
)
{
    char const* p;

    assert(NULL != text);
    assert(NULL != out);
    assert(0 != out_cap);

    p = text;

    while ('\0' != *p)
    {
        char    line[PYTHON_LINE_MAX_];
        size_t  n = 0;

        while ('\0' != *p && '\n' != *p && '\r' != *p)
        {
            if (n + 1 < sizeof(line))
            {
                line[n++] = *p;
            }

            ++p;
        }

        line[n] = '\0';

        if ('\r' == *p)
        {
            ++p;
        }

        if ('\n' == *p)
        {
            ++p;
        }

        if (0 == try_extract_specifier_(line, out, out_cap))
        {
            return 0;
        }
    }

    return 1;
}

static int
cmp_name_(
    void const* a
,   void const* b
)
{
    return strcmp((char const*)a, (char const*)b);
}

static int
list_subdirs_(
    char const* dir
,   char        names[][PYTHON_NAME_MAX_]
,   size_t      cap
,   size_t*     count
)
{
    assert(NULL != dir);
    assert(NULL != names);
    assert(NULL != count);

    *count = 0;

    {
#ifdef _WIN32

        char                pattern[LIBVER_INTERNAL_PATH_MAX];
        HANDLE              h;
        WIN32_FIND_DATAA    fd;

        if (0 != libver_internal_join_path(pattern, sizeof(pattern), dir, "*"))
        {
            return LIBVER_RC_IO;
        }

        h = FindFirstFileA(pattern, &fd);

        if (INVALID_HANDLE_VALUE == h)
        {
            return LIBVER_RC_IO;
        }

        do
        {
            size_t n;

            if (0 == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                continue;
            }

            if (is_dots_(fd.cFileName))
            {
                continue;
            }

            n = strlen(fd.cFileName);

            if (n >= PYTHON_NAME_MAX_)
            {
                continue;
            }

            if (*count >= cap)
            {
                break;
            }

            memcpy(names[*count], fd.cFileName, n + 1);
            ++*count;
        } while (FindNextFileA(h, &fd));

        FindClose(h);
#else /* ? _WIN32 */

        DIR* dirp = opendir(dir);

        if (NULL == dirp)
        {
            return LIBVER_RC_IO;
        }

        for (;;)
        {
            struct dirent*  ent;
            struct stat     st;
            char            path[LIBVER_INTERNAL_PATH_MAX];
            size_t          n;

            ent = readdir(dirp);

            if (NULL == ent)
            {
                break;
            }

            if (is_dots_(ent->d_name))
            {
                continue;
            }

            if (0 != libver_internal_join_path(
                        path
                    ,   sizeof(path)
                    ,   dir
                    ,   ent->d_name
                    )
            )
            {
                continue;
            }

            if (0 != stat(path, &st) || !S_ISDIR(st.st_mode))
            {
                continue;
            }

            n = strlen(ent->d_name);

            if (n >= PYTHON_NAME_MAX_)
            {
                continue;
            }

            if (*count >= cap)
            {
                break;
            }

            memcpy(names[*count], ent->d_name, n + 1);
            ++*count;
        }

        closedir(dirp);
#endif /* _WIN32 */
    }

    if (0 != *count)
    {
        qsort(names, *count, PYTHON_NAME_MAX_, cmp_name_);
    }

    return LIBVER_RC_SUCCESS;
}

static int
file_exists_(
    char const* dir
,   char const* name
,   char*       path
,   size_t      path_cap
)
{
    int const rc = libver_internal_regular_file_in_dir(
                        dir
                    ,   name
                    ,   path
                    ,   path_cap
                    );

    return LIBVER_RC_SUCCESS == rc || LIBVER_RC_PARSE == rc;
}

static int
push_hit_from_file_(
    char const*     path
,   python_hit_t*   hits
,   size_t          cap
,   size_t*         count
)
{
    char*   text = NULL;
    char    version[LIBVER_INTERNAL_VERSION_MAX];
    int     rc;
    int     major;
    int     minor;
    int     patch;

    rc = libver_internal_read_file(path, &text, NULL);

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    if (0 != libver_internal_python_extract_literal(
                text
            ,   version
            ,   sizeof(version)
            ))
    {
        free(text);

        return LIBVER_RC_SUCCESS;
    }

    free(text);

    if (0 != libver_internal_parse_semver(
                version
            ,   &major
            ,   &minor
            ,   &patch
            ,   NULL
            ,   NULL
            ,   NULL
            ,   NULL
            ))
    {
        return LIBVER_RC_SUCCESS;
    }

    if (*count >= cap)
    {
        return LIBVER_RC_SUCCESS;
    }

    {
        size_t const path_len = strlen(path);
        size_t const ver_len = strlen(version);

        if (path_len >= LIBVER_INTERNAL_PATH_MAX ||
            ver_len >= LIBVER_INTERNAL_VERSION_MAX)
        {
            return LIBVER_RC_SUCCESS;
        }

        memcpy(hits[*count].source, path, path_len + 1);
        memcpy(hits[*count].version, version, ver_len + 1);
        ++*count;
    }

    return LIBVER_RC_SUCCESS;
}

static int
try_legacy_name_(
    char const*     dir
,   char const*     name
,   python_hit_t*   hits
,   size_t          cap
,   size_t*         count
,   int*            saw_marker
)
{
    char    path[LIBVER_INTERNAL_PATH_MAX];
    int     rc;

    rc = libver_internal_regular_file_in_dir(
            dir
        ,   name
        ,   path
        ,   sizeof(path)
        );

    if (LIBVER_RC_NO_MATCH == rc)
    {
        return LIBVER_RC_SUCCESS;
    }

    if (LIBVER_RC_PARSE == rc)
    {
        *saw_marker = 1;

        return LIBVER_RC_SUCCESS;
    }

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    *saw_marker = 1;

    return push_hit_from_file_(path, hits, cap, count);
}

static int
collect_legacy_(
    char const*     dir
,   python_hit_t*   hits
,   size_t          cap
,   size_t*         count
,   int*            saw_marker
)
{
    char    names[PYTHON_SUBDIR_CAP_][PYTHON_NAME_MAX_];
    size_t  nnames = 0;
    size_t  i;
    int     rc;

    rc = try_legacy_name_(
            dir
        ,   "setup.py"
        ,   hits
        ,   cap
        ,   count
        ,   saw_marker
        );

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    rc = try_legacy_name_(
            dir
        ,   "__init__.py"
        ,   hits
        ,   cap
        ,   count
        ,   saw_marker
        );

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    rc = list_subdirs_(dir, names, PYTHON_SUBDIR_CAP_, &nnames);

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    for (i = 0; i < nnames; ++i)
    {
        char rel[PYTHON_NAME_MAX_ + 16];
        int  n;

        n = snprintf(rel, sizeof(rel), "%s/__init__.py", names[i]);

        if (n < 0 || (size_t)n >= sizeof(rel))
        {
            continue;
        }

        rc = try_legacy_name_(
                dir
            ,   rel
            ,   hits
            ,   cap
            ,   count
            ,   saw_marker
            );

        if (LIBVER_RC_SUCCESS != rc)
        {
            return rc;
        }
    }

    return LIBVER_RC_SUCCESS;
}

static void
fill_inconsistent_(
    libver_internal_warning_t*  w
,   char const*                 source
,   char const*                 version
,   char const*                 winner_version
,   char const*                 winner_source
)
{
    size_t source_len;

    assert(NULL != w);
    assert(NULL != source);
    assert(NULL != version);
    assert(NULL != winner_version);
    assert(NULL != winner_source);

    source_len = strlen(source);

    if (source_len >= LIBVER_INTERNAL_PATH_MAX)
    {
        source_len = LIBVER_INTERNAL_PATH_MAX - 1;
    }

    w->kind = LIBVER_WARNING_INCONSISTENT_SOURCES;
    w->scheme = LIBVER_SCHEME_PYTHON;
    memcpy(w->source, source, source_len);
    w->source[source_len] = '\0';
    snprintf(
        w->message
    ,   sizeof(w->message)
    ,   "also found version '%s' at '%s' (using '%s' from '%s')"
    ,   version
    ,   w->source
    ,   winner_version
    ,   winner_source
    );
}

static void
emit_disagreements_(
    python_hit_t const*         hits
,   size_t                      n_hits
,   size_t                      winner_ix
,   libver_internal_warning_t*  warnings
,   size_t                      warnings_cap
,   size_t*                     num_warnings
)
{
    size_t          i;
    char const*     win_ver = hits[winner_ix].version;
    char const*     win_src = hits[winner_ix].source;

    for (i = 0; i < n_hits; ++i)
    {
        if (i == winner_ix)
        {
            continue;
        }

        if (0 == strcmp(hits[i].version, win_ver))
        {
            continue;
        }

        if (*num_warnings >= warnings_cap)
        {
            return;
        }

        fill_inconsistent_(
            &warnings[*num_warnings]
        ,   hits[i].source
        ,   hits[i].version
        ,   win_ver
        ,   win_src
        );
        ++*num_warnings;
    }
}

static int
commit_winner_(
    python_hit_t const*         hits
,   size_t                      n_hits
,   size_t                      winner_ix
,   libver_internal_hit_t*      hit
,   libver_internal_warning_t*  warnings
,   size_t                      warnings_cap
,   size_t*                     num_warnings
)
{
    int rc;

    rc = libver_internal_hit_set_version(
            hit
        ,   LIBVER_SCHEME_PYTHON
        ,   hits[winner_ix].source
        ,   hits[winner_ix].version
        );

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    emit_disagreements_(
        hits
    ,   n_hits
    ,   winner_ix
    ,   warnings
    ,   warnings_cap
    ,   num_warnings
    );

    return LIBVER_RC_SUCCESS;
}


/* /////////////////////////////////////////////////////////////////////////
 * API (internal)
 */

int
libver_backend_python_present(
    char const* dir
,   char*       path_out
,   size_t      path_cap
)
{
    char    names[PYTHON_SUBDIR_CAP_][PYTHON_NAME_MAX_];
    size_t  nnames = 0;
    size_t  i;

    assert(NULL != dir);
    assert(NULL != path_out);
    assert(0 != path_cap);

    if (file_exists_(dir, "pyproject.toml", path_out, path_cap) ||
        file_exists_(dir, "setup.py", path_out, path_cap) ||
        file_exists_(dir, "__init__.py", path_out, path_cap))
    {
        return 1;
    }

    if (LIBVER_RC_SUCCESS != list_subdirs_(
                                dir
                            ,   names
                            ,   PYTHON_SUBDIR_CAP_
                            ,   &nnames
                            ))
    {
        return 0;
    }

    for (i = 0; i < nnames; ++i)
    {
        char rel[PYTHON_NAME_MAX_ + 16];
        int  n;

        n = snprintf(rel, sizeof(rel), "%s/__init__.py", names[i]);

        if (n < 0 || (size_t)n >= sizeof(rel))
        {
            continue;
        }

        if (file_exists_(dir, rel, path_out, path_cap))
        {
            return 1;
        }
    }

    return 0;
}

int
libver_backend_python_probe(
    char const*                 dir
,   libver_internal_hit_t*      hit
,   libver_internal_warning_t*  warnings
,   size_t                      warnings_cap
,   size_t*                     num_warnings
)
{
    char            path[LIBVER_INTERNAL_PATH_MAX];
    int             rc;
    int             pyproject_present = 0;
    int             saw_marker = 0;
    python_hit_t    hits[PYTHON_HIT_CAP_];
    size_t          n_hits = 0;
    toml_result_t   parsed;
    toml_datum_t    version;

    assert(NULL != dir);
    assert(NULL != hit);

    if (NULL != num_warnings)
    {
        *num_warnings = 0;
    }

    rc = libver_internal_regular_file_in_dir(
            dir
        ,   "pyproject.toml"
        ,   path
        ,   sizeof(path)
        );

    if (LIBVER_RC_PARSE == rc || LIBVER_RC_IO == rc)
    {
        return rc;
    }

    if (LIBVER_RC_SUCCESS == rc)
    {
        pyproject_present = 1;
        saw_marker = 1;

        parsed = toml_parse_file_ex(path);

        if (!parsed.ok)
        {
            toml_free(parsed);

            return LIBVER_RC_PARSE;
        }

        version = toml_seek(parsed.toptab, "project.version");

        if (TOML_STRING == version.type && NULL != version.u.s)
        {
            rc = libver_internal_hit_set_version(
                    hit
                ,   LIBVER_SCHEME_PYTHON
                ,   path
                ,   version.u.s
                );

            toml_free(parsed);

            if (LIBVER_RC_SUCCESS != rc)
            {
                return rc;
            }

            rc = collect_legacy_(
                    dir
                ,   hits
                ,   PYTHON_HIT_CAP_
                ,   &n_hits
                ,   &saw_marker
                );

            if (LIBVER_RC_SUCCESS != rc)
            {
                return rc;
            }

            if (NULL != warnings && NULL != num_warnings)
            {
                size_t i;

                for (i = 0; i < n_hits; ++i)
                {
                    if (0 == strcmp(hits[i].version, hit->version))
                    {
                        continue;
                    }

                    if (*num_warnings >= warnings_cap)
                    {
                        break;
                    }

                    fill_inconsistent_(
                        &warnings[*num_warnings]
                    ,   hits[i].source
                    ,   hits[i].version
                    ,   hit->version
                    ,   hit->source
                    );
                    ++*num_warnings;
                }
            }

            return LIBVER_RC_SUCCESS;
        }

        toml_free(parsed);
    }

    rc = collect_legacy_(
            dir
        ,   hits
        ,   PYTHON_HIT_CAP_
        ,   &n_hits
        ,   &saw_marker
        );

    if (LIBVER_RC_SUCCESS != rc)
    {
        return rc;
    }

    if (0 != n_hits)
    {
        if (NULL == warnings || NULL == num_warnings)
        {
            return libver_internal_hit_set_version(
                    hit
                ,   LIBVER_SCHEME_PYTHON
                ,   hits[0].source
                ,   hits[0].version
                );
        }

        return commit_winner_(
                hits
            ,   n_hits
            ,   0
            ,   hit
            ,   warnings
            ,   warnings_cap
            ,   num_warnings
            );
    }

    if (pyproject_present || saw_marker)
    {
        return LIBVER_RC_NO_VERSION;
    }

    return LIBVER_RC_NO_MATCH;
}


/* ///////////////////////////// end of file //////////////////////////// */
