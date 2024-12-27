# Readme File for Compilation Guidelines

## Creating Static Library

```bash
gcc -c staticlib.c -o staticlib.o
ar rcs libstaticlib.a staticlib.o
```

> **ar** - This is the archiver tool, which is used to create, modify, and extract files from static libraries. A static library is essentially a collection of object files bundled into a single file, typically with the `.a` extension in Linux/Unix.  
> - **r** - The `r` option tells `ar` to replace or insert the object files into the archive. If the file already exists in the archive, it will be replaced; otherwise, it will be added.  
> - **c** - The `c` option instructs `ar` to create the archive if it doesn't exist. Without this option, `ar` would fail if the specified archive file doesn't already exist.  
> - **s** - The `s` option ensures that a symbol table is added or updated in the archive. The symbol table allows the linker to efficiently look up symbols (e.g., function names) when linking against the static library.  
> - **libstaticlib.a** - This is the name of the static library being created. The `.a` extension conventionally denotes static libraries in Unix-like systems.  
> - **staticlib.o** - This is the object file to be added to the static library. The object file is created from compiling `staticlib.c` with `gcc -c`.

### Why Use Static Libraries?

- **Convenience:** A static library consolidates multiple object files into one, making it easier to manage and distribute them.  
- **Efficiency:** The linker extracts only the required parts of the library to include in the final executable.  
- **Portability:** The library's code becomes part of the executable, meaning no external dependencies at runtime.  

---

## Creating Dynamic Library

```bash
gcc -fPIC -c dynamiclib.c -o dynamiclib.o
gcc -shared -o libdynamiclib.so dynamiclib.o
```

> **-fPIC** - Stands for Position-Independent Code.  
> - This flag tells the compiler to generate code that can be relocated in memory.  
> - Position-independent code is a requirement for creating shared libraries because they may be loaded at different memory addresses in different programs.  
> - Without this flag, the resulting object file may not work correctly when linked into a shared library.  

- The information about `-fPIC` provided above is sufficient for working with dynamic libraries.  
- In summary, `-fPIC` is essential for creating shared libraries because it allows the code to be position-independent, enabling multiple processes to use the same library simultaneously without conflicts.  
- It's an intriguing topic—one can explore it further after gaining a solid understanding of the linking process! 😊  

> **gcc** - The GNU Compiler Collection, used for compiling, linking, and building programs and libraries.  
> **-shared** - This flag tells GCC to create a shared library instead of a regular executable.  
> - A shared library is a collection of code and data that multiple programs can load and use at runtime.  
> - On Linux systems, shared libraries typically have a `.so` extension (short for "shared object").  
> **libdynamiclib.so** - Specifies the name of the output file.  
> - The convention for shared libraries in Linux is to use the prefix `lib` followed by the library name (`dynamiclib` in this case) and the `.so` extension.  
> **dynamiclib.o** - The object file to be included in the shared library.  
> - This file must be compiled with position-independent code (`-fPIC`) to allow the shared library to be loaded at different memory addresses by various processes.  

---

## Compile Main Application

```bash
gcc main.c -o main -L/path/to/lib/folder -lstaticlib -ldynamiclib
# To avoid providing dynamic libraries path at runtime
gcc main.c -o main -L/path/to/lib/folder -lstaticlib -ldynamiclib -Wl,-rpath,/path/to/dynamiclib/folder/at/runtime
```

---

## Export `LD_LIBRARY_PATH` and Run the Main Application

```bash
export LD_LIBRARY_PATH="/path/to/dynamiclib.so/folder":$LD_LIBRARY_PATH
./main
```
