# Py3YAEL

This is a modified Yael implementation for Python 3.x based on the original implementation. Just for academic exchange only.

## Description

Yael is a library implementing computationally intensive functions used in large scale image retrieval, such as neighbor search, clustering and inverted files. The library offers interfaces for C, Python and Matlab. Here, we only focus on C and Python interfaces.   

- *Py2_yael_v438*: original version 4.38 which supports python 2.x
- *py3_yael_v438*: modified version 4.38 which support python 3.x
- *py3yael*: an example compilation based on *py3_yael_v438*

### Environment

- Yael_v438 (released in 2014-10-29). 

  Yael main page: http://yael.gforge.inria.fr/index.html

  Download: https://gforge.inria.fr/frs/?group_id=2151

- Ubuntu 18.04 x86_64 with GCC 7.5.0

- Python 2.7 / python 3.6

### Major Changes

To make the yael library work for python 3.x, some changes should be made: 

- In `common.swg`:

  - Change `PyString_FromStringAndSize` (python 2.x) to `PyUnicode_FromStringAndSize` (python 3.x);

  - For `PyFile_Check()` (undefined), replace it by`#define PyFile_Check(op) PyObject_TypeCheck(op, &PyType_Type)`; for `PyFileObject` (undefined), use [py3c](https://github.com/encukou/py3c) function `py3c_PyFile_AsFileWithMode` as a replacement. Specifically, creat a `helper.h` in `yael_v438/yael` folder, with

    ```
    //! helper.h
    #ifndef HELPER_H_INCLUDED
    #define HELPER_H_INCLUDED
    
    #ifdef __cplusplus
    extern "C" {
    #endif
    
    #include <Python.h>
    #include "py3c.h"
    #include "py3c/fileshim.h"
    
    #define PyFile_Check(op) PyObject_TypeCheck(op, &PyType_Type)
    #define PyFile_AsFile(in) py3c_PyFile_AsFileWithMode(in, "r")
    
    #ifdef __cplusplus
    }
    #endif
    #endif
    ```

    Here, `PyType_Type` is defined in `object.h` in python 3.x, defined as `PyAPI_DATA(PyTypeObject) PyType_Type`.

    Download [py3c](https://github.com/encukou/py3c), copy its source files (in `py3c/include` folder), i.e. `py3c.h` and `py3c` folder, to `yael_v438/yael` folder. In `fileshim.h`, change `#include <py3c/compat.h>` to `#include "compat.h"`.

    Then in `common.swg`, before `PyFile_check()` (line 24), add

    ```
    %{
      #include "helper.h"
    %}
    %include "helper.h"
    ```

    Change `$1=((PyFileObject*)$input)->f_fp;` (line 36) to `$1=PyFile_AsFile($input);`

- In `ynumpy.py`, use `from . import yael`, and,

  ```
  # revise
  def _numpy_to_gmm((w, mu, sigma)):
  # to
  def _numpy_to_gmm(gmm_npy):
      (w, mu, sigma) = gmm_npy
  ```


- Modify the `.py` files in `test`, `test/py`, `progs`, `yael` folders to python3.x format.
  - `print()`;
  - In `test_ynumpy.py` and `threads.py`, modify `except Exception, e` to `except Exception as e`. 
  - In `test_kmeans.py` and `test_nn.py`, change `xrange` to `range`.
  - In `threads.py`, change `def f(self,(i,x))` to `def f(self, v):(i,x)=v`.
  - In `threads.py`, the import module "`thread`" is renamed to "`_thread`" in python3, so change to `_thread` and modify the corresponding calling functions.
  - In `learn_gmm.py`, modify `args = sys.argv[:1]` to `args = sys.argv[1:]`.  
  
- In `gmm.c`, in line 513 (iteration end condition), chang `if (key == old_key)` to `if ((long int)(key*1e4) == (long int)(old_key*1e4))`, to make it convergence quickly. 


## Installation

### Dependencies Installation
The Yael library requires gfortran, blas, lapack, swig, and python.

**1. Blas and Lapack**

```
$ sudo apt-get install gfortran
$ sudo apt-get install libblas-dev liblapack-dev # may have been installed
```

**2. Swig**

Swig is required to create the python interface.

```
$ sudo apt-get install swig
```
This method just install the swig-3.0.12, not the latest. May be not suitable for python3.x interfaces.

Thus for python3.x interfaces, you may should install the newest version. Here is swig-4.0.2, and install from source code. Swig requires g++ and pcre. G++ is often installed with Ubuntu system, if not, install by`sudo apt-get install g++`. For pcre, use pcre-8.44 (released in 2020).

```
$ cd pcre
$ wget ftp://ftp.pcre.org/pub/pcre/pcre-8.44.tar.gz
$ tar -zxvf pcre-8.44.tar.gz
$ ./configure
$ make
$ sudo make install
$ pcre-config —version # check if successful
```

Download Swig-4.0.2 from [main page](http://www.swig.org/ ), and install it base on official installation guide.

```
$ cd swig
$ wget http://prdownloads.sourceforge.net/swig/swig-4.0.2.tar.gz
$ tar -zxvf swig-4.0.2.tar.gz
$ ./configure
$ make
$ sudo make install
```
Then open `.bashrc` and add the path `PATH=$PATH:/usr/local/share/swig/4.0.2`. Then activate its path, 
```
$ source ~/.bashrc
$ swig -version  # check if successful
```

**3. Python-dev**

This package contains the include files of the Python-C API. If only python is installed, not python-dev, you will get an error saying that the file `Python.h` can not be found.

```
$ sudo apt install python-dev  # for python2.x
$ sudo apt install python3-dev  # for python3.x
```



### Compilation

First introduce the installation for C and python 2.x interfaces, and then for python 3.x. 

**1. C interface**

In yael_v438 directory,

```
$ cd yael_v438
$ sudo ./configure.sh  
$ # if CPU support sse4, use sudo ./configure.sh --msse4
```

It will generate `makefile.inc`, open it by `vim/gedit makefile.inc ` and modify the python version to your installed python version, i.e.,
```
PYTHONCFLAGS = -I/usr/include/python2.7
```

Save and then

```
$ sudo make
```

***Test***. Then test the C test code. Go to test/prog folders, run codes to test the C interfaces, e.g.,

```
$ gcc ./test/c/test_kmeans.c -I/home/***/***/yael/yael_v438 -L/home/***/***/yael/yael_v438/yael -lyael -o test_kmeans
```
It will generate executable file `test_kmeans`, then put it together with `libyael.so`, and run `./test_kmeans`.



**2. Python 2.x interface**

For Python 2.x, compile as

```
$ sudo ./configure.sh --enable-numpy
$ sudo make
```

Before `sudo make`, modify the `makefile.inc` with corrent python path, 

```
PYTHONCFLAGS = -I/usr/include/python2.7
NUMPYCFLAGS = -I/usr/lib/python2.7/dist-packages/numpy/core/include
```

For test, please see section 3.



**3. Python 3.x interface**

Originally, Yael only support python 2.x. Thus, for Python 3.x, 

before `sudo make`, first modify the `common.swg, yael.swg` file clarified in section "Major Changes", and modify `makefile.inc` with correct python path and version, including

```
PYTHONCFLAGS = -I/usr/include/python3.6m
SWIG=swig -python -py3
NUMPYCFLAGS = -I/usr/lib/python3/dist-packages/numpy/core/include
```

Then `sudo make` to compile the python 3.x interface. It will generate `yael.py` and `_yael.so` files.

***Pack Yael Pylib***

Create a `yael` folder under system python packages, e.g. `/usr/local/lib/python2.7/dist-packages/yael`. Put all the python `.py` files in the `yael_v438/yael` folder and the compiled `_yael.so` (for python call) together and copy to `**/dist-packages/yael` folders, thus you can use `from yael import yael` to call `yael` module. Basically, `yael` provide modules: `yael`, `ynumpy`, `threads`, `yutils`. Use them like `from yael import yael, ynumpy`.

***Test***

- For python 2.x, e.g. `python test_numpy.py`.
- For python 3.x, e.g. `python3 test_numpy.py`. 



1. In python 2.x test, run `test_numpy.py` with error `numpy interface not compiled in`, and run `test_ynumpy.py` with error `AttributeError: 'module' object has no attribute ‘numpy_to_fvec_ref’`.

   Solution: first `make clean` and then run compilation: `sudo ./configure.sh --enable-numpy ...`, more details can be seen in [Forums](https://gforge.inria.fr/forum/message.php?msg_id=151758&group_id=2151).

 

## Contributor





## Reference

[1] Py3c: https://github.com/encukou/py3c

