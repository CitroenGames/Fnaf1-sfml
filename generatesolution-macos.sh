#!/usr/bin/env bash
set -euo pipefail

sighmake Fnaf-SFML.buildscript -g makefile
sighmake --build . --config Release --parallel "$(sysctl -n hw.ncpu)"
