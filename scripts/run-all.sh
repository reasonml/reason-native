#!/bin/bash
for pkg in pastel-console pastel console refmterr rely; do
    esy "@${pkg}" "$@"
done
