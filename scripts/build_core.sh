#!/usr/bin/env bash
set -euo pipefail

# Build v2ray-core and put binaries into ./bin
# Usage: ./scripts/build_core.sh [arm64|arm32]
# Default arch: auto-detect (arm64/arm/amd64)

ARCH_ARG=${1:-auto}
CORE_DIR=third_party/v2ray-core
OUT_DIR=./bin
mkdir -p "${OUT_DIR}"

if [ ! -d "${CORE_DIR}" ]; then
  echo "v2ray-core not found in ${CORE_DIR}. Run scripts/fetch_core.sh first."
  exit 1
fi

# detect arch
if [ "${ARCH_ARG}" = "auto" ]; then
  UNAME_M=$(uname -m)
  if [ "${UNAME_M}" = "aarch64" ] || [ "${UNAME_M}" = "arm64" ]; then
    TARGET_ARCH=arm64
  elif [ "${UNAME_M}" = "armv7l" ] || [ "${UNAME_M}" = "arm" ]; then
    TARGET_ARCH=armv7
  else
    TARGET_ARCH=amd64
  fi
else
  TARGET_ARCH=${ARCH_ARG}
fi

echo "-> Building v2ray-core for target: ${TARGET_ARCH}"

# prefer using local go installation
if ! command -v go >/dev/null 2>&1; then
  echo "Go not found in PATH. Please install Go (1.15+ recommended) and retry." >&2
  exit 1
fi

pushd "${CORE_DIR}" > /dev/null
# ensure modules downloaded
GO111MODULE=on go mod download

case "${TARGET_ARCH}" in
  arm64)
    CGO_ENABLED=0 GOOS=linux GOARCH=arm64 go build -trimpath -ldflags "-s -w -buildid=" -o "${OUT_DIR}/v2ray" ./main
    CGO_ENABLED=0 GOOS=linux GOARCH=arm64 go build -trimpath -tags confonly -ldflags "-s -w -buildid=" -o "${OUT_DIR}/v2ctl" ./infra/control/main
    ;;
  armv7)
    CGO_ENABLED=0 GOOS=linux GOARCH=arm GOARM=7 go build -trimpath -ldflags "-s -w -buildid=" -o "${OUT_DIR}/v2ray" ./main
    CGO_ENABLED=0 GOOS=linux GOARCH=arm GOARM=7 go build -trimpath -tags confonly -ldflags "-s -w -buildid=" -o "${OUT_DIR}/v2ctl" ./infra/control/main
    ;;
  amd64)
    CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -trimpath -ldflags "-s -w -buildid=" -o "${OUT_DIR}/v2ray" ./main
    CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -trimpath -tags confonly -ldflags "-s -w -buildid=" -o "${OUT_DIR}/v2ctl" ./infra/control/main
    ;;
  *)
    echo "Unknown target arch: ${TARGET_ARCH}" >&2
    exit 1
    ;;
esac

popd > /dev/null

chmod +x "${OUT_DIR}/v2ray" "${OUT_DIR}/v2ctl" || true

echo "Built v2ray binaries in ${OUT_DIR}/"
