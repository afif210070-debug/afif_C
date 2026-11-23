# afif_C — 9-Stage C Compiler & IDE

A custom-built C compiler written in **C++** featuring a complete 9-stage compilation pipeline and a Visual Studio Code-inspired web interface.

## Compilation Pipeline (9 Stages)
1. **Source Code:** C code input via the VS Code-style web editor (`index.html`).
2. **Lexer (`lexer.h`):** Tokenizes raw source code into keywords, identifiers, numbers, and operators.
3. **Parser (`compiler.h`):** Validates syntax rules using Recursive Descent Parsing.
4. **Semantic Analyzer:** Checks variable declarations, redeclarations, and division-by-zero errors.
5. **IR Generation:** Generates Three-Address Intermediate Representation (IR) code.
6. **Optimization:** Performs **Constant Folding** to pre-calculate constant expressions.
7. **Bytecode Generation:** Converts optimized IR into stack-based VM instructions (`PUSH`, `LOAD`, `ADD`, `STORE`, `PRINT`).
8. **Virtual Machine:** Executes bytecode using an internal stack and memory map.
9. **Output:** Displays final execution output along with intermediate stages in real time.

## How to Run (Windows)
- **Option 1 (1-Click):** Double-click on `run.bat`.
- **Option 2 (Terminal):**
  ```bash
  g++ main.Cpp -o main.exe -lws2_32
  ./main.exe
  ```
  Then open `http://localhost:8080` in your browser.
