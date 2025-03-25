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

---

## 📂 Project Structure
```
📁 CustomShellProject
│-- 📜 Standard_shell.c    # Main shell handling standard commands
│-- 📜 Math_shell.c        # Shell for mathematical operations
│-- 📜 Logic_shell.c       # Shell for logic-based operations
│-- 📜 String_shell.c      # Shell for string manipulations
│-- 📜 <other_command>.c   # Additional commands (if required)
│-- 📜 Sys_shell.sh        # Compilation and execution script
│-- 📜 README.md           # Project documentation
```

---

## 🔧 Installation & Execution
### 1️⃣ Clone the repository
```sh
git clone <repository_url>
cd CustomShellProject
```
### 2️⃣ Compile the project
```sh
chmod +x Sys_shell.sh
./Sys_shell.sh
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
- Execute **any valid Linux command** (up to 3 parameters)
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
- `fork()`, `exec()`, `wait()`, `open()`, `read()`, `write()`
- No usage of `system()` command
- Efficient memory handling and process management

---

## 🤝 Contributors
👨‍💻 **Yuval Kogan**  
📧 [LinkedIn](https://www.linkedin.com/in/yuval-kogan) | [GitHub](https://github.com/KoganTheDev)

---

## 📜 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
