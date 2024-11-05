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