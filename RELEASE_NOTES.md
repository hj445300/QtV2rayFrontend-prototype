# Release v0.1.0 - QtV2rayFrontend-prototype

Minimal Qt5/C++ frontend prototype for v2ray-core.

Included:
- Qt frontend (start/stop v2ray, subscription import, config editor, realtime chart)
- Scripts to fetch and build v2ray-core (scripts/fetch_core.sh, scripts/build_core.sh)
- Top-level build_all.sh to build core + frontend and collect artifacts into ./dist
- pack.sh to create QtV2rayFrontend.zip from repository

How to produce the release ZIP locally (Ubuntu 22.04 aarch64):

1. Install dependencies:

   sudo apt update
   sudo apt install -y build-essential cmake git qtbase5-dev qttools5-dev-tools zip rsync curl unzip

2. Install Go (if building core):
   Download and install a Go binary for linux-arm64 (e.g. go1.21.x) and add /usr/local/go/bin to PATH.

3. Build everything and pack:

   chmod +x scripts/*.sh build_all.sh pack.sh
   ./build_all.sh auto
   ./pack.sh

4. Create a GitHub Release and upload QtV2rayFrontend.zip:

   # using GitHub CLI (recommended)
   gh release create v0.1.0 QtV2rayFrontend.zip --title "v0.1.0" --notes "Minimal Qt5 prototype: start/stop v2ray, subscription import, config editor, realtime chart"

   # or use the GitHub web UI: Repository -> Releases -> Draft a new release -> upload QtV2rayFrontend.zip -> Publish

Notes:
- The repository does not contain prebuilt binaries; pack.sh will include source and scripts. To include built artifacts in the release you must run build_all.sh first.
- If you want me to create the Release on GitHub and upload the zip, please provide the generated QtV2rayFrontend.zip file or authorize a secure file upload method.
