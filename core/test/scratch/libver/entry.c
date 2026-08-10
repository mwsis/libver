/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/scratch/libver/entry.c
 *
 * Purpose: Scratch program that prints the libver library version.
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


/** \file core/test/scratch/libver/entry.c Scratch program that prints libver's version
 */

/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <libver/libver.h>

#include <diagnosticism/version_string.h>

#include <stdio.h>
#include <stdlib.h>


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char* argv[])
{
    char    buffer[101];
    size_t  n;
    int     r;

    ((void)argc);
    ((void)argv);

    {
        /* Composite release marker is 0xFF; version_string uses 0xFFFF to
         * suppress a pre-release / fourth-component suffix.
         */
        uint16_t const  alphabeta = (0xFF == (LIBVER_VER_ALPHABETA & 0xFF))
            ? (uint16_t)0xFFFF
            : (uint16_t)LIBVER_VER_ALPHABETA
            ;

        r = diagnosticism_calc_version_string(
            buffer
        ,   sizeof(buffer) / sizeof(buffer[0])
        ,   (uint16_t)LIBVER_VER_MAJOR
        ,   (uint16_t)LIBVER_VER_MINOR
        ,   (uint16_t)LIBVER_VER_PATCH
        ,   alphabeta
        ,   &n
        );
    }

    if (0 != r)
    {
        fprintf(stderr, "libver: failed to format version string\n");

        return EXIT_FAILURE;
    }

    buffer[n] = '\0';

    fprintf(stdout, "libver %s\n", buffer);

    return EXIT_SUCCESS;
}


/* ///////////////////////////// end of file //////////////////////////// */
