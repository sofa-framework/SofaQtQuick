This project is a plugin and an executable GUI for SOFA based on the QtQuick library. This work is based on an initial project from [Anatoscope](www.anatoscope.com). For your information, the roadmap of the project is available on a dedicated [issue](https://github.com/sofa-framework/SofaQtQuick/issues/1).

# How to build runSofa2 on MacOS


## get code, plugins, source dependencies...

### get all the source code
```
$ mkdir runsofa2
$ cd runsofa2
$ mkdir plugins
$ cd plugins
$ git clone git@github.com:SofaDefrost/SofaQtQuick.git -b macos
$ git clone git@github.com:SofaDefrost/plugin.SofaPython3.git -b fix_for_SofaQtQuick
$ cd ..
$ git clone git@github.com:SofaDefrost/sofa.git -b SofaQtQuickGUI
$ git clone git@github.com:paceholder/nodeeditor.git
```

### plugins/CMakeLists.txt

Create a CmakeLists.txt in the plugins/ directory:

```
add_subdirectory(plugin.SofaPython3)
add_subdirectory(SofaQtQuick)
```

## dependencies

### Qt5

Better take brew's version of qt5 instead of downloading it form their website

```
$ brew install qt
```
### pybind

```
$ brew install pybind11
```
### ccache (optionnal)

```
$ brew install ccache
```

### nodeeditor

nodeeditor is a third-party library necessary for some gui widgets.

```
$ cd nodeeditor
$ mkdir build
$ cd build
$ cmake ..
$ make
$ cd ../..
```

## build

```
$ mkdir build
$ cd build
$ cmake ../sofa -DSOFA_EXTERNAL_DIRECTORIES="../plugins" -DAPPLICATION_RUNSOFA2=ON -DSOFA_USE_CCACHE=ON -G "CodeBlocks - Ninja"
$ ninja
```

## have a coffee break during build :-)

## run

```
$ bin/runSofa2
```


***Hopefully, that's all folks !***
