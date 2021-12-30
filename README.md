![!Theme Image](resources/tool_header.png)
# Pin Tools
## Instruction counter PIN
Simple instrumentation that does a instruction counting made on c++ to be used with [intel pin](https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-dynamic-binary-instrumentation-tool.html). It uses isntructions blocks to count it faster.

## Disclaimer

The instructions tool were made with the tutoring and help of [Andrei Rimsa](http://rimsa.com.br/page/). Andrei is my professor and thesis tutor from CEFET-MG.

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
make PIN_ROOT=/path/to/pin-folder/ obj-intel64/icounter-pin..so
```
### Running it

#### icounter-pin..cpp

```bash
$PIN_ROOT/pin -t obj-intel64/icounter-pin..so -- /bin/ls 
##or
$PIN_ROOT/pin -t obj-intel64/icounter-pin..so -o output.txt -- /bin/ls 
```