# 🚀 Custom UNIX Shells

![Linux](https://img.shields.io/badge/Linux-OS-blue?style=flat-square) ![C](https://img.shields.io/badge/C-Language-blue?style=flat-square) ![Shell](https://img.shields.io/badge/Shell-Terminal-green?style=flat-square)

## 📌 Overview
This project is a **custom shell environment** implemented in **C on UNIX (Ubuntu)**, utilizing **Linux system calls** instead of `system()`. The shell supports **multiple specialized sub-shells** for handling standard commands, mathematical operations, logic-based operations, and string manipulations.

## 🎯 Features
✅ Fully functional command-line environment  
✅ Supports multiple specialized shells  
✅ Uses **fork()**, **exec()**, and **file handling** for execution  
✅ Saves command history for each shell  
✅ Graceful error handling and validation  
✅ Modular and structured code design  
✅ Pipes (`|`) and I/O redirection (`<`, `>`, `>>`) between arbitrary commands  
✅ Job control: background jobs (`&`), `jobs`/`fg`/`bg`, Ctrl+C/Ctrl+Z handled per-job instead of killing the shell  
✅ Cross-process command counter via POSIX shared memory + a named semaphore (`Stats` command)  

---

## 📂 Project Structure
```
📁 src
│-- 📜 Standard_shell.c    # Main shell: REPL + builtin dispatch (exit/Math/Logic/String/jobs/fg/bg/Stats)
│-- 📜 jobs.c / jobs.h     # Job control: process groups, terminal control, SIGCHLD reaping, jobs/fg/bg
│-- 📜 pipeline.c / pipeline.h  # Pipe/redirection/background execution for regular commands
│-- 📜 shell_config.h      # Shared compile-time limits (input size, max args, max pipeline stages)
│-- 📁 common/             # Code shared across binaries
│   │-- 📜 subshell_common.c/.h  # Table-driven REPL engine used by Math/Logic/String
│   │-- 📜 ipc_stats.c/.h        # Shared-memory command counter (POSIX shm + semaphore)
│-- 📁 functions/          # One executable per builtin/sub-shell command
│-- 📜 Makefile            # Build rules for all ~18 binaries
│-- 📜 Sys_shell.sh        # Thin wrapper: `make && ./Standard_shell`
│-- 📜 README.md           # Project documentation
```

---

## 🔧 Installation & Execution
### 1️⃣ Clone the repository
```sh
git clone <repository_url>
cd Custom-UNIX-Shells/src
```
### 2️⃣ Compile the project
```sh
make        # builds every binary
make run    # builds, then launches Standard_shell
make clean  # removes all built binaries and the runtime Commands/ dir
```
### 3️⃣ Start using the shell
Once executed, the shell environment is ready. You can navigate through different shells as follows:

```sh
StandShell> Math      # Switch to MathShell
MathShell> Sqrt 100   # Calculate square root
MathShell> Cls        # Return to StandShell
StandShell> exit      # Exit and delete Commands directory
```

---

## 🖥️ Available Shells & Commands
### 1️⃣ **StandardShell (`StandShell>`)**
- Execute **any valid Linux command** (up to 6 parameters)
- Pipelines: `ls | grep .c | wc -l` (up to 4 stages)
- Redirection: `sort < names.txt > sorted.txt`, `echo hi >> log.txt`
- Background jobs: `sleep 30 &` → `[1] 12345`
- Job control: `jobs`, `fg [job_id]`, `bg [job_id]` - Ctrl+C kills, Ctrl+Z stops the foreground job without killing the shell
- `Stats` - total commands executed across every shell in this session (Standard + Math + Logic + String), tracked via shared memory
- Example: `ls -l`, `mkdir testDir`, `grep -c word file.txt`
- Exit: `exit` (Deletes `Commands/` directory)

### 2️⃣ **MathShell (`MathShell>`)**
| Command       | Description |
|--------------|-------------|
| `Sqrt n`     | Compute square root of `n` |
| `Power x y`  | Compute `x^y` |
| `Solve a b c` | Solve quadratic equation `ax² + bx + c = 0` |
| `History`    | Show executed commands |
| `Cls`        | Return to `StandShell>` |

### 3️⃣ **LogicShell (`LogicShell>`)**
| Command       | Description |
|--------------|-------------|
| `Highbit n`  | Count 1 bits in binary representation |
| `DectoBin n` | Convert decimal to binary |
| `DectoHex n` | Convert decimal to hexadecimal |
| `History`    | Show executed commands |
| `Cls`        | Return to `StandShell>` |

### 4️⃣ **StringShell (`StringShell>`)**
| Command                 | Description |
|-------------------------|-------------|
| `PrintFile filename`    | Display file contents |
| `Find filename string`  | Search for a string in the file (`WIN` if found) |
| `Replace str word pos`  | Insert `word` at `pos` in `str` |
| `History`               | Show executed commands |
| `Cls`                   | Return to `StandShell>` |

---

## 📜 Example Execution
```sh
./Sys_shell.sh

StandShell> date
Sat Jun 22 21:30:26 IDT 2024

StandShell> Math
MathShell> Sqrt 100
10
MathShell> History
1. Sqrt 100
MathShell> Cls
StandShell> exit
Goodbye...
```

---

## ⚙️ System Calls Used
- `fork()`, `exec()`, `wait()`/`waitpid()`, `open()`, `read()`, `write()`
- `pipe()`, `dup2()` for command pipelines
- `signal()`/`sigaction()` for SIGINT/SIGTSTP/SIGCHLD handling
- `setpgid()`, `tcsetpgrp()`, `killpg()` for job control (process groups + terminal control)
- `shm_open()`, `mmap()`, `sem_open()` for the cross-process shared command counter
- No usage of `system()` command
- Efficient memory handling and process management

---

## 📹 Demo Video

[![Watch the demo](https://img.youtube.com/vi/WD693Mx4NYQ/0.jpg)](https://www.youtube.com/watch?v=WD693Mx4NYQ)

[watch on YouTube](https://www.youtube.com/watch?v=WD693Mx4NYQ)

---

## 🤝 Contributors
👨‍💻 **Yuval Kogan**  
📧 [LinkedIn](https://www.linkedin.com/in/yuval-kogan) | [GitHub](https://github.com/KoganTheDev)

---

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
