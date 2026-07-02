#!/usr/bin/env bash
# Thin wrapper kept for backward compatibility - the actual build rules live
# in ./Makefile. Prefer `make` / `make run` directly; this script just
# builds everything and launches the shell, like earlier versions did.
set -e
make
./Standard_shell
