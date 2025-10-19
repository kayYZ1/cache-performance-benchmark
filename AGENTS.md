# AGENTS.md - CachingPerf Project

## Build Commands
- Build project: `make` (creates 'program' executable)
- Clean build artifacts: `make clean`
- No linting or testing framework configured

## Code Style Guidelines

### Language & Standards
- Use C++11 standard
- Compiler: clang++

### Compilation Flags
- Warnings: `-Wall -Wextra` (enable all warnings)
- Optimization: `-O2` (performance optimization)
- Debugging: `-g` (include debug symbols)

### Naming Conventions
- Files: lowercase with .cpp extension
- Functions: camelCase (e.g., `main()`)
- Variables: camelCase

### Imports
- Use angle brackets for standard library: `#include <iostream>`
- One include per line

### Formatting
- 4-space indentation
- Opening braces on same line as function/class declarations
- Consistent spacing around operators

### Error Handling
- Return appropriate exit codes from main()
- Use standard library error reporting (std::cout for messages)

### Project Structure
- Single main.cpp file in root
- Build system: GNU Make
- Target executable: 'program'