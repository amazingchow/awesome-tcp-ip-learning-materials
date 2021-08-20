The gperftools is a very handy tool made by google to profile an execution. It is well coupled with pprof.

In general, gperftools will generate profile data, and pprof is used to view/visualize the profile result.

### Install gperftools

```shell
# or you can "apt-cache search libunwind" to find what you need
sudo apt-get install libunwind-dev

cd /path/to
git clone https://github.com/gperftools/gperftools
cd gperftools
git checkout gperftools-2.7
./autogen.sh
./configure
make
sudo make install
```

### To install pprof

```shell
go get -u github.com/google/pprof
```

### Usefule References

* [gperftools heapprofile by google](https://gperftools.github.io/gperftools/heapprofile.html)
* [gperftools heap-checker by google](https://gperftools.github.io/gperftools/heap_checker.html)
* [gperftools cpuprofile by google](https://gperftools.github.io/gperftools/cpuprofile.html)
