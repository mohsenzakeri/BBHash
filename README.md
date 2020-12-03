# BBHash

This project implements a minimal perfect hash function using the BBHash method introduced in the paper "Fast and scalable minimal perfect hashing for massive key sets" (https://drops.dagstuhl.de/opus/volltexte/2017/7619/pdf/LIPIcs-SEA-2017-25.pdf). The xxhash library (https://github.com/Cyan4973/xxHash) is also used in this implementation as the core hash function of the bbhash approach.

## Instructions to install the project
```
> git clone https://github.com/mohsenzakeri/BBHash.git
> cd BBHash
> mkdir build
> cd build
> cmake ../
> make
```

## Instructions to run the benchmark
```
> ./benchmark
```
