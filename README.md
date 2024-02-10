# basc64

**Notice: This compiler is still being developed, please do not use this compiler outside testing purposes until this notice is gone.**

A 64-bit compiler for my basic C-like language. This compiler does not use any external tools such as LLVM, it is written from scratch in C and compiles only to x86_64 nasm assembly.

## Setup

You will need to build the compiler from source in order to obtain an executable.

### Requirements

- C compiler
- make
- nasm

### Build from source

Simply run the makefile:

```bash
make
```

The ```basc64``` executable should appear in a directory called ```bin```.

## Usage

```bash
./bin/basc64 <filename> [-o <outname>, -h]
```

The only required argument is ```<filename>``` and the default output executable name is 'out.out', but you can change this with the ```-o``` flag.

### Flags

- ```-o <filename>``` - Set the output executable name to ```<outname>```, otherwise use 'out.out'
- ```-h``` - Show compiler usage

## License

[GPL-3.0](./LICENSE)
