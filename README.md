# Использование в проекте

- Подключить QCrashHandler в качестве модуля

Для этого создать в корне проекта файл .gitmodules (в той же директории, что и .gitignore проекта)
```bash
git submodule init
```
И добавить через систему git модуль, указав ссылку на репозиторий и путь для загрузки модуля `YOURPATH`/QCrashHandler
```bash
git submodule add https://github.com/inelsy/QCrashHandler.git YOURPATH/QCrashHandler
```

При успешном добавлении модуля, в .gitmodules будет записано:
```txt
[submodule "YOURPATH/QCrashHandler"]
	path = YOURPATH/QCrashHandler
	url = https://github.com/inelsy/QCrashHandler.git
```

Загрузить модули, вложенные в QCrashhandler 
```bash
git submodule update --init --remote --rebase --recursive
```

-  Включить `qdumper.pri` в ваш **\*.pro** файл проекта
```cmake
include(QCrashHandler/qdumper.pri)
```

- Включить заголовочный файл `qdumper.h` в точку входу вашего проекта (файл **main.cpp**)
```c++
#include "QCrashHandler/qdumper.h"
```

- Инициализировать класс ***одним*** из *четырех* способов <br> 
сразу после инициализации приложения
```c++
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
```
> 1. Использовать значения по умолчанию: <br>
>`pathToDumpsFolder` = "" (пустой путь заменяется на первое значение [QStandardPaths::AppLocalDataLocation](https://doc.qt.io/qt-6/qstandardpaths.html#StandardLocation-enum)) <br>
>`deleteDumpAfterUploadBoolFlag` = true (true - удалять, false - не удалять)<br>
>`parent` = nullptr

```c++
    QDumper Dumper = QDumper(); 
```
>  2. Указать `путь` для сохранения дампов:
```c++
    QDumper Dumper("pathToDumpsFolder");
```
> 3. Указать `путь` для сохранения дампов и `флаг` для удаления дампов после отправки на сервер:  

```c++
    QDumper Dumper("pathToDumpsFolder", deleteDumpAfterUploadBoolFlag); 
    //QDumper Dumper("", deleteDumpAfterUploadBoolFlag);
    //QDumper Dumper(nullptr, deleteDumpAfterUploadBoolFlag);
```
> 4. Указать `путь` для сохранения дампов, `флаг` для удаления дампов после отправки на сервер и `ссылку` на родительский элемент
```c++
    QDumper Dumper("pathToDumpsFolder", deleteDumpAfterUploadBoolFlag, parent);
```

```c++
    // ... остальной ваш код

    return a.exec();
}
```
# Генерация и чтение дампов 

## Сборка проекта PROJECT на mingw 

Для чтения дампов, нужно создать из бинарного файла вашего проекта `PROJECT.exe` соответствующий файл с отладочными символами `*.pdb`, а в данном случае, чтобы оставить только информацию для трассировки стека, создать карту символов `*.sym` (смещение байтов в оперативной памяти для каждой функции в коде) **именно карту символов `*.sym`** читает по специальной директории программа minidump_stackwalk и выводит удобочитаемый стек вызовов, который привел к падению приложения. <br>

### Собирать проект нужно в режиме `debug` или в любом режиме (debug|release), только с дополнительным параметром сборки проекта `"CONFIG+=deploy"`, который собирает проект, как debug-сборку


1. <a name="important_step"></a> Создать из вашего PROJECT.exe > PROJECT.pdb - файл символов (используйте [cv2pdb](https://github.com/rainers/cv2pdb), находится в QCrashHandler: `src/cv2pdb.exe`) **только для windows**
    ```bash
    PROJECT/YOURPATH/QCrashHandler/src/cv2pdb.exe PROJECT/debug/PROJECT.exe 
    ```
    ### ***Именно после этого шага следует запускать приложение и тестировать падения, так как дампы будут генерироваться с информацией об этом разделении!***
1. Создать PROJECT.sym - карта символов (используйте dump_syms от [breakpad](https://github.com/google/breakpad), находится в QCrashHandler: `deps/breakpad.git/src/tools/windows/binaries/dump_syms.exe`)
    ```bash
    PROJECT/YOURPATH/QCrashHandler/deps/breakpad.git/src/tools/windows/binaries/dump_syms.exe PROJECT/debug/PROJECT.exe > PROJECT/debug/PROJECT.sym
    ```
    `PROJECT/debug/` - стандартный путь debug-сборки проекта **без теневой сборки**
    </br>
    
1. Получить номер сборки (нужен для следующего шага).
    
    ```bash
    head -n 1 PROJECT/debug/PROJECT.sym | awk '{print $4}'
    ```

    Первая строка файла `PROJECT.sym` содержит: 
    > MODULE windows x86_64 63D1AC58C4A2CBA81511319C65247FE90 PROJECT.pdb
    
    *всегда в конце должен быть указан файл с отладочной информацией, то есть файл символов PROJECT.pdb для windows или PROJECT для linux сборки*

    *63D1AC58C4A2CBA81511319C65247FE90 - это случайный номер сборки `BUILD_ID` вашего проекта PROJECT*

1. Создать специальную директорию (нужна для чтения дампов)
    ```bash
    mkdir -p PROJECT/PROJECT.pdb/BUILD_ID/
    ```
1. Поместить PROJECT.sym в созданную ранее специальную директорию
    ```bash
    mv PROJECT/debug/PROJECT.sym PROJECT/PROJECT.pdb/BUILD_ID/
    ```

Для чтения дампов нужно указать папку **перед** папкой PROJECT.pdb, то есть `PROJECT/`

## Чтение дампов (используйте minidump_stackwalk от [breakpad](https://github.com/google/breakpad) или minidump-stackwalk от [mozilla](https://github.com/rust-minidump/rust-minidump), в QCrashHandler **отсутствует**)

Чтение происходит при наличии сгененрированном  дампа `hash.dmp`, где вместо `hash` - хеш дампа. *Дамп должен быть создан из разделенного на [шаге 1 сборки](#important_step) бинарного файл!*

### Рекомендуется скачать release - версию `minidump-stackwalk` от [mozilla](https://github.com/rust-minidump/rust-minidump/releases/) и положить её в папку проекта PROJECT/

Для удобства чтения, рекомендуется поместить файл дампа в папку с проектом PROJECT/

```bash
PROJECT/minidump-stackwalk.exe PROJECT/hash.dmp PROJECT/ [flags] > result.txt
```
[flags] – полезные флаги **(для minidump-stackwalk от mozilla)**:
- `-brief` удалить информацию о файлах зависимостей (останется только информация о системе и стэк вызовов)
- `-json` сохранить результат в формате json 

</br>

> QDumper основан на QCrashHandler [(README)](./src/qcrashhandler/README.md)

> Дополнительная информация о внутренней работе breakpad в проектах Qt - [тут](https://github.com/JPNaude/dev_notes/wiki/Using-Google-Breakpad-with-Qt)