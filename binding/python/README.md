# Prerequisites

Python 2.7 or higher is required to use the binding and run its accompanying unit tests. If using a Python version prior
to 3.4 install [enum34](https://pypi.python.org/pypi/enum34) using

```bash
pip install enum34
```

The unit test uses [PySoundFile](https://pypi.python.org/pypi/PySoundFile) for reading audio test files. It can be
installed using

```bash
pip install pysoundfile
```

# Running Unit Tests

Using command line (from the root of the repository)

```bash
python binding/python/test_porcupine.py
```

# Binding Class

Porcupine's Python binding uses [ctypes](https://docs.python.org/3.5/library/ctypes.html) to access Porcupine's C
library. For an example usage refer to [Python demo application](/demo/python/porcupine_demo.py).  