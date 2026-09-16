# **libver** C-API <!-- omit in toc -->

Status: Phase 2 pragmatic contract — matches **libver/libver.h**. Warning / conflict records remain for Phase 4.


## Table of Contents <!-- omit in toc -->

- [Entry points](#entry-points)
- [Status codes](#status-codes)
- [Schemes](#schemes)
- [Result model](#result-model)
- [Ownership and lifetime](#ownership-and-lifetime)
- [Errors and conflicts](#errors-and-conflicts)
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

* `dir` is a directory path; NULL or empty is a precondition violation (not "use cwd");
* `flags` is reserved; pass 0;
* `schemes` is `LIBVER_SCHEMES_ALL` (`"*"`) or a single scheme name;
* On `LIBVER_RC_SUCCESS`, `result` is populated and the caller must call `libver_result_free`;
* On failure, `result` is empty (`num_schemes == 0`, `schemes == NULL`);


## Status codes

Phase 2 integer codes (subject to Phase 4 review):

| Code | Value | Meaning |
| --- | ---: | --- |
| `LIBVER_RC_SUCCESS` | 0 | A version was found |
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
    const char* scheme;     /* e.g. "cargo", "zig" */
    int         major;
    int         minor;
    int         patch;
    int         alphabeta;  /* 0 in Phase 2 */
    int         build;      /* 0 in Phase 2 */
    const char* version;    /* canonical string from the winning source */
    const char* source;     /* winning file path as joined from dir */
} libver_scheme_result_t;

typedef struct libver_result_t {
    size_t                  num_schemes;
    libver_scheme_result_t* schemes;
} libver_result_t;
```

v0 returns **one** winner (`num_schemes == 1`) according to [detection-precedence.md](./detection-precedence.md). `alphabeta` / `build` integers are unused (0); the full string including prerelease/build metadata is in `version`.


## Ownership and lifetime

* Strings in `libver_scheme_result_t` are owned by the result object;
* `libver_result_free` releases all associated storage;
* The API is intended to be bindable from multiple languages (FFI-friendly, no C++ types);


## Errors and conflicts

Covered in Phase 2:

* directory missing / not readable;
* no recognised project type (`LIBVER_RC_NO_MATCH`);
* recognised type but version not found (`LIBVER_RC_NO_VERSION`);
* parse failure of a present marker (`LIBVER_RC_PARSE`) — does **not** fall through to a lower-precedence ecosystem;

Deferred to Phase 4:

* structured warnings when other ecosystems are also present;
* inconsistent version sources within one ecosystem;


## Open questions

* Whether `LIBVER_RC_*` stays an integer table or becomes a dedicated enum type;
* Absolute vs project-relative `source` paths (Phase 2 joins `dir` + marker name);
* Machine-readable JSON shape for the shared CLI;
* Whether warnings are strings, structured records, or both;


<!-- ########################### end of file ########################### -->
