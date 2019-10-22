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

Trigram Grep Finder is the application of the future that allows you to find fastly a *string* in files and manually check all found matches!

## Contents

- [Instruction of use](#-instruction-of-use)
- [Quickstart](#-quickstart)
- [Used tecnhologies](#-used-tecnhologies)
- [Idea of the solution](#-idea-of-the-solution)
- [Files description](#-files-description)
- [Benchmarks](#-benchmarks)
- [Personal impression](#-personal-impression)
- [Contributors](#-contributors)
- [License](#-license)

## 📖 Instruction of use

1. First screen is this. Click on the folder's icon in the upper left conner and select a directory!
<img src="/examples/Initial%20screen.png" width="500" title="Initial screen">

2. Now click "choose". The application will start indexing. In the future you will find *strings* in files of this directory
<img src="/examples/Select%20a%20Directory%20for%20the%20indexing.png" width="500" title="Select a Directory for the indexing">

3. You can see the progress bars and timings at the bottom of the application.
<img src="/examples/Indexing.png" width="500" title="Indexing">

4. If you want, you can stop the indexing for a while and start a searching. Just click on the button "stop".
<img src="/examples/Stop%20the%20indexing.png" width="500" title="Stop mode">

5. After the indexing is over or you've clicked "stop" you can search strings. Use the special input field in the upper part of the application
<img src="/examples/Search%20the%20string.png" width="800" title="Stop mode">

6. If you've typed the search string, at the left side you see all files with it. Choose one and click on it! At the right side - the content of the choosen file. Amazing!
<img src="/examples/See%20the%20file%20with%20the%20search%20string.png" width="800" title="See the file with the search string">

7. You can see at the upper right side of the application number of matches with the search string, number of the current selected match and navigation on matches buttons. Use the last to get the best experience!
<img src="/examples/Go%20to%20the%20next%20match.png" width="800" title="Go to the next match">

7. Also you can open the file or the path where it is just by special buttons at the bottom of the application!
<p>
<img src="/examples/Open%20the%20file.png" width="400" title="Open the file">
<img src="/examples/Open%20the%20path.png" width="400" title="Open the path">
</p>

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

- QT framework - for UI and multithreading
- C++ - as the language for the backend development
- Mutexes - for the multithreading correctness

## 🎉 Idea of the solution

Ok, that is very easy to describe:

1. Firstly, we index the selected directory by the method "trigram grep" ([described by Google's developers][trigram index]). That means we remember in `QSet<quint32> file_trigrams` all trigrams inside each file (trigram is a sequence of exactly 3 letters) (yes, we represent trigrams as `quint32`, for more details [check the code][how do we use quint32])
2. Secondly, to fastly find the search string, [we memorize trigrams of the search string][trigrams' memorizing]
3. Finally, we fastly check every file [does it contain the search trigrams][checking trigrams in files] and then manually check [does it actually contain the whole search string][exact checking]

## 📂 Files description

### Code of the application:
 - `main.cpp` - file with function `main`
 - `mainwindow.h`, `mainwindow.cpp` - the code of the main application
 - `IndexingWorker.cpp`, `IndexingWorker.h` - for the first step in our idea. Starts in a new QT-thread
 - `SearchingWorker.cpp`, `SearchingWorker.h` - for the third step. It starts in an extra thread by `mainwindow.cpp`. Starts in a new QT-thread
 - `Trigram.cpp`, `Trigram.h` - helper class for working with file lists (e.g. to remember what trigrams are there)
 
### Also:
 - `mainwindow.ui` — XML-file with the description of the main window. Utility `uic` uses it for the building of the file `ui-mainwindow.h`, which includes to `mainwindow.cpp`. `ui`-files can be opened by QT Designer or QT Creator.
 - `CMakeLists.txt` — `cmake`'s build-script.
 - `trigram_grep.pro` — `qmake`'s build-script.
 
 ## 🏄 Benchmarks
 
[Indexing by trigrams][trigram index] is as fast as reading from a hard drive, so it depends on what you have - HDD or SSD (100-500 MB/s). To solve the problem of finding string in files obviously you must at least once read all data where your string can be, so it's sensational result what our indexing function doesn't decreases at all the maximum possible speed!

On other hand, application finishes searching after 0.5-1s depending on the directory's size. It's almost the same speed as you type symbols in search string field!

Amazing, is not it?

 ## 💣 Personal impression

QT is very cool framework! It allowed to me easily create a beautigul and convenient UI with wide functionality & implemented multithreading. But "easily", because at some point I finally figured out everything! This is an incredibly exciting experience of learning new technologies 📚, googling 🔍, overcoming 🧗 framework's limitations, testing 🌶️ and immediately viewing 👀 the result 💥🔥🎆 !!!

Thanks a lot to sunny white Saint Petersburg nights 🌙, deadlines ☠️, Google 🤝 and friends 💞. You are breathtaking 💋 !!! You all helped me a lot to develop this amazing application!!!!
 
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
[trigram index]: https://swtch.com/~rsc/regexp/regexp4.html
[how do we use quint32]: https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/IndexingWorker.cpp#L93
[trigrams' memorizing]: https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/SearchingWorker.cpp#L75
[checking trigrams in files]: https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/SearchingWorker.cpp#L96
[exact checking]: https://github.com/turing228/trigram_grep/blob/ccdceabcc5352e7979c014e28272ce7a847a6cd9/back/SearchingWorker.cpp#L115
