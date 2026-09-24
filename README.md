Qt V2Ray Frontend (Extended) - Minimal Prototype
===============================================

Features
- Start/Stop v2ray via QProcess, show stdout/stderr logs.
- System tray with quick actions.
- Subscription import (basic vmess:// base64 parsing and plain node lines).
- Config editor with JSON validation and Save As.
- Lightweight realtime chart that visualizes numeric samples parsed from v2ray logs.
- Packaging script pack.sh to create QtV2rayFrontend.zip.

Build (Ubuntu 22.04 aarch64 / DGX Spark)
1. Install deps:
   sudo apt update
   sudo apt install -y build-essential cmake git qtbase5-dev qttools5-dev-tools

2. Build:
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)

3. Run:
   ./QtV2rayFrontend

Packaging
- After building, run at project root:
  ./pack.sh

Notes
- Subscription parser is basic; production should add full ss/vmess/vless parsing & validation.
- Chart parses numbers heuristically from log lines (in/out/bytes or first multi-digit integer).
- GUI requires graphical environment. For headless devices use X forwarding / Xvfb.
