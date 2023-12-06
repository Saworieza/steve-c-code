# Requirements

  * gcc v8.5 or newer
  * moss (to update the latest version see https://theory.stanford.edu/~aiken/moss/)
  * make
  * unzip
  * ls

# Compilation

The code can be compiled with the provided makefile using the standard `make` command.

## Inputting Test-Cases

Before compiling the autograder, you need to open `ag.h` and change lines 6-19 to update:
	* 1. name of program which is going to be graded
		** I assume that for each assignment, all students use the same program name in their Makefile
	* 2. number of test-cases
	* 3. stdin of each test-case 
		* if program expects nothing from stdin, then use empty string
	* 4. and command-line arguments of each test-case 
		* list of each test-case's CLAs is ended by an empty string ""

# Execution

Before running the autograder, you need to make sure that:
	* students only submit one .zip file compressing all .c files, .h files, and a Makefile,
		** to ensure that students submit a zip file, you can "Restrict Upload File Types" on Canvas,
	* all submissions are downloaded from Canvas (using "Download Submissions"),
		*by default, Canvas downloads all submissions after compressing them into a zip file, 
	* the submissions (.zip files) are extracted to the autograder's root directory,
	* moss.pl is placed in the autograder's root directory.

The simplest way to run the autograder is without command line arguments, using

```
./ag
```

This way, the autograder does the following steps:
	* 1. UNZIP step: unzips each submission
	* 2. CLEAN step: cleans each submission (removes all .o files and the executable file)
	* 3. MAKE step: makes each submission's program
	* 4. MOSS step: runs moss for all submissions
	* 5. RUN step: runs all test cases on each submission
	* 6. prints the stdout of each submission in "results.txt" file
	* 7. generates the moss report and prints the report's url on screen

## Command-Line Arguments

The autograder's proper usage of CLAs is one of the following two ways: 
	** ag [-xu] [-xm] [-xs] [-xc] [-s nn mm oo pp qq ...] 
		* -xu skips the UNZIP step (to save time assuming that submissions are already unzipped)
		* -xm skips the MAKE step (for shortening the stderr assuming that programs are already made)
		* -xs skips the MOSS step (to save time)
		* -xc skips the CLEAN step
		* nn, mm, oo, pp, qq, etc are the submissions that you do NOT want to execute/test 
			* e.g. -s 1 skips running all test-cases on the very first submission
			* index/order of submissions is the same as what Canvas shows in "Speed Grader"
	** ag [-xu] [-xm] [-xs] [-xc] [-s nn-mm]
		* nn-mm is the range/window of submissions that you do NOT want to execute/test
			* e.g. -s 2-20 skips the execution of programs in 2nd, 3rd, ..., 20th submission.
		
## Sample Execution

Let's say, there are six submissions (available w/ this package) for a programming assignment.
	* If the goal is to only run the plagiarism check, then do this:
		```
		./ag -xc -xm -s 1-6 | grep http
		```
	* If the goals is to only run all test-cases w/o plagiarism check, then do this:
		```
		./ag -xs
		```
	* If you have unzipped, cleaned, and compiled all submissions before 
	* and you want to execute test cases on all but the third and fifth ones, then do this:
		```
		./ag -xu -xc -xm -xs -s 3 5
		```


