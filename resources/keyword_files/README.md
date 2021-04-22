## WASM (WebAssembly)

Keyword files are stored in binary format. In order to use them within a JavaScript environment you need to convert them
to a `Uint8Array`. You may use any tool to read the keyword file byte-by-byte. On a Linux box you can use `xxd` utility.
For example

```console
xxd -i -g 1 resources/keyword_files/wasm/americano_wasm.ppn
```
