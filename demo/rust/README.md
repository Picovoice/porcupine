# Porcupine Demo for Rust

This is an example demo written in Rust that uses the [porcupine-sys](https://crates.io/crates/porcupine-sys) crate. The demo has been tested to work on Linux and macOS.

# Running the demo

To run this demo, simply enter this in your terminal:

`cargo run -- -m path/to/porcupine_params.pv -k [path/to/keyword.ppn]`

You can also use multiple keyword files by providing more `-k` options such as:

`cargo run -- -m path/to/porcupine_params.pv -k [path/to/keyword_a.ppn] -k [path/to/keyword_b.ppn] -k [path/to/keyword_c.ppn]`
