#!/bin/bash
set -e
for pkg in pastel-console pastel console refmterr rely; do
    "$@" "${pkg}"
done
