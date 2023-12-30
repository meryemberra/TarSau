# Tarsau - Archive Program

Tarsau is a command-line archive program written in C language for Linux (or Unix) operating systems. It functions similarly to tools like tar, rar, or zip but does not involve compression. The program can be used to merge text files into a single archive and extract files from a previously created archive.

## Table of Contents

- [Usage](#usage)
- [Building](#building)
- [Operations](#operations)
    - [-b Merge Files](#-b-merge-files)
    - [-a Extract Archive](#-a-extract-archive)
- [Contributors](#Contributors)
  
## Usage

./tarsau -b input_files -o output_file
./tarsau -a archive_file_name [extract_directory]

## Building

Compile the program using the provided Makefile:

make

## Operations

### -b: Merge Files

Merge multiple text files into a single archive.

#### Creating an Archive

To create an archive, use the -b option followed by input file names and specify the output archive file name with -o:

tarsau –b t1 t2 t3 t4.txt t5.dat –o s1.sau

### -a: Extract Archive
Extract files from a previously created archive.
#### Extracting from an Archive

To extract files from an existing archive, use the -a option followed by the archive file name and the directory name:

./tarsau -a archive.sau [extract_directory]

## Contributors

- [Ayşe Nur Yılmaz](https://github.com/aysenrr) 
- [Meryem Berra Avşar](https://github.com/meryemberra) 
