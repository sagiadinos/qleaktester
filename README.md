# qleaktester

With this tool it is possible to find out if the underlying multimedia libs maybe have memory leaks.

During developing [garlic-player](https://garlic-player.com) I faced with some annoying memory leaks and valgrind not helped much first.
To isolate I wrote this little tool, which can play continouesly a little video file and measuring the used [RSS memory](https://en.wikipedia.org/wiki/Resident_set_size) .

It is neither a big thing not a replacement for valgrind, but it gives me some hints and maybe someone else can use it.

# qleaktester

QtAV should be installed

There are two multimedia libs for Qt  [QtAV](http://www.qtav.org) and [QtMultimedia](http://doc.qt.io/qt-5/qtmultimedia-index.html)
QtAV based on ffmpeg and QtMultimedia depending on operating system (Windows => DirectShow, Linux GStreamer)


# Usage

Start autoplay checkbox before one of the "Allocate-lib"-Buttons and wait what happens. 
It make no sense to run both libs, but if you like... ;)

After some repeats a maximum value should be reached. If this won't happens and allocation continue that is a strong hint for a memory leak in lib or wrapper.
