#!/usr/bin/env bash
set -euo pipefail

OUT=QtV2rayFrontend.zip
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

rsync -a --exclude build --exclude .git . "$TMPDIR/QtV2rayFrontend"
cd "$TMPDIR"
zip -r "../$OUT" "QtV2rayFrontend"
echo "Generated $OUT in $(pwd)/.."
