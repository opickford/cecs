# CECS

An Entity Component System in C, specifically designed for my game/game engine https://github.com/opickford/csrge.

[![Windows Release CI](https://github.com/opickford/cecs/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/opickford/cecs/actions/workflows/ci.yml)
# TODO: licence etc

## Contents
- [Requirements](#requirements)
- [Dependencies](#dependencies)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Running tests](#running-tests)
- [Versioning](#versioning)
- [License](#license)

## Requirements

- C11 or later. (TODO: Do we actually need C11?)
- CMake 3.11

## Dependencies
- [chds](https://github.com/opickford/chds) - data structures library

## Getting Started

### Building from source

CHDS must be installed and findable by CMake (find_package) before building CECS.
See [CHDS installation guide](https://github.com/opickford/chds#building).


**Configure:**
```bash
cmake -S . -B build
```

**Configure with tests or examples:**
```bash
cmake -S . -B build -DCECS_BUILD_EXAMPLES=ON -DCECS_BUILD_EXAMPLES=ON
```

**Configure release:**
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

**Build:**
```bash
cmake --build build
```

**Install:**
```bash
cmake --install build --prefix install
```

### CMake FetchContent

Declare `chds` before `cecs` so CMake can resolve the dependency automatically:
```cmake
include(FetchContent)

FetchContent_Declare(
    chds
    GIT_REPOSITORY https://github.com/opickford/chds.git
    GIT_TAG        v0.1.0
)

FetchContent_Declare(
    cecs
    GIT_REPOSITORY https://github.com/opickford/cecs.git
    GIT_TAG        v0.1.0
)

FetchContent_MakeAvailable(chds cecs)

target_link_libraries(your_target PRIVATE cecs)

---

## Usage

TODO

### Error handling

TODO

### Version

The current library version is available at runtime:

```c
include <cecs/version.h>

printf("cecs version: %s\n", CECS_VERSION_STRING);
```

See the [examples](examples/) directory for more.

---

## Running tests

```bash
cmake -S . -B build -DCECS_BUILD_TESTS=ON
cmake --build build --config Release
ctest -C Release --test-dir build --output-on-failure
```

---

## Integration

### CMake FetchContent

TODO

---

## Versioning

TODO

This project follows [semantic versioning](https://semver.org). See [CHANGELOG.md](CHANGELOG.md) for release history.

---

## License

TODO

MIT — see [LICENSE](LICENSE) for details.


---

