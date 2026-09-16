# **libver** C-API <!-- omit in toc -->

Status: Phase 4 contract — matches **libver/libver.h**. Integer `LIBVER_RC_*`
codes and structured warning records are the v0 result model.


## Table of Contents <!-- omit in toc -->

- [Entry points](#entry-points)
- [Status codes](#status-codes)
- [Schemes](#schemes)
- [Result model](#result-model)
- [Warnings](#warnings)
- [Ownership and lifetime](#ownership-and-lifetime)
- [Errors and conflicts](#errors-and-conflicts)
- [CLI mapping](#cli-mapping)
- [Open questions](#open-questions)


## Entry points

```c
int libver_init(void* reserved0);   /* pass NULL */
void libver_uninit(void);

int libver_find(
    const char*         dir     /* [in] */
,   int                 flags   /* [in] */
,   const char*         schemes /* [in] */
,   libver_result_t*    result  /* [out] */
);
void libver_result_free(
    libver_result_t*    result
);
```

* `dir` is a directory path; NULL or empty is a precondition violation (not "use cwd"). The **libver** / **cargo-libver** CLIs resolve an omitted directory to the process cwd before calling the API;
* `flags` is reserved; pass 0;
* `schemes` is `LIBVER_SCHEMES_ALL` (`"*"`) or a single scheme name;
* On `LIBVER_RC_SUCCESS`, `result` is populated and the caller must call `libver_result_free`;
* On failure, `result` is empty (`num_schemes == 0`, `schemes == NULL`, `num_warnings == 0`, `warnings == NULL`);


## Status codes

Integer macros (not a dedicated enum). Kept as an integer table for FFI:
callers can switch on `int` without a C enum ABI.

| Code | Value | Meaning |
| --- | ---: | --- |
| `LIBVER_RC_SUCCESS` | 0 | A version was found (warnings do not change this) |
| `LIBVER_RC_NO_MATCH` | 1 | No selected marker in `dir` |
| `LIBVER_RC_DIR_NOT_FOUND` | 2 | `dir` does not exist |
| `LIBVER_RC_DIR_NOT_READABLE` | 3 | `dir` is not a usable directory |
| `LIBVER_RC_PARSE` | 4 | Marker found but could not be parsed |
| `LIBVER_RC_NO_VERSION` | 5 | Marker found but no usable version |
| `LIBVER_RC_NO_MEMORY` | 6 | Allocation failed |
| `LIBVER_RC_IO` | 7 | I/O failure reading a marker |
| `LIBVER_RC_INVALID` | -1 | Invalid argument (defensive) |


## Schemes

| Macro | Value | Marker |
| --- | --- | --- |
| `LIBVER_SCHEME_CARGO` | `"cargo"` | `Cargo.toml` (`[package].version`) |
| `LIBVER_SCHEME_ZIG` | `"zig"` | `build.zig.zon` (top-level `.version`) |
| `LIBVER_SCHEMES_ALL` | `"*"` | Every known scheme, precedence order |


## Result model

```c
typedef struct libver_scheme_result_t {
    const char* scheme;
    int         major;
    int         minor;
    int         patch;
    int         alphabeta;
    int         build;
    const char* version;
    char const* prerelease;
    char const* build_metadata;
    const char* source;
} libver_scheme_result_t;

typedef struct libver_warning_t {
    char const* kind;
    char const* scheme;
    char const* source;
    char const* message;
} libver_warning_t;

typedef struct libver_result_t {
    size_t                  num_schemes;
    libver_scheme_result_t* schemes;
    size_t                  num_warnings;
    libver_warning_t*       warnings;
} libver_result_t;
```

v0 returns **one** winner (`num_schemes == 1`) according to [detection-precedence.md](./detection-precedence.md).

`version` is the canonical string from the winning source (including
prerelease/build when present). `prerelease` and `build_metadata` are the
SemVer extras without the leading `-` / `+`; they are empty strings when
absent (never NULL).

`alphabeta` / `build` integers are a Synesis convenience **in addition to**
the strings:

* `alphabeta` is `0xFF` for a release (no prerelease); Synesis encoding
  (`0x41+` alpha, `0x81+` beta, `0xC1+` rc) when the prerelease is
  `alpha` / `beta` / `rc` with an optional `.N` or `N` (N ≥ 1); otherwise 0;
* `build` is the decimal integer of `build_metadata` when that field is
  entirely digits; otherwise 0;

Consumers that need arbitrary SemVer pre/build should use the strings, not
the ints.


## Warnings

Structured records (kind + scheme + source + human `message`), not
strings-only. Kinds:

| Macro | Value | When |
| --- | --- | --- |
| `LIBVER_WARNING_OTHER_ECOSYSTEM` | `"other-ecosystem"` | Another **selected** scheme's marker is present beside the winner |
| `LIBVER_WARNING_INCONSISTENT_SOURCES` | `"inconsistent-sources"` | Reserved: intra-ecosystem disagreement (e.g. Python). Not emitted for Cargo/Zig (one marker each) |

`libver_find(..., LIBVER_SCHEMES_ALL, ...)` on a tree with both
`Cargo.toml` and `build.zig.zon` returns Cargo as the winner and one
`other-ecosystem` warning for Zig. Filtering to a single scheme does **not**
warn about unselected ecosystems (**cargo-libver** stays silent about Zig).

Warnings never change `LIBVER_RC_SUCCESS`.


## Ownership and lifetime

* Strings in `libver_scheme_result_t` and `libver_warning_t` are owned by
  the result object;
* `libver_result_free` releases all associated storage (schemes and
  warnings);
* The API is intended to be bindable from multiple languages (FFI-friendly,
  no C++ types);


## Errors and conflicts

* directory missing / not readable;
* no recognised project type (`LIBVER_RC_NO_MATCH`);
* recognised type but version not found (`LIBVER_RC_NO_VERSION`);
* parse failure of a present marker (`LIBVER_RC_PARSE`) — does **not** fall
  through to a lower-precedence ecosystem;
* structured warnings when other **selected** ecosystems are also present;
* intra-ecosystem inconsistency is typed (`LIBVER_WARNING_INCONSISTENT_SOURCES`)
  but not emitted until a backend has multiple sources (Phase 5 Python);


## CLI mapping

The shared **libver** / **cargo-libver** CLIs map `libver_find` as follows
(see [INSTALL.md](../INSTALL.md#running-the-clis)):

* process status is the `LIBVER_RC_*` value when it is ≥ 0;
* `LIBVER_RC_INVALID` (−1) and usage errors (unrecognised flag, extra
  arguments, empty directory, `getcwd` failure) are process status **1**;
* success with warnings is still **0**; warnings go to stderr in the human
  format, or into the `warnings` array with `--json`;
* `--json` writes scheme / version / source / SemVer fields / warnings.
  Failures stay as human stderr (no JSON error object);


## Open questions

* Absolute vs project-relative `source` paths (today: `dir` joined with the
  marker name, so relative if `dir` is relative);


<!-- ########################### end of file ########################### -->
