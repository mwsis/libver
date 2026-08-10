/* /////////////////////////////////////////////////////////////////////////
 * File:    core/test/scratch/rust/entry.c
 *
 * Purpose: Scratch program: read [package].version from a Cargo.toml via
 *          tomlc17.
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


/** \file core/test/scratch/rust/entry.c Scratch: Cargo.toml version via tomlc17
 *
 * Usage: test.scratch.rust <path-to-Cargo.toml>
 */


/* /////////////////////////////////////////////////////////////////////////
 * includes
 */

#include <tomlc17.h>

#include <stdio.h>
#include <stdlib.h>


/* /////////////////////////////////////////////////////////////////////////
 * main
 */

int main(int argc, char** argv)
{
    const char*     path;
    toml_result_t   result;
    toml_datum_t    version;

    if (2 != argc)
    {
        fprintf(stderr, "usage: %s <path-to-Cargo.toml>\n", argv[0]);

        return EXIT_FAILURE;
    }

    path = argv[1];

    result = toml_parse_file_ex(path);

    if (!result.ok)
    {
        fprintf(stderr, "%s: %s\n", path, result.errmsg);

        return EXIT_FAILURE;
    }

    version = toml_seek(result.toptab, "package.version");

    if (TOML_STRING != version.type)
    {
        fprintf(stderr, "%s: missing or invalid [package].version\n", path);
        toml_free(result);

        return EXIT_FAILURE;
    }

    fprintf(stdout, "%s\n", version.u.s);

    toml_free(result);

    return EXIT_SUCCESS;
}


/* ///////////////////////////// end of file //////////////////////////// */
