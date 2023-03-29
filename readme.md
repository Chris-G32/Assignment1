# Git Repository Readme

This Git repository contains the following files and folders:

- `BashScripts/`: A folder that contains Bash scripts.
- `producer.cpp`: A C++ source code file that implements a producer.
- `consumer.cpp`: A C++ source code file that implements a consumer.
- `shared.hpp`: A C++ header file that contains shared definitions used by both producer and consumer.

## BashScripts

This folder contains utility scripts that I used to quickly do repetitive tasks. This includes compiling, running the processes, doing both, and deleting the shared memory partition while debugging.

## consumer.cpp

This is a C++ source code file that implements a consumer. The consumer reads data from the shared table and outputs it to the console.

## producer.cpp

This is a C++ source code file that implements a producer. The producer generates data and writes it to the shared table.

## shared.hpp

This is a C++ header file that contains the definition of the table struct to be used by both producer and consumer.

## Getting Started

To get started with this repository, you will need to clone it to your local machine:

```bash
git clone https://github.com/Chris-G32/Assignment1
```

Once you have cloned the repository, you can compile and run the producer and consumer processes. Make sure that you have a C++ compiler installed on your machine.

g++ -o producer producer.cpp shared.hpp
g++ -o consumer consumer.cpp shared.hpp
After compiling, you can run the producer and consumer processes in separate terminal windows:

./producer
./consumer


## Contributing

If you wish to contribute to this repository, please submit a pull request. Before submitting a pull request, please make sure that your code adheres to the repository's coding standards and has been thoroughly tested.

## License

This repository is licensed under the MIT license. See the LICENSE file for more information.
