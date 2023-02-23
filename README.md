# Nifty Language
Nifty is a new, work in progress, programming language.

```rs
using "nsl.fmt"

fn main() {
    println("Hello world!")
}
```

See nifty-spec.nifty for information on the language.  
Install the nifty-lang VSCode extension in the highlighting folder to make it easier to read.

# Directory structure:

<pre>
* bench   - Benchmark code to test the speed of compiled nifty code
* example - Example code to get an idea of how the language works
* inc     - 3rd party includes
* nsl     - The Nifty Standard Library (nsl) code
* src     - The nifty compiler code
* tst     - Test code to verify the compiler
</pre>


# Building

## macOS

Make sure XCode, the command line tools, and CMake are installed.  
LLVM can be installed with homebrew via  
`brew install llvm@15`

If you have an ARM based mac this will install the intel version and the build will fail.  
I've found the best way to fix this is to download the official mac ARM release and copy and paste in the downloaded lib folder. This should fix the build and allow the installed tools to still run.  
If you know how to install the ARM version of llvm with homebrew please let me know. (Using arch -arm64 doesn't work.)

Finally, to build nifty run  
`sh build.sh`

If you are using CLion on an ARM based mac then you need to set `-DARCH=arm64` in Settings -> Build, Execution, Deployment -> CMake -> CMake options.

Currently only macOS is supported but windows, linux, and bsd will be supported as well.

## Windows
TODO

## Linux
TODO

## BSD
TODO
