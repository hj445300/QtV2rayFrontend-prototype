#!/usr/bin/env bash
set -euo pipefail

# Fetch v2ray-core source into third_party/v2ray-core
# Usage: ./scripts/fetch_core.sh [<git-ref>]
# If no ref is provided, this will fetch the default branch (master/main) shallowly.

REF=${1:-""}
CORE_DIR=third_party/v2ray-core
REPO=https://github.com/v2fly/v2ray-core.git

echo "-> Fetching v2ray-core into ${CORE_DIR}"
rm -rf "${CORE_DIR}"
mkdir -p "${CORE_DIR}"

if [ -z "${REF}" ]; then
  git clone --depth 1 "${REPO}" "${CORE_DIR}"
else
  git clone --depth 1 --branch "${REF}" "${REPO}" "${CORE_DIR}"
fi

echo "Fetched v2ray-core into ${CORE_DIR}"
