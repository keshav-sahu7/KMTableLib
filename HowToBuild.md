# Building KMTableLib

Open terminal and type this

```console
git clone https://github.com/keshav-sahu7/KMTableLib.git
cd KMTableLib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

On windows with MinGW

```console
git clone https://github.com/keshav-sahu7/KMTableLib.git
cd KMTableLib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles"
make
```

To install the built library and header files

```console
# inside KMTableLib/build
cmake . -DCMAKE_INSTALL_PREFIX:PATH=path/to/destination
make install
```

To use it in [TableKM](https://github.com/keshav-sahu7/TableKM)

```console
cmake . -DCMAKE_INSTALL_PREFIX:PATH=path/to/TableKM/KMTableLib
make install
```
