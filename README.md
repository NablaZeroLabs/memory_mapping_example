# Memory Mapping Example

This repository provides a platform-independent memory mapping. The test driver
is a simple implementation of `wc -l`.

## Compilation on Linux/macOS

```
clang++ memory_mapping.t.cpp memory_mapping.cpp -Wall -Wextra -march=native -O3 -Wpedantic -std=c++17
```

## Compilation on Windows

```
cl memory_mapping.t.cpp memory_mapping.cpp /std:c++17 /O2 /FA /EHsc /GL /Ob2 /nologo
```

## Performance

I counted the newlines in the source code of CSPICE
(https://naif.jpl.nasa.gov/naif/toolkit_C.html) using macOS:

```
# File-based
$ ./a.out cspice/src/cspice/*.c
# Map-based
$ NZL_USE_MAP=1 ./a.out cspice/src/cspice/*.c
```

Some timings (nanoseconds):

```
Using MemoryMapping
79833000
80383000
81283000
77522000

Using file-based I/O
150205000
150359000
149858000
149975000
```

The memory mapping implementation performs an order of magnitude faster than the
file-based implementation (there are ways to make the file-based implementation
go faster, but that's not the point of this example).
