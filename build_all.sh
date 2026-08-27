#!/usr/bin/env bash
set -euo pipefail

# Top-level build script: fetch core, build core, build frontend (Qt), and collect artifacts
# Usage: ./build_all.sh [arch]

ARCH=${1:-auto}
ROOT_DIR=$(pwd)

echo "==> Build all: arch=${ARCH}"

# 1) fetch core
./scripts/fetch_core.sh || { echo "fetch_core failed"; exit 1; }

# 2) build core
./scripts/build_core.sh ${ARCH} || { echo "build_core failed"; exit 1; }

# 3) build frontend (Qt)
mkdir -p build
pushd build > /dev/null
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)
popd > /dev/null

# 4) collect artifacts
mkdir -p dist
cp -v ./bin/v2ray ./dist/ || true
cp -v ./bin/v2ctl ./dist/ || true
cp -v build/QtV2rayFrontend ./dist/ || true

echo "All built. Artifacts in ./dist"
