/* /////////////////////////////////////////////////////////////////////////
 * File:    libver/libver.h
 *
 * Purpose: Definition of the libver core API.
 *
 * Created: 10th August 2026
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


/** \file libver/libver.h Definition of the libver core API
 */

#ifndef LIBVER_INCL_LIBVER_H_LIBVER
#define LIBVER_INCL_LIBVER_H_LIBVER


/* /////////////////////////////////////////////////////////////////////////
 * version
 */

/** \def LIBVER_VER_MAJOR
 * The major version number of Synesis Information Systems' libver.
 */

/** \def LIBVER_VER_MINOR
 * The minor version number of Synesis Information Systems' libver.
 */

/** \def LIBVER_VER_PATCH
 * The patch version number of Synesis Information Systems' libver.
 */

/** \def LIBVER_VER_ALPHABETA
 * The alpha/beta/rc designator of Synesis Information Systems' libver,
 * where alpha versions are from 0x41, beta versions are from 0x81, release
 * candidates are from 0xC1, and release versions are 0xFF.
 */

/** \def LIBVER_VER
 * The current composite version number of Synesis Information Systems'
 * libver.
 */


#define LIBVER_VER_MAJOR        0
#define LIBVER_VER_MINOR        0
#define LIBVER_VER_PATCH        4
#define LIBVER_VER_ALPHABETA    0xFF
#define LIBVER_VER_REVISION     LIBVER_VER_PATCH

#define LIBVER_VER \
    (0\
        |   (   LIBVER_VER_MAJOR       << 24   ) \
        |   (   LIBVER_VER_MINOR       << 16   ) \
        |   (   LIBVER_VER_PATCH       <<  8   ) \
        |   (   LIBVER_VER_ALPHABETA   <<  0   ) \
    )

#ifndef SYNSOFT_DOCUMENTATION_SKIP_SECTION
# define LIBVER_VER_REVISION    LIBVER_VER_PATCH
#endif /* !SYNSOFT_DOCUMENTATION_SKIP_SECTION */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

 #include <stddef.h>


 /* /////////////////////////////////////////////////////////////////////////
 * language
 */

#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////////////////////
 * constants
 */

/** Success. */
#define LIBVER_RC_SUCCESS                                   (0)

/** No recognised project marker in the search directory. */
#define LIBVER_RC_NO_MATCH                                  (1)

/** Search directory does not exist. */
#define LIBVER_RC_DIR_NOT_FOUND                             (2)

/** Search directory exists but is not usable (not a directory, or not
 * readable).
 */
#define LIBVER_RC_DIR_NOT_READABLE                          (3)

/** A marker was found but could not be parsed. */
#define LIBVER_RC_PARSE                                     (4)

/** A marker was found but no usable version field was present. */
#define LIBVER_RC_NO_VERSION                                (5)

/** Memory allocation failed. */
#define LIBVER_RC_NO_MEMORY                                 (6)

/** I/O failure while reading a marker file. */
#define LIBVER_RC_IO                                        (7)

/** Invalid argument (defensive; may also be a precondition violation). */
#define LIBVER_RC_INVALID                                   (-1)

/** Scheme name for Cargo / Rust (`Cargo.toml`). */
#define LIBVER_SCHEME_CARGO                                 "cargo"

/** Scheme name for Python (`pyproject.toml`, then legacy markers). */
#define LIBVER_SCHEME_PYTHON                                "python"

/** Scheme name for Zig (`build.zig.zon`). */
#define LIBVER_SCHEME_ZIG                                   "zig"

/** @a schemes value selecting every known scheme in precedence order. */
#define LIBVER_SCHEMES_ALL                                  "*"

/** Warning kind: multiple version sources within one ecosystem disagree.
 * Emitted for Python when pyproject.toml / setup.py / __init__.py version
 * strings differ; Cargo and Zig each have a single marker, so they do not
 * emit this kind.
 */
#define LIBVER_WARNING_INCONSISTENT_SOURCES                 "inconsistent-sources"

/** Warning kind: another selected ecosystem's marker is present beside
 * the precedence winner.
 */
#define LIBVER_WARNING_OTHER_ECOSYSTEM                      "other-ecosystem"


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

struct libver_scheme_result_t
{
    const char* scheme;
    int         major;
    int         minor;
    int         patch;
    /** Synesis prerelease encoding when it maps; 0xFF for a release
     * (no prerelease); 0 when the prerelease string is not Synesis-style.
     */
    int         alphabeta;
    /** Integer build metadata when @a build_metadata is a decimal integer;
     * otherwise 0.
     */
    int         build;
    const char* version;
    /** SemVer prerelease without the leading '-'; empty when absent. */
    char const* prerelease;
    /** SemVer build metadata without the leading '+'; empty when absent. */
    char const* build_metadata;
    const char* source;
};
#ifndef __cplusplus
typedef struct libver_scheme_result_t                       libver_scheme_result_t;
#endif /* !__cplusplus */

struct libver_warning_t
{
    char const* kind;
    char const* scheme;
    char const* source;
    char const* message;
};
#ifndef __cplusplus
typedef struct libver_warning_t                             libver_warning_t;
#endif /* !__cplusplus */


struct libver_result_t
{
    size_t                  num_schemes;
    libver_scheme_result_t* schemes;
    size_t                  num_warnings;
    libver_warning_t*       warnings;
};
#ifndef __cplusplus
typedef struct libver_result_t                              libver_result_t;
#endif /* !__cplusplus */


/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** Initialises the API
 *
 * @note This function is thread-safe, and may be called multiple times.
 *   Each additional call increments a reference count, and each successful
 *   call must be matched by a corresponding call to libver_uninit().
 *
 * @return
 *   - 0 on success;
 *   - non-zero on failure, being an error code;
 */
int
libver_init(
    void* reserved0
);

/** Uninitialises the API
 *
 * @note This function is thread-safe, and may be called multiple times.
 *   Each additional call decrements a reference count, and the final call
 *   must match the initial call to libver_init().
 *
 * @return
 *   - 0 on success;
 *   - non-zero on failure, being an error code;
 *
 * @pre Behaviour is undefined if called more times than libver_init() has
 *   returned 0.
 */
void
libver_uninit(void);

/** Finds the definitive project version(s) for @a dir.
 *
 * Probes @a dir (not recursively) in documented precedence order: Cargo
 * (`Cargo.toml`), Zig (`build.zig.zon`), then Python (`pyproject.toml`,
 * then same-directory `setup.py` / `__init__.py` and one-level package
 * `__init__.py`). The first matching scheme that yields a version is the
 * winner. Other selected schemes whose markers are also present are
 * recorded as LIBVER_WARNING_OTHER_ECOSYSTEM warnings; they do not change
 * the return code. Disagreeing Python sources add
 * LIBVER_WARNING_INCONSISTENT_SOURCES (winner unchanged).
 *
 * @param dir
 *   The directory to search for project version(s);
 * @param flags
 *   Reserved; pass 0;
 * @param schemes
 *   LIBVER_SCHEMES_ALL ("*") for every known scheme, or a single scheme
 *   name (LIBVER_SCHEME_CARGO, LIBVER_SCHEME_PYTHON, LIBVER_SCHEME_ZIG);
 * @param result
 *   The result of the search. On success the caller must eventually call
 *   libver_result_free(). On failure @a result is empty;
 *
 * @return
 *   - LIBVER_RC_SUCCESS (0) when a version was found;
 *   - LIBVER_RC_NO_MATCH when no selected marker is present;
 *   - other non-zero LIBVER_RC_* codes on failure;
 *
 * @pre Behaviour is undefined if @a dir is NULL or empty;
 * @pre Behaviour is undefined if @a schemes is NULL or empty;
 * @pre Behaviour is undefined if @a result is NULL;
 */
int
libver_find(
    const char*         dir     /* [in] */
,   int                 flags   /* [in] */
,   const char*         schemes /* [in] */
,   libver_result_t*    result  /* [out] */
);

/** Release storage owned by @a result.
 *
 * Frees scheme and warning records (and their strings). All pointers in
 * @a result are owned by the result object.
 *
 * @param result
 *   The result to release;
 *
 * @pre Behaviour is undefined if @a result is NULL.
 */
void
libver_result_free(
    libver_result_t*    result
);


/* /////////////////////////////////////////////////////////////////////////
 * language
 */

#ifdef __cplusplus
} /* extern "C" */
#endif


/* /////////////////////////////////////////////////////////////////////////
 * inclusion control
 */

#ifdef STLSOFT_CF_PRAGMA_ONCE_SUPPORT
# pragma once
#endif /* STLSOFT_CF_PRAGMA_ONCE_SUPPORT */

#endif /* !LIBVER_INCL_LIBVER_H_LIBVER */

/* ///////////////////////////// end of file //////////////////////////// */
