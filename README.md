# Usage in project


- include qdumper.pri to your .pro file
```cmake
include(qcrashhandler/qdumper.pri)
```

- include the class at the entry point of your project
```c++
#include "qcrashhandler/qdumper.h"
```

- init class
```c++
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDumper Dumper(0); //using only default value

    /*or*/ QDumper Dumper("pathToDumpsFolder"); //set path to save Dumps

    /*or*/ QDumper Dumper("pathToDumpsFolder", deleteDumpAfterUploadBoolFlag); // add the bool flag to delete this dump after uploading 

    /*or*/ QDumper Dumper("pathToDumpsFolder", deleteDumpAfterUploadBoolFlag, parent); // add parent for delete Dumper along with it


    // ... other stuff

    return a.exec();
}
```


# Generating and reading dumps 

## Generating PROJECT for mingw 

1. Create PROJECT.pdb from your PROJECT.exe (use [cv2pdb](https://github.com/rainers/cv2pdb)) **only for windows**
    ```bash
    cv2pdb.exe PROJECT.exe 
    ```
1. Create PROJECT.sym (use dump_syms of [breakpad](https://github.com/google/breakpad))
    ```bash
    breakpad.git/src/tools/windows/binaries/dump_syms.exe PROJECT.exe > PROJECT.sym
    ```
    </br>

    on **linux** PROJECT is binary/esecutable, could be *.so file
    ```bash
    breakpad.git/src/tools/linux/binaries/dump_syms PROJECT > PROJECT.sym
    ```
    </br>

    The first line of PROJECT.sym contains: 
    
    > MODULE windows x86_64 63D1AC58C4A2CBA81511319C65247FE90 PROJECT.pdb
    
    *should always refer to a file with debugging information, i.e. PROJECT.pdb*

    *63D1AC58C4A2CBA81511319C65247FE90 is the random `BUILD_ID` of this PROJECT*
1. Create a special directory
    ```bash
    mkdir /PROJECT.pdb/BUILD_ID/PROJECT.sym
    ```

How to read dumps, you need to specify the directory in front of the PROJECT.pdb folder as the `SYMDIR` folder

## Reading dumps (use minidump_stackwalk of [breakpad](https://github.com/google/breakpad) or [mozilla](https://github.com/rust-minidump/rust-minidump))

```bash
minidump_stackwalk.exe file.dmp /path/To/SYMDIR/ [flags] > result.txt
```
[flags] – useful flags:
- `-brief` to delete unnecessary lines with files (only the stack and system information will remain)
- `-json` to save the file in json format

</br>

> QDumper based on QCrashHandler [(README)](./src/qcrashhandler/README.md)

> Additional information is available [here](https://github.com/JPNaude/dev_notes/wiki/Using-Google-Breakpad-with-Qt)