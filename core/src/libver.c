/* /////////////////////////////////////////////////////////////////////////
 * File:    libver.c
 *
 * Purpose: Implementation of the libver core API.
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


/** \file libver.c Implementation of the libver core API
 */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <libver/libver.h>

#include <assert.h>
#include <string.h>


/* /////////////////////////////////////////////////////////////////////////
 * globals
 */


/* /////////////////////////////////////////////////////////////////////////
 * helper functions
 */


/* /////////////////////////////////////////////////////////////////////////
 * API
 */

int
libver_init(
    void*
)
{
    /* Stub: no global state yet. */
    return 0;
}

void
libver_uninit(void)
{
    /* Stub: no global state yet. */
}

int
libver_find(
    const char*         dir
,   int                 flags
,   const char*         schemes
,   libver_result_t*    result
)
{
    (void)dir;
    (void)flags;
    (void)schemes;

    if (NULL == result)
    {
        return -1;
    }

    result->num_schemes =   0;
    result->schemes     =   NULL;

    /* Stub: no detection yet. */
    return 1;
}

void
libver_result_free(
    libver_result_t*    result
)
{
    if (NULL == result)
    {
        return;
    }

    result->num_schemes =   0;
    result->schemes     =   NULL;
}


/* ///////////////////////////// end of file //////////////////////////// */
