# Tarsau - Archive Program

Tarsau is a command-line archive program written in C language for Linux (or Unix) operating systems. It functions similarly to tools like tar, rar, or zip but does not involve compression. The program can be used to merge text files into a single archive and extract files from a previously created archive.

## Table of Contents

- [Usage](#usage)
- [Building](#building)
- [Operations](#operations)
    - [-b: Merge Files](#b-merge-files)
    - [-a: Extract Archive](#a-extract-archive)
## Usage

./tarsau -b input_files -o output_file
./tarsau -a archive_file_name [extract_directory]

## Building

Compile the program using the provided Makefile:

make

## Operations

## -b: Merge Files

Merge multiple text files into a single archive.
Example

./tarsau -b file1.txt file2.txt -o merged.sau

## -a: Extract Archive

Extract files from a previously created archive.
Example

./tarsau -a archive.sau [extract_directory]
