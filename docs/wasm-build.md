# Building MiniCC for WebAssembly

## Prerequisites

Install the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html):

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

## Build

```bash
cd /path/to/MiniCompiler
emcmake cmake -B build-web -S . -DMINICC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
emmake make -C build-web minicc -j$(nproc)
```

This produces:
- `build-web/minicc.js` -- JavaScript glue
- `build-web/minicc.wasm` -- WebAssembly binary

## Playground

The playground at `docs/playground.html` uses these files. To test locally:

```bash
cd docs
python3 -m http.server 8000
# Open http://localhost:8000/playground.html
```

Without WASM binaries, the playground runs in demo mode with pre-computed outputs.

## CI Deployment

The `gh-pages` workflow deploys the `docs/` directory to GitHub Pages automatically on push to master.
