# multi-thread-html-parse
A multithreaded C++ program which accepts a list of URLs and prints the URL, total number of nodes, number of leaf nodes, and number of <div> nodes.

Should build on both Linux and Windows

### Libraries used:

[C++ Requests: Curl for People](https://github.com/whoshuu/cpr) -  HTTP Requests

[HTML Tidy](https://github.com/htacg/tidy-html5/) - HTML Tree Navigation

### Requirements:

-cmake
-openssl (for https requests)

### Building:
-cmake -S . -B build
-cmake --build build

### Running:
run from the command line, first argument is a path to a text file with a list of URLs (one per line), second argument is the number of threads to use.
