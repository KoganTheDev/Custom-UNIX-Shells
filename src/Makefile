# Makefile for Custom-UNIX-Shells.
#
# Every shell/tool here is intentionally its own standalone executable
# (Standard_shell forks + execs the others by relative path), so this
# Makefile builds ~18 small binaries rather than one big one. Two of them
# (Standard_shell, and each of Math/Logic/String) link a couple of shared
# translation units instead of duplicating that logic per binary - see
# common/subshell_common.c and jobs.c/pipeline.c.
#
# Usage:
#   make            build everything
#   make run        build, then start Standard_shell
#   make clean      remove all built binaries and the runtime Commands/ dir

# gnu11 (not the stricter c11) is required: this code relies on POSIX/BSD
# extensions gated behind glibc's default feature-test macros (WCONTINUED,
# killpg(), SA_RESTART, tcsetpgrp(), etc.), which strict c11 hides.
CC      := gcc
CFLAGS  := -Wall -Wextra -std=gnu11
LDFLAGS := -pthread

STANDARD_DIR := functions/Standard_Shell_functions
MATH_DIR     := functions/Math_Shell_functions
LOGIC_DIR    := functions/Logic_Shell_functions
STRING_DIR   := functions/String_Shell_functions
COMMON_DIR   := common

BINARIES := \
	Standard_shell \
	$(STANDARD_DIR)/exit $(STANDARD_DIR)/Goodbye \
	$(STANDARD_DIR)/Math $(STANDARD_DIR)/Logic $(STANDARD_DIR)/String \
	$(MATH_DIR)/Power $(MATH_DIR)/Solve $(MATH_DIR)/Sqrt $(MATH_DIR)/History \
	$(LOGIC_DIR)/DectoBin $(LOGIC_DIR)/DectoHex $(LOGIC_DIR)/Highbit $(LOGIC_DIR)/History \
	$(STRING_DIR)/Find $(STRING_DIR)/PrintFile $(STRING_DIR)/Replace $(STRING_DIR)/History

.PHONY: all clean run

all: $(BINARIES)

# --- Standard_shell: main dispatch + job control + pipeline modules ---
Standard_shell: Standard_shell.c jobs.c pipeline.c $(COMMON_DIR)/ipc_stats.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# --- exit / Goodbye: standalone, no shared modules ---
$(STANDARD_DIR)/exit: $(STANDARD_DIR)/exit.c
	$(CC) $(CFLAGS) $< -o $@

$(STANDARD_DIR)/Goodbye: $(STANDARD_DIR)/Goodbye.c
	$(CC) $(CFLAGS) $< -o $@

# --- Math/Logic/String sub-shells: share the table-driven REPL engine ---
$(STANDARD_DIR)/Math: $(STANDARD_DIR)/Math.c $(COMMON_DIR)/subshell_common.c $(COMMON_DIR)/ipc_stats.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(STANDARD_DIR)/Logic: $(STANDARD_DIR)/Logic.c $(COMMON_DIR)/subshell_common.c $(COMMON_DIR)/ipc_stats.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(STANDARD_DIR)/String: $(STANDARD_DIR)/String.c $(COMMON_DIR)/subshell_common.c $(COMMON_DIR)/ipc_stats.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# --- Math_Shell_functions: leaf binaries; the ones doing real math need libm ---
$(MATH_DIR)/Power: $(MATH_DIR)/Power.c
	$(CC) $(CFLAGS) $< -o $@ -lm

$(MATH_DIR)/Solve: $(MATH_DIR)/Solve.c
	$(CC) $(CFLAGS) $< -o $@ -lm

$(MATH_DIR)/Sqrt: $(MATH_DIR)/Sqrt.c
	$(CC) $(CFLAGS) $< -o $@ -lm

$(MATH_DIR)/History: $(MATH_DIR)/History.c
	$(CC) $(CFLAGS) $< -o $@

# --- Logic_Shell_functions: leaf binaries, pure integer arithmetic ---
$(LOGIC_DIR)/DectoBin: $(LOGIC_DIR)/DectoBin.c
	$(CC) $(CFLAGS) $< -o $@

$(LOGIC_DIR)/DectoHex: $(LOGIC_DIR)/DectoHex.c
	$(CC) $(CFLAGS) $< -o $@

$(LOGIC_DIR)/Highbit: $(LOGIC_DIR)/Highbit.c
	$(CC) $(CFLAGS) $< -o $@

$(LOGIC_DIR)/History: $(LOGIC_DIR)/History.c
	$(CC) $(CFLAGS) $< -o $@

# --- String_Shell_functions: leaf binaries ---
$(STRING_DIR)/Find: $(STRING_DIR)/Find.c
	$(CC) $(CFLAGS) $< -o $@

$(STRING_DIR)/PrintFile: $(STRING_DIR)/PrintFile.c
	$(CC) $(CFLAGS) $< -o $@

$(STRING_DIR)/Replace: $(STRING_DIR)/Replace.c
	$(CC) $(CFLAGS) $< -o $@

$(STRING_DIR)/History: $(STRING_DIR)/History.c
	$(CC) $(CFLAGS) $< -o $@

run: all
	./Standard_shell

clean:
	rm -f $(BINARIES)
	rm -rf Commands
