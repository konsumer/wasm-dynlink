The idea here is 2 wasm modules, that both import wasi. This is based on concepts from [wamr's multi-module sample](https://github.com/bytecodealliance/wasm-micro-runtime/tree/main/samples/multi-module).

- host - Compiled with wasi-sdk. This contains helper wrappers around WASI file access. I this case, drawing utils for working with a WASI framebuffer.
- cart -  Compiled with wasi-sdk. This is the "user code" that calls things that are exposed from runtime, but also has access to same WASI, directly.
- runtime - This is the native host that runs the host + cart in wamr. The same thing is also implemented in docs/index.html for web.

```
cmake -B build
cd build
make

./null0 cart.wasm
```

This may not be totally possible with wasi-sdk. Maybe something [here](https://github.com/bytecodealliance/wasm-micro-runtime/tree/main/samples/shared-heap) would help. I got it working sharing functions, but they don't share memory, so pointers don't work. We might have to do this with emscriptne:

- [Dynamic-Linking](https://emscripten.org/docs/compiling/Dynamic-Linking.html)
- [usecase](https://helda.helsinki.fi/server/api/core/bitstreams/f535af54-70a8-4b35-a0aa-927543d4601c/content)
