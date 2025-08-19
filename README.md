# CustomSTL

**Version: 1.0.0-beta**

**CustomSTL** is a dual-language library project that implements core features of the C++ Standard Template Library (STL) in both **C** and **C++**.  
It provides equivalent functionality for common data structures and algorithms, making it easier to learn, compare, or integrate in hybrid C/C++ environments.

The two header-only libraries are located at the project root:
- `custom_stl_c`
- `custom_stl_cpp`

---

## CustomSTL C

<details>
<summary><b>Highlights</b></summary>

- TODO

</details>
<!-- END Highlights -->

<details>
<summary><b>Usage</b></summary>

- TODO

</details>
<!-- END Usage -->

<details>
<summary><b>C Headers</b></summary>

- `c_list`
- `c_priority_queue`
- `c_queue`
- `c_stack`
- `c_utility`
- `c_vector`

</details>

---

## CustomSTL C++

<details>
<summary><b>Highlights</b></summary>

- TODO

</details>
<!-- END Highlights -->

<details>
<summary><b>Usage</b></summary>

- TODO

</details>
<!-- END Usage -->

<details>
<summary><b>C++ Headers</b></summary>

- `algorithm`
- `array`
- `barrier`
- `bit`
- `bitset`
- `chrono`
- `complex`
- `condition_variable`
- `counting_semaphore`
- `deque`
- `forward_list`
- `functional`
- `iterator`
- `limits`
- `list`
- `map`
- `memory`
- `mutex`
- `numbers`
- `numeric`
- `pair`
- `queue`
- `ratio`
- `set`
- `shared_mutex`
- `stack`
- `string_view`
- `string`
- `thread`
- `tuple`
- `type_traits`
- `unordered_map`
- `unordered_set`
- `utility`
- `vector`

</details>

---

## Requirements

- **C Compiler**: C99 compliant  
- **C++ Compiler**: C++20 or newer  
- **Build System**: CMake (≥ 3.22.1), Ninja  
- **Testing Frameworks**:  
  - [GoogleTest](https://github.com/google/googletest) (fetched via CMake)  
  - [Unity](https://github.com/ThrowTheSwitch/Unity) (fetched via CMake)  

---

## Installation & Build

<details>
<summary><b>Build Instructions</b></summary>

```bash
# Clone the repository
git clone https://github.com/your-username/CustomSTL.git
cd CustomSTL

# Create a build directory
cmake -S . -B build -G Ninja

# Build the project
cmake --build build
```