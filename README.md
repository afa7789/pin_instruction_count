![!Theme Image](resources/tool_header.png)
# Pin Tools
## Instruction Count & BBL Count
Two simple instrumentation for instruction counting made on c++ to be used with [intel pin](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html).

## Disclaimer

These instructions tools have a hashmap folder, that's a c library made with [Andrei Rimsa](http://rimsa.com.br/page/) `mostly him` :rofl: , my professor and thesis tutor from CEFET-MG. 

## How to Install

### Install Curl

We are using curl to download intel pin.

[follow this tutorial](https://help.ubidots.com/en/articles/2165289-learn-how-to-install-run-curl-on-windows-macosx-linux)

### Install Pin.

They must be downloaded [here](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html)

#### Linux

```bash
curl -O https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.19-98425-gd666b2bee-gcc-linux.tar.gz

tar zxf pin-3.19-98425-gd666b2bee-gcc-linux.tar.gz

mv pin-3.19-98425-gd666b2bee-gcc-linux pin-folder
```

#### Macos

```bash
curl -O https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.19-98425-gd666b2bee-clang-mac.tar.gz

tar zxf pin-3.19-98425-gd666b2bee-clang-mac.tar.gz

mv pin-3.19-98425-gd666b2bee-clang-mac pin-folder
```

### This tool package

```bash
git clone https://github.com/afa7789/pin_instruction_count.git tool_folder
```

## Running PIN tool

### Make first

```bash
cd tool_folder
make PIN_ROOT=/path/to/pin-folder/ obj-intel64/bblcount.so
##or
make PIN_ROOT=/path/to/pin-folder/ obj-intel64/instructioncount.so
```
### Running it

#### instructioncount.cpp

```bash
$PIN_ROOT/pin -t obj-intel64/instructioncount.so -- /bin/ls 
##or
$PIN_ROOT/pin -t obj-intel64/instructioncount.so -o output.txt -- /bin/ls 
```
#### bblcount.cpp

```bash
$PIN_ROOT/pin -t obj-intel64/bblcount.so -- /bin/ls 
##or
$PIN_ROOT/pin -t obj-intel64/bblcount.so -o output.txt -- /bin/ls 
```