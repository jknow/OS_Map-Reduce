Introduction
MapReduce is a distributed framework heavily used by Google, Yahoo, and many others for processing huge data sets in a specific manner. You can read a summary of this in my 417 notes. MapReduce achieves its high scalability by breaking the data to be analyzed into chunks and allocating its processing among many workers. It works in two phases:

map
A map worker is a process that reads a portion of input data, parses it, and spits out one or more (key, value) sets of data. Many map workers will work concurrently to process all the input data. The (key, value) data is stored in temporary files.
reduce
The (key, value) results of the map workers are sorted by key and passed to one or more reduce workers. Each reduce worker calls a reduce function multiple times, once per unique key. This reduce function is called with a single key and a list of all the values that are associated with that key regardless of which map worker created them. The reduce workers write their output to a file. The collection of output from all the reduce workers comprises the final result of the program.
Objective
For this assignment, you will implement a framework for multithreaded data processing on a single computer that follows the MapReduce paradigm. While MapReduce has been designed for a large-scale distributed computing environment, you will create the framework for a single-system, multi-core environment where map and reduce workers run on the same system, use shared memory for communication, and use semaphores for synchronization.

Group Size
You may work in a group comprising up to five students. However, if you have a group of three or more students, you must implement both a multi-process as well as a multi-threaded version of the assignment and provide performance comparison testing between the two versions in your report.

Specifications
You are required to design and implement a MapReduce framework using C or C++ on Linux.
If you are working with a group of three or more, your framework must be configurable to provide parallelism in two distinct ways: (a) Using multiple Linux processes and (b) Using POSIX threads (pthreads). If you are working in a group of under 3, you can pick one method but be sure to document it. If you are using threads then you implicitly have shared memory so you need not use shared memory. If you are using processes then, of course, you need not use pthreads but will have to use shared memory to share data among the processes.
Your framework should be designed in a general manner. In particular, the problem to be solved (see item 5 below) should be encapsulated solely in the implementation of the map() and reduce() functions. Substitution of different map() and reduce() functions should enable your framework to solve a different problem without any further changes in the framework itself.

In a multiprocess implementation, all intermediate data that is to be transferred between the map and reduce phases is to be stored in POSIX shared memory. Reading and writing to this shared memory may require synchronization (mutual exclusion). You may NOT write intermediate output to the file system. There also needs to be synchronization (barrier) to explicitly separate the map and reduce phases.

You may not use an intermediate file in a multithreaded implementation either, although all memory is shared so you do not have to use operating system inter-process shared memory.

Once your framework is working, you must implement two sets of map/reduce functions to solve the following problems:

Word count: given an input file, you must count the number of times each individual word appears in the input and output a file containing a list of words followed by their counts. The output file will contain one word per line.
Integer sort: given an input file containing a list of integers, you must output a file containing the same integers in sorted, ascending order.
You will be provided with sample input and output files for each problem. Your implementation must be able to process the example input and must produce output in exactly the same format as the example output file.
Your framework must compile to a single executable file called mapred, that conforms to the following command line structure:


      mapred -a [wordcount, sort] -i [procs, threads] -m num_maps -r num_reduces infile outfile
where:

-a specifies the application: wordcount or sort
-i specifies whether processes or threads are used
-m specifies the number of map processes (or threads)
-r specifies the number of reduce processes (or threads)
infile is the name of the input file
outfile is the name of the input file
For example, you can execute this four different ways:

wordcount with processes
wordcount with threads
sort with processes
sort with threads
Any of these can be run with with an arbitrary number of maps and reduces.

Planning
Be sure to allocate plenty of time for writing small programs to test threading, shared memory, and semaphores so that you can be comfortable working with these. Also, be sure to allocate time to understand how MapReduce works and to think about the architecture of your program. If you wait to start until four days before the assignment is due, you will most definitely panic and not get it done.

Note that a map function emits one or more key, value pairs that are stored in memory until all the map workers have finished their processing. Then, a shuffle/sort function runs at each reduceworker that fetches the a subset of all key, value pairs that this specific reduce worker will process. The shuffle function sorts all the fetched data by the key. For each instance of the key, it calls a reduce function with a list of all the values that are associated with that key.

Put special effort into thinking about how you will store the intermediate data and how the map workers will read it and sort it. You may use a hash function to determine which keys go to which reduce workers. For example, hash(key) mod m may identify the reduce worker (0..m) that is responsible for a particular key.

Input preprocessing
You may partition your input data into N number of files, where N is the number of map workers. In this case, an input file myfile will be split into the files myfile.0, myfile.1, etc. You can use the Linux split command to break up your file. Alternatively, you can download and run this script to break a file into N roughly chunks with roughly the same number of lines in each chunk. The usage of the command is:


	./split.sh yourfile 25
This will break the file yourfile into 25 files, yourfile.0 ... yourfile.24, each containing roughly the same number of lines of text. To run this, download the file and make it executable via:


	chmod +x split.sh
Alternatively, you can explicitly run it from bash:


	bash ./split.sh yourfile 25
You can remove the ./ if the current directory (.) is in your search path (PATH shell variable; see the bash documentation if you don't know about this).

Make it clear in your report whether the input data needs to be preprocessed in this manner.

Output postprocessing
You may create a separate output file for each of your reduce workers. The files can be concatenated together after the program is run to create the final composite file. Again, make it clear in your report whether the output data needs generates multiple files.

Report
You must prepare a report, submitted either as pdf, that contains, at a minimum:

Names of group members.
Detailed instructions for using the program, including any preprocessing and postprocessing steps that must be taken. Make sure that you submit only the program source, headers, and makefile. A user should do nothing more than run make to compile the program. Make sure yoar instructions clearly specify the command name and usage and provide an example.
Architectural description of the framework, including where you had to employ synchronization primitives.
Tests that were run.
Description of difficulties and problems that you encountered.
If you implemented both threads and processes, an evaluation of the performance difference between the two.
Data sets
For text data, you can download one or more books in ASCII text form from Project Gutenberg.

For number data, you can write a program that uses a random number generator and generates N lines of random numbers, one per line as decimal numbers in ASCII text. Alternatively, you can use this bash script to accomplish the task.

Submission
Only one team member should submit the assignment. Be sure to indicate all members of your team in the opening comments in your code. If it takes us effort to figure out whose program this is, you will lose points.

The submission should comprise two files:

A pdf file containing the report.
A .tar file containing the source code and a Makefile.
You must submit a Makefile so that I can run make to compile your program (see some basic instructions on makefiles).

Submit only what is required. Do not submit object files, executables, or temporary editor files.

Hand the assignment in via sakai.

Before sending the file to me, make sure that all the components are there. If we cannot compile the assignment, you will get no credit.
