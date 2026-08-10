# **libver** C-API <!-- omit in toc -->

Status: sketch for v0 — not finalised.


## Table of Contents <!-- omit in toc -->

- [Entry points](#entry-points)
- [Result model](#result-model)
- [Ownership and lifetime](#ownership-and-lifetime)
- [Errors and conflicts](#errors-and-conflicts)
- [Open questions](#open-questions)


## Entry points

```c
/** Opaque context type */
typedef struct libver_result libver_result;

/** Attempt to obtain library version
int libver_find(
    char const*     dir
,   int             flags
,   libver_result*  result /* [out] */
);
void libver_result_free(
    libver_result*  result
);
```

* `dir` may be `NULL` or `""` to mean the current working directory;
* On success, `out` is populated and the caller must eventually call `libver_result_free`;
* Exact integer status codes are TBD (see below);


## Result model

Suggested fields (names may change):

```c
typedef struct libver_result {
    const char* language;   /* e.g. "cargo", "zig", "python", ... */
    const char* version;    /* canonical version string, or NULL if unknown */
    const char* source;     /* winning file path (project-relative or absolute — TBD) */
    /* warnings / conflicts — TBD */
} libver_result;
```


## Ownership and lifetime

* Strings returned in `libver_result` are owned by the result object;
* `libver_result_free` releases all associated storage;
* The API is intended to be bindable from multiple languages (FFI-friendly, no C++ types);


## Errors and conflicts

TBD — expected cases include:

* directory missing / not readable;
* no recognised project type;
* recognised type but version not found;
* multiple ecosystems present (winner + warnings);
* inconsistent version sources within one ecosystem (e.g. Python 2 heuristics);


## Open questions

* Status code enumeration vs errno-style;
* Absolute vs project-relative `source` paths;
* Machine-readable JSON shape for the shared CLI;
* Whether warnings are strings, structured records, or both;


<!-- ########################### end of file ########################### -->
