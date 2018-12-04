#!/bin/bash
for pkg in chalk-console chalk console refmterr test-runner; do
    esy "@${pkg}" "$@"
done
