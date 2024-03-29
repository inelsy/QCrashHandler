<<<<<<< HEAD
# Использование в проекте
=======
## Usage in project


- include qtdumper.pri to your .pro file
```cmake
include(qcrashhandler/qtdumper.pri)
```

- include the class at the entry point of your project
```c++
#include "qtdumper/qtdumper.h"
```

- init class
```c++
QtDumper Dumper();
// or QtDumper Dumper(parent);
// or QtDumper Dumper(parent, deleteDumpAfterUploadBoolFlag);
// or QtDumper Dumper(parent, deleteDumpAfterUploadBoolFlag, "pathToDumpsFolder");
```

## How to Use
>>>>>>> upstream/master


-  Включить `qdumper.pri` в ваш *.pro файл проекта
```cmake
include(qcrashhandler/qdumper.pri)
```

- Включить заголовочный файл `qdumper.h` в точку входу вашего проекта
```c++
#include "qcrashhandler/qdumper.h"
```

- Инициализировать класс (сразу после инициализации приложения)
```c++
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDumper Dumper(0); //Использовать значения по умолчанию

    /*или*/ QDumper Dumper("pathToDumpsFolder"); //установить путь для сохранения дампов

    /*или*/ QDumper Dumper("pathToDumpsFolder", deleteDumpAfterUploadBoolFlag); // еще указать флаг для удаления дампов после отправки на сервер 

    /*or*/ QDumper Dumper("pathToDumpsFolder", deleteDumpAfterUploadBoolFlag, parent); // еще указать ссылку на родительский элемент


    // ... остальной ваш код

    return a.exec();
}
```


# Генерация и чтение дампов 

## Сборка проекта PROJECT на mingw 

1. Создать из вашего PROJECT.exe > PROJECT.pdb - файл символов (используйте [cv2pdb](https://github.com/rainers/cv2pdb)) **только для windows**
    ```bash
    cv2pdb.exe PROJECT.exe 
    ```
1. Создать PROJECT.sym - карта символов (используйте dump_syms от [breakpad](https://github.com/google/breakpad))
    ```bash
    deps/breakpad.git/src/tools/windows/binaries/dump_syms.exe PROJECT.exe > PROJECT.sym
    ```
    </br>

    на **linux** PROJECT - это двоичный/исполняемый файл, может быть файлом *.so
    ```bash
    deps/breakpad.git/src/tools/linux/binaries/dump_syms PROJECT > PROJECT.sym
    ```
    </br>

    Первая строка файла PROJECT.sym содержит: 
    
    > MODULE windows x86_64 63D1AC58C4A2CBA81511319C65247FE90 PROJECT.pdb
    
    *всегда в конце должен быть указан файл с отладочной информацией, то есть файл символов PROJECT.pdb для windows или PROJECT для linux*

    *63D1AC58C4A2CBA81511319C65247FE90 - это случайный номер сборки `BUILD_ID` вашего проекта PROJECT*
1. Создать специальную директорию
    ```bash
    mkdir ./PROJECT.pdb/BUILD_ID/PROJECT.sym
    ```

Для чтения дампов нужно указать папку **перед** папкой PROJECT.pdb, дальше будем называть её `SYMDIR`

## Чтение дампов (используйте minidump_stackwalk от [breakpad](https://github.com/google/breakpad) или [mozilla](https://github.com/rust-minidump/rust-minidump))

```bash
minidump_stackwalk.exe file.dmp /path/To/SYMDIR/ [flags] > result.txt
```
[flags] – полезные флаги **(для minidump_stackwalk от mozilla)**:
- `-brief` удалить информацию о файлах зависимостей (останется только информация о системе и стэк вызовов)
- `-json` сохранить результат в формате json 

</br>

> QDumper основан на QCrashHandler [(README)](./src/qcrashhandler/README.md)

> Дополнительная информация о внутренней работе breakpad в проектах Qt - [тут](https://github.com/JPNaude/dev_notes/wiki/Using-Google-Breakpad-with-Qt)