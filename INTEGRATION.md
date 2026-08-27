# Build instructions and integration notes

This repo now includes helper scripts to fetch and build v2ray-core alongside the Qt frontend.

Files added in /scripts:
- fetch_core.sh : shallow-clone v2fly/v2ray-core into third_party/v2ray-core
- build_core.sh : build v2ray and v2ctl for selected arch (arm64/armv7/amd64)
- build_all.sh   : top-level script to fetch/build core and build the Qt frontend, collect artifacts into ./dist

How to build on Ubuntu 22.04 aarch64 (DGX Spark)
1) Install prerequisites:
   sudo apt update
   sudo apt install -y build-essential cmake git qtbase5-dev qttools5-dev-tools zip rsync curl unzip

2) Install Go (if building core from source):
   - recommended: install latest Go 1.20+/1.21 via official tarball for linux-arm64

3) Build everything (auto-detect arch):
   chmod +x scripts/*.sh build_all.sh
   ./build_all.sh auto

4) Resulting artifacts:
   ./dist/v2ray
   ./dist/v2ctl
   ./dist/QtV2rayFrontend

Notes & considerations
- This approach places v2ray-core under third_party/v2ray-core. We do not commit the core source into the repo; the scripts clone it at build time. If you want the full core source as part of the repo (monorepo), we can change the scripts to add as a submodule or commit the tree, but that will significantly increase repo size.
- Licensing: v2ray-core is MIT-licensed; the frontend is GPL-3.0. Ensure you understand combined-distribution implications if you redistribute a packaged bundle.
- For headless devices, GUI requires X/Wayland; use Xvfb for virtual display if necessary.
