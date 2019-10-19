Trigram Grep Finder
-------------------------------------------------------

<p align="center">
    <a href="https://github.com/turing228/trigram_grep/blob/master/LICENSE">
        <img src="https://img.shields.io/github/license/turing228/trigram_grep" title="Trigram Grep Finder is released under the MIT license." />
    </a>
    <a href="https://github.com/turing228/trigram_grep/graphs/contributors">
        <img src="https://img.shields.io/github/contributors/turing228/trigram_grep?color=orange" title="Contributors"/>
    </a>
    <a href="https://github.com/turing228/trigram_grep/graphs/contributors">
        <img src="https://img.shields.io/github/v/release/turing228/trigram_grep" title="Release version"/>
    </a>
    <img src="https://img.shields.io/github/repo-size/turing228/trigram_grep" title="Repository size"/>
    <img src="https://img.shields.io/badge/build-passing-brightgreen" title="Build passing"/>
    <img src="https://img.shields.io/github/stars/turing228/trigram_grep?style=social" title="Stars"/>
</p>

Trigram Grep Finderis the application of the future that allows you to find fastly a *string* in files and manually check all found matches!

## Contents

- [Instruction of use](#-instruction-of-use)
- [Quickstart](#-quickstart)
- [Used tecnhologies](#-used-tecnhologies)
- [Idea of the solution](#-idea-of-the-solution)
- [Files description](#-files-description)
- [Contributors](#-contributors)
- [License](#-license)

## 📖 Instruction of use

1. First screen is this. Click on the folder's icon in the upper left conner and select a directory!
<img src="/examples/Initial%20screen.png" width="500" title="Initial screen">

2. Now click "choose". The application will start indexing. In the future you will find *strings* in files of this directory
<img src="/examples/Select%20Directory%20for%20Indexing.png" width="500" title="Select a directory for indexing">

3. You can see the progress bars and timings at the bottom of the application.
<img src="/examples/Indexing.png" width="500" title="Indexing">

4. If you want, you can stop the indexing for a while and start a searching. Just click on the button "stop".
<img src="/examples/Stop%20indexing.png" width="800" title="Stop mode">

5. After the indexing is over or you've clicked "stop" you can search strings. You the special input field in the upper part of the application
<img src="/examples/Search.png" width="800" title="Search">

6. If you've typed the search string, at the left side you see all files with it. Choose one and click on it! At the right side - the content of the choosen file. Amazing!
<img src="/examples/Search.png" width="800" title="Search">

6. You can see at the upper right side of the application number of matches with the search string, number of the current selected match and navigation on matches buttons. Use the last to get the best experience!
<img src="/examples/Search.png" width="800" title="Search">

## 🚀 Quickstart

You have two options:

CMake-way building:

    $ cmake -DCMAKE_BUILD_TYPE=Debug .
    $ make
    
You are also free to write if you want, for example, `-DCMAKE_BUILD_TYPE=RelWithDebInfo` instead of `-DCMAKE_BUILD_TYPE=Debug`

QMake-way building:

    $ qmake CONFIG+=debug -o Makefile extra_files_finder.pro
    $ make

To run it just write in your terminal:

    $ ./trigram_grep

## 📋 Used tecnhologies

- QT framework - for UI application and multi-threading
- C++ - as the language for the backend development

## 🎉 Idea of the solution

Ok, that is very easy to describe:

1. Firstly, we index the selected directory by the method "trigram grep". That means we remember in `QSet<quint32> file_trigrams` all trigrams inside each file (trigram is a sequence of exactly 3 letters) (yes, we represent trigrams as `quint32`, for more details [check the code](https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/IndexingWorker.cpp#L93))
2. Secondly, to fastly find the search string, [we memorize trigrams of the search string](https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/SearchingWorker.cpp#L75)
3. Finally, we fastly check every file [does it contain the search trigrams](https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/SearchingWorker.cpp#L96) and then manually check [does it actually contain the whole search string](https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/SearchingWorker.cpp#L115)

## 📂 Files description

### Code of the application:
 - `main.cpp` - file with function `main`
 - `mainwindow.h`, `mainwindow.cpp` - the code of the main application
 - `clustering.cpp`, `clustering.h` - for the first step in our idea
 - `clusterWorker.cpp`, `clusterWorker.h` - for the second step. It starts in an extra thread by `mainwindow.cpp` to don't disturb UI-thread
 
### Also:
 - `mainwindow.ui` — XML-file with the description of the main window. Utility `uic` uses it for the building of the file `ui-mainwindow.h`, which includes to `mainwindow.cpp`. `ui`-files can be opened by QT Designer or QT Creator.
 - `CMakeLists.txt` — `cmake`'s build-script.
 - `extra_files_finder.pro` — `qmake`'s build-script.
 
 ## 👪 Contributors

<a href="https://github.com/sorokin" title="Github profile of Ivan Sorokin">
    <img src="https://github.com/sorokin.png" width="40" height="40">
    Ivan Sorokin, JetBrains C++ developer. C++ master developer. Author of the app's idea and the app base creator
</a>
<br/>
<a href="https://github.com/eugene536" title="Github profile of Evgeniy Nemchenko">
    <img src="https://github.com/eugene536.png" width="40" height="40">
    Evgeniy Nemchenko, VK.com developer. Code reviewer. Adviced how to decrease the code complexity and what features we can add more
</a>
<br/>
<a href="https://github.com/Igorjan94" title="Github profile of Igor Kolobov">
    <img src="https://github.com/Igorjan94.png" width="40" height="40">
    Igor Kolobov. Also a code reviewer. Adviced how to decrease the code complexity and what features we can add more
</a>
<br/>
<a href="https://github.com/turing228" title="Github profile of Nikita Lisovetin">
    <img src="https://github.com/turing228.png" width="40" height="40">
    Nikita Lisovetin, student of ITMO University, Department of Computer Technologies. Developer. Wrote the code
</a>
 
 ## 📄 License

Extra Files Finder is MIT licensed, as found in the [LICENSE][l] file.

[l]: https://github.com/turing228/trigram_grep/blob/master/LICENSE




trigram_grep — Графическое приложение QT для поиска строки в файлах
-------------------------------------------------------

### Описание файлов

Исходный код расположен в файлах
 - `main.cpp`— файл с функцией `main`
 - `mainwindow.h`, `mainwindow.cpp` — код главного окна приложения
 
 Кроме исходного кода есть
 - `mainwindow.ui` — XML-файл с описанием главного окна. В процессе сборки на его основе утилитой `uic` будет сгенерён файл `ui_mainwindow.h`, который включается в `mainwindow.cpp`. `ui`-файлы можно открыть Qt Designer'ом. Qt Creator имеет Qt Designer встроенный в себя и тоже умеет открывать `ui`-файлы.
 - `CMakeLists.txt` — билд-скрипт для `cmake`.  Не используется при билде `qmake`'ом.
 - `trigram_grep.pro` — файл-проект для `qmake`. Не используется при билде `cmake`'ом.

### Сборка `cmake`'ом

    $ cmake -DCMAKE_BUILD_TYPE=Debug .
    $ make

Вместо `-DCMAKE_BUILD_TYPE=Debug` может быть `-DCMAKE_BUILD_TYPE=RelWithDebInfo` или что-то другое в зависимости от того, какую конфигурацию вы желаете.

### Сборка qmake'ом

    $ qmake CONFIG+=debug -o Makefile trigram_grep.pro
    $ make

-------------------------------------------------------
Этот проект мой самый технически сложный и объемный в программировании, но в то же время и самый интересный.

#### Что сделал?
Я реализовал на C++ многопоточную программу с графической оболочкой QT с кучей кнопок (например, остановки поиска), выбором директорий и удобным выводом результата. Удобным, то есть с возможностью по нажатию на файл из списка увидеть тут же его содержимое, а затем, нажимая по кнопкам, переходить по совпадениям в нем.

Основано решение на докладе команды из Google, а именно - поиске по триграммам (триграмма - подстрока из 3 подряд символов). Оказывается, если в файле содержатся все триграммы искомой строки, то почти наверняка содержится и сама строка. Поэтому мы сначала молниеносно сканируем директорию, находя триграммы в файлах и запоминая какие где, а затем, когда нам поступает запрос на поиск строки, ищем по ее триграммам. Затем проверяем, что эта строка действительно есть в файле, а не просто ее триграммы.

#### Результат
##### Бенчмарки
Результаты впечатляют. Индексация происходит почти со скоростью чтения с диска и зависит от железа – HDD или SSD, то есть 100-500 мб/c. А заканчивает искать строчку через 0,5-5 секунд после начала в зависимости от размера директории, то есть почти со скоростью ввода букв в строку поиска.
##### Дополнительно
В качестве дополнительного функционала я реализовал многопоточность для независимой работы потоков графического интерфейса, индексации, поиска. Пользователь может смотреть файлы, пока происходит поиск строки. Может и остановить в любой момент выполняемый процесс по нажатию на кнопку или просто отмасштабировать окно приложения. В любом случае **графический интерфейс не лагает**. Так же я настроил обновление индексации в случае изменения файлов. 

##### 
Что из себя представляет графический интерфейс на движке QT? Сверху выбор директории, под ней поле для ввода искомой строки. В левой половине список файлов с искомой строкой, по нажатию на один из них он выводится на правой половине. По выведенному файлу можно перемещаться по совпадениям, нажимая на кнопки. Так же снизу прогресс-бар выполнения этапов работы, время их выполнения.


#### С чем стокнулся
QT обалденный фреймворк. Я в итоге легко как создал красивый и удобный графический интерфейс с широким функционалом, так и реализовал многопоточность. Но легко потому, что я в какой-то момент наконец разобрался во всем) 

Простые штуки действительно просто делать, но, если что-то хоть немного сложное и нетривиальное, то сталкиваешься с ограничениями QT (я иногда натыкался на неожиданное отсутствие нужных мемберов в классах), а в интернете часто примеры это именно что примеры - они написаны без мысли, что ты собираешься использовать их код совместно с другим кодом.

Самым сложным было понять *как правильно организовать несколько крупных потоков*, которые ты то выключаешь, то включаешь, хотя казалось бы – это лишь немного сложнее «линейных» графических приложений. 

Но знакомые, гугл, дедлайны и несколько бессонных ночей сделали свое дело) и я стал специалистом по QT фреймворку, создав отличнейшее приложение.

#### В итоге
Разбираться в новой технологии, гуглить, мучиться, обходить ограничения, тестировать и видеть тут же результат было безумно интересно. Я рад, что у меня получилось выполнить этот проект и я им горжусь.
