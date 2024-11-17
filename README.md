# Memory Slab Allocator

A high-performance, thread-safe memory allocator implemented in C, using the slab allocation technique. This allocator efficiently manages memory blocks of fixed sizes, reducing fragmentation and improving allocation speed. It uses a free list to manage memory inside slabs and supports multithreading with mutex locks.

## Table of Contents

- [Features](#features)
- [How It Works](#how-it-works)
- [Installation](#installation)
- [Usage](#usage)
- [Thread Safety](#thread-safety)
- [Functions](#functions)

## Features

- ✨ **Fixed-size block allocation**: Allocates memory in fixed-size blocks, which improves allocation efficiency.
- 🔒 **Thread-safe operations**: Uses `pthread_mutex` to ensure thread safety during memory allocation and deallocation.
- ⚡ **Fast allocation and deallocation**: Reduces fragmentation and speeds up the memory management process.
- 📊 **Memory pool management**: Manages slabs of memory efficiently and allocates objects from slabs of a fixed size.
- 🧪 **Comprehensive test suite**: Ensures the allocator behaves correctly under various conditions.
  
## How It Works
```
+------------------------+
|     Hash Table         |
+------------------------+
| Index 0                |  ---> Slab for object size 1
| Index 1                |  ---> Slab for object size 2
| Index 2                |  ---> Slab for object size 3
| ...                    |
| Index N                |  ---> Slab for object size N
+------------------------+
```
```
Slab (size: 64 bytes)
+--------------------------------------------+
| Slab Header (object_size, num_objects, etc) |
+--------------------------------------------+
| Free Block 1 ----------------------------> | ---> Block of 64 bytes
+--------------------------------------------+
| Free Block 2 ----------------------------> | ---> Block of 64 bytes
+--------------------------------------------+
| Free Block 3 ----------------------------> | ---> Block of 64 bytes
+--------------------------------------------+
| ...                                        |
+--------------------------------------------+
| NULL (end of free list)                    |
+--------------------------------------------+
```

```
+---------------------------------------------+
|                Hash Table                  |
+---------------------------------------------+
| Index 0  ---> Slab 1 (Object size 64 bytes) |  ---> Slab with blocks of 64 bytes
| Index 1  ---> Slab 2 (Object size 128 bytes)|  ---> Slab with blocks of 128 bytes
| Index 2  ---> Slab 3 (Object size 256 bytes)|  ---> Slab with blocks of 256 bytes
+---------------------------------------------+

Each slab has its own free list where memory blocks are managed
```

### Slab Memory Layout

The memory allocator organizes memory into slabs, which are essentially contiguous blocks of memory containing fixed-size objects. Each slab has a free list that manages the available blocks. When memory is allocated, blocks are taken from the free list, and when memory is deallocated, blocks are added back to the list.

### Slab Structure

- **Slab**: Holds multiple objects of the same size.
- **FreeBlock**: Represents a block of free memory in a slab, linked together in a free list.

### Memory Allocation Process

1. **Create Slab**: If no slab exists for the requested object size, a new slab is created.
2. **Allocate Memory**: Memory is allocated from a slab's free list. If the slab runs out of memory, a new slab is created.
3. **Deallocate Memory**: Memory is deallocated by adding the freed block back into the slab's free list.


## Before Allocation(Empty free list):

```
Hash Table:
+---------------------------------------------+
| Index 0  ---> Slab 1 (Object size 64 bytes) |  ---> Slab with blocks of 64 bytes (Free list: Block A -> Block B -> NULL)
| Index 1  ---> Slab 2 (Object size 128 bytes)|  ---> Slab with blocks of 128 bytes
| Index 2  ---> Slab 3 (Object size 256 bytes)|  ---> Slab with blocks of 256 bytes
+---------------------------------------------+

Slab 1 (64 bytes):
+--------------------------------------------+
| Block A (64 bytes) --------------------->  |
+--------------------------------------------+
| Block B (64 bytes) --------------------->  |
+--------------------------------------------+
| NULL (end of free list)                    |
+--------------------------------------------+
```
## After Allocation(Block A allocated):

```
Hash Table:
+---------------------------------------------+
| Index 0  ---> Slab 1 (Object size 64 bytes) |  ---> Slab with blocks of 64 bytes (Free list: Block B -> NULL)
| Index 1  ---> Slab 2 (Object size 128 bytes)|  ---> Slab with blocks of 128 bytes
| Index 2  ---> Slab 3 (Object size 256 bytes)|  ---> Slab with blocks of 256 bytes
+---------------------------------------------+

Slab 1 (64 bytes):
+--------------------------------------------+
| Block B (64 bytes) --------------------->  |
+--------------------------------------------+
| NULL (end of free list)                    |
+--------------------------------------------+
```

### Thread Safety

The allocator is designed to be used in multithreaded environments. It uses a global mutex (`pthread_mutex_t mutex`) to ensure thread safety during memory allocation and deallocation. This prevents race conditions when accessing the slabs.

## Installation

To use the allocator, simply clone this repository and include the `main.c` file in your project.



TO Check if everything is working
```bash
git clone https://github.com/akshayrivers/Memory-Allocator
cd src
gcc tests.c main.c -o tests
./tests
```
TO USE IN YOUR PROGRAMS:

NOTE: Make sure that you have included main.h in your program 

```bash
gcc YOUR_PROGRAM.c main.c -o YOUR_PROGRAM
./YOUR_PROGRAM
```

## Usage

The allocator provides functions to allocate and deallocate memory. Here’s how you can use it in your code:

1. **Allocate memory**:

   ```c
   size_t object_size = 64;
   void* memory_block = allocate(object_size);
   ```

2. **Deallocate memory**:

   ```c
   deallocate(object_size, memory_block);
   ```

3. **Free all slabs**:

   To free all slabs and memory, call:

   ```c
   free_all();
   ```

## Functions

### `create_slab(size_t object_size, size_t num_objects)`

Creates a new slab for the specified object size and number of objects. Allocates memory for the slab and objects.

### `allocate_from_slab(Slab* slab)`

Allocates memory from the given slab. If the slab has no available memory, it creates a new slab.

### `deallocate_to_slab(Slab* slab, void* block)`

Deallocates the memory by adding the block back to the free list of the slab.

### `destroy_slab(Slab* slab)`

Destroys a slab, frees its memory, and removes it from the tracking list.

### `get_slab_for_size(size_t object_size)`

Retrieves the slab corresponding to the given object size. If no matching slab exists, it creates a new one.

### `allocate(size_t object_size)`

Allocates memory of the given object size from the appropriate slab.

### `deallocate(size_t object_size, void* block)`

Deallocates memory of the given object size and returns it to the appropriate slab.

### `free_all()`

Frees all slabs in the allocator's tracking array.

## Thread Safety

The allocator uses mutex locks to ensure thread safety when allocating and deallocating memory. This ensures that no data races or inconsistencies occur in a multithreaded environment.

### Thread Safety Usage

- All functions that access or modify slabs are wrapped with `pthread_mutex_lock` and `pthread_mutex_unlock` to ensure exclusive access to the slabs.




