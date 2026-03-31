# CECS

CECS is an Entity Component System (ECS) library in C, specifically designed for my game/game engine https://github.com/opickford/csrge.

[![Windows Release CI](https://github.com/opickford/cecs/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/opickford/cecs/actions/workflows/ci.yml)
TODO: licence etc

## Contents
- [Overview](#overview)
- [Requirements](#requirements)
- [Dependencies](#dependencies)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Running tests](#running-tests)
- [Versioning](#versioning)
- [License](#license)


## Overview

### What is an ECS?

An [ECS](https://en.wikipedia.org/wiki/Entity_component_system) is an architectural pattern where a world consists of entities and an entity is a combination of components. Systems act on sets of components, to perform some function. 

An ECS is a great example of [Data Oriented Design](https://en.wikipedia.org/wiki/Data-oriented_design), where a structure of arrays (SOA) approach is taken to optimise CPU cache usage. For example, by storing each component type in its own array, a system can iterate over that array, meaning no unnecessary data is pulled into the cache, leading to fewer cache misses and overall better performance. On the other hand, a typical Object Oriented approach would use an array of structures, so if some system wanted to iterate over every object in the world, each object's entire data would have to be fetched.

Beyond performance benefits, using an ECS can also benefit development by utilising composition over inheritance. So modifying an entity's behaviour is as simple as changing its components.

### Implementation

There are multiple ways to implement an ECS, CECS uses an archetype approach, where an archetype refers to a specific combination of components. 

Archetypes can be visualised as database tables, where rows are grouped by their component signature, with all matching entities living in the same table.

| EntityId | Position  | Velocity |
|----------|-----------|----------|
| 0        | (0,0)     | (0,0)    |
| 1        | (100,100) | (5,0)    |
| 2        | (67, 0)   | (-2,1)   |

| EntityId | Position  | Health   |
|----------|-----------|----------|
| 3        | (0,0)     | 0 	      |
| 4        | (50,-50)  | 50       |

Components are stored contiguously in columns and entities are row IDs. This highlights the cache friendliness of the architecture, e.g. if a system queried Position and Velocity components, it receives contiguous arrays of exactly those components, without fetching unnecessary data. Adding or removing an entity's components moves the entity to the archetype matching its new signature. 

To facilitate writing systems, CECS provides views. A view holds a list of archetypes matching a given signature, and acts as a single iterator over all matching entities. See the examples directory for more.

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

To run the tests locally, simply build with tests enabled and then run via ctest.

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

