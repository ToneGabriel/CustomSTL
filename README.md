# CustomSTL

**Version: 1.0.7-beta**

**CustomSTL** is a dual-library project that implements core features of the C++ Standard Template Library (STL) in both **C** and **C++**.  
It provides equivalent functionality for common data structures and algorithms, making it easier to learn, compare or integrate in hybrid C/C++ environments.

---

## Contents

<details>
<summary><b>Highlights</b></summary>

- Custom STL containers and algorithms have similar functionality to the ones provided by C++ STL standard library.
- Fully type-safe – Templates ensure strict type correctness, avoiding unsafe casting between container types or elements.
- Automatic memory management – Elements are automatically managed and destroyed via default or user-provided functions/macros.
- Consistent, easy-to-learn API – Container and algorithm usage follows a uniform design similar to C++ STL. Simply include the headers.
- No callback functions - Template functions and macros are executed directly, avoiding the overhead of stored callback functions.
- Well-tested – The project includes unit tests and builds the corresponding test executables.

</details>
<!-- END Highlights -->

<details>
<summary><b>Usage</b></summary>

STL C++ library - use exactly as the original `std::`, but with `custom::` namespace instead

```C++
#include "custom/vector.h"

int main()
{
    custom::vector<int> myVector(10);

    myVector.push_back();
    myVector.push_back();
    myVector.push_back();

    custom::vector<int>::iterator it = myVector.begin();
    ++it;

    return 0;
}
```

STL C library - the user must declare the desired container or functions via macros and provide a custom name prefix, desired type and other memory management functions for that type (e.g. copy, move, compare, delete)

```C
#include "custom/c_vector.h"

DEFINE_GENERIC_VECTOR(
    MyVectorINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)

int main()
{
    MyVectorINT myVector = MyVectorINT_create(10);

    int val = 0;
    MyVectorINT_push_back(&myVector, &val);
    MyVectorINT_push_back(&myVector, &val);
    MyVectorINT_push_back(&myVector, &val);

    MyVectorINT_Iterator it = MyVectorINT_begin(&myVector);
    MyVectorINT_Iterator_pre_increment(&it);

    MyVectorINT_destroy(&myVector);

    return 0;
}
```

</details>
<!-- END Usage -->

<details>
<summary><b>C Headers</b></summary>

- `c_list` - `c_vector` - `c_queue` - `c_priority_queue` - `c_stack` - `c_pair`
- `c_utility`

</details>
<!-- END C Headers -->

<details>
<summary><b>C++ Headers</b></summary>

- `array` - `bitset` - `deque` - `forward_list` - `list` - `vector` - `map` - `set` - `unordered_map` - `unordered_set` - `pair` - `tuple` - `queue` - `stack` - `string_view` - `string`
- `algorithm` - `bit` - `complex` - `numbers` - `numeric` - `iterator` - `limits` - `functional` - `memory`
- `chrono` - `ratio` - `type_traits` - `utility`
- `thread` - `condition_variable` - `counting_semaphore` - `barrier` - `mutex` - `shared_mutex`

</details>
<!-- END C++ Headers -->

---

## Requirements

- **C Compiler**: C99 compliant
- **C++ Compiler**: GNU C++ or MSVC, C++20 or newer (multithreading is disabled for MSVC)
- **Build System**: CMake (≥ 3.22.1), Ninja  
- **Testing Frameworks**:  
  - [GoogleTest](https://github.com/google/googletest) (auto fetched via CMake)  
  - [Unity](https://github.com/ThrowTheSwitch/Unity) (auto fetched via CMake)  

---

## Installation & Build

```bash
# Clone the repository
git clone https://github.com/ToneGabriel/CustomSTL.git
cd CustomSTL

# Create a build directory
cmake -G "Ninja" -B build

# Build the project
cmake --build build

# Run tests
ctest --test-dir build
```

Or simply run the script `scripts/RUN_TESTS` and the build is done automatically.   
The results can be found in `build/Testing/Temporary` folder.
