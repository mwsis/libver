/* /////////////////////////////////////////////////////////////////////////
 * File:    libver.internal.h
 *
 * Purpose: Internal declarations for the libver core and backends.
 *
 * Created: 16th September 2026
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


/** \file libver.internal.h Internal declarations for the libver core
 */

#ifndef LIBVER_INCL_SRC_H_LIBVER_INTERNAL
#define LIBVER_INCL_SRC_H_LIBVER_INTERNAL


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <libver/libver.h>

#include <stddef.h>


/* /////////////////////////////////////////////////////////////////////////
 * constants
 */

#define LIBVER_INTERNAL_PATH_MAX                            (4096)
#define LIBVER_INTERNAL_VERSION_MAX                         (128)
#define LIBVER_INTERNAL_MESSAGE_MAX                         (LIBVER_INTERNAL_PATH_MAX + 128)
#define LIBVER_INTERNAL_MAX_SCHEMES                         (16)


/* /////////////////////////////////////////////////////////////////////////
 * types
 */

struct libver_internal_hit_t
{
    const char* scheme;
    char        source[LIBVER_INTERNAL_PATH_MAX];
    char        version[LIBVER_INTERNAL_VERSION_MAX];
    char        prerelease[LIBVER_INTERNAL_VERSION_MAX];
    char        build_metadata[LIBVER_INTERNAL_VERSION_MAX];
    int         major;
    int         minor;
    int         patch;
    int         alphabeta;
    int         build;
};
#ifndef __cplusplus
typedef struct libver_internal_hit_t                        libver_internal_hit_t;
#endif /* !__cplusplus */

struct libver_internal_warning_t
{
    char const* kind;
    char const* scheme;
    char        source[LIBVER_INTERNAL_PATH_MAX];
    char        message[LIBVER_INTERNAL_MESSAGE_MAX];
};
#ifndef __cplusplus
typedef struct libver_internal_warning_t                    libver_internal_warning_t;
#endif /* !__cplusplus */


/* /////////////////////////////////////////////////////////////////////////
 * helpers
 */

/** Classify @a dir as a usable search root.
 *
 * @return LIBVER_RC_SUCCESS, LIBVER_RC_DIR_NOT_FOUND, or
 *   LIBVER_RC_DIR_NOT_READABLE.
 */
int
libver_internal_check_dir(
    const char* dir
);

/** Join @a dir and @a name into @a dest.
 *
 * @return 0 on success; non-zero if the result would not fit.
 */
int
libver_internal_join_path(
    char*       dest
,   size_t      dest_cap
,   const char* dir
,   const char* name
);

/** Locate a regular file @a name under @a dir.
 *
 * @return
 *   - LIBVER_RC_SUCCESS and fills @a path_out;
 *   - LIBVER_RC_NO_MATCH if the path does not exist;
 *   - LIBVER_RC_PARSE if the path exists but is not a regular file;
 *   - LIBVER_RC_IO on other access failures.
 */
int
libver_internal_regular_file_in_dir(
    const char* dir
,   const char* name
,   char*       path_out
,   size_t      path_cap
);

/** Read the whole of @a path into a NUL-terminated malloc'd buffer.
 *
 * @param text  [out] Set to the buffer; caller must free().
 * @param len   [out] Byte length excluding the NUL; may be NULL.
 *
 * @return LIBVER_RC_SUCCESS, LIBVER_RC_IO, or LIBVER_RC_NO_MEMORY.
 */
int
libver_internal_read_file(
    const char* path
,   char**      text
,   size_t*     len
);

/** Parse a SemVer core `MAJOR.MINOR.PATCH` with optional prerelease/build.
 *
 * Slice outputs (into @a s) may be NULL. Lengths are 0 when absent.
 *
 * @return 0 on success; non-zero if @a s is not a usable SemVer string.
 */
int
libver_internal_parse_semver(
    const char*     s
,   int*            major
,   int*            minor
,   int*            patch
,   char const**    prerelease
,   size_t*         prerelease_len
,   char const**    build_metadata
,   size_t*         build_metadata_len
);

/** Copy @a scheme and @a version into @a hit and parse SemVer fields.
 *
 * @return LIBVER_RC_SUCCESS or LIBVER_RC_NO_VERSION.
 */
int
libver_internal_hit_set_version(
    libver_internal_hit_t*  hit
,   const char*             scheme
,   const char*             source
,   const char*             version
);

/** Extract the first `version=` / `__version__` quoted literal from
 * @a text.
 *
 * @return 0 on success; non-zero if none found.
 */
int
libver_internal_python_extract_literal(
    char const* text
,   char*       out
,   size_t      out_cap
);

/** Populate @a result from a single @a hit and optional warnings.
 *
 * @return LIBVER_RC_SUCCESS or LIBVER_RC_NO_MEMORY.
 */
int
libver_internal_result_set(
    libver_result_t*                    result
,   libver_internal_hit_t const*        hit
,   libver_internal_warning_t const*    warnings
,   size_t                              num_warnings
);


/* /////////////////////////////////////////////////////////////////////////
 * backends
 */

int
libver_backend_cargo_probe(
    char const*                 dir
,   libver_internal_hit_t*      hit
,   libver_internal_warning_t*  warnings
,   size_t                      warnings_cap
,   size_t*                     num_warnings
);

int
libver_backend_python_present(
    char const* dir
,   char*       path_out
,   size_t      path_cap
);

int
libver_backend_python_probe(
    char const*                 dir
,   libver_internal_hit_t*      hit
,   libver_internal_warning_t*  warnings
,   size_t                      warnings_cap
,   size_t*                     num_warnings
);

int
libver_backend_zig_probe(
    char const*                 dir
,   libver_internal_hit_t*      hit
,   libver_internal_warning_t*  warnings
,   size_t                      warnings_cap
,   size_t*                     num_warnings
);


/* /////////////////////////////////////////////////////////////////////////
 * inclusion control
 */

#endif /* !LIBVER_INCL_SRC_H_LIBVER_INTERNAL */

/* ///////////////////////////// end of file //////////////////////////// */
