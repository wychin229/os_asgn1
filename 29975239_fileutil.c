/* 
Student Name : Chin Wen Yuan
Student ID : 29975239
Start date : 23/8/2021
Last modified : 27/8/2021

Please read the user documentation provided in userdoc.txt for detail instructions.

The following program is a multipurpose file utility, which combines the 
simplified features of multiple Linux utilities such as head, tail, cat and cp.
Syntax:
	./fileutil [sourcefile] [-d destdir] [-n numlines] [-L]
	
	[sourcefilepath]
		The absolute file path of the source text file to be used.
		(Ex. /home/student/Desktop/FIT2100/x.txt)
	
	[-d destdir] 
		is for specifying the destination directory that the copied text file will
	    be created. (Note, a text file created will be of the same name as the
	    source text file.)
	    
	[-n numlines]
		is for specifying the number of lines to be read/copied from the source
		text file.
		Default is 10 lines, if not specified.
		
	[-L]
		is for specifying that the operations reads from the end of the file. If
		not used, the program will retrieve the first n/10 lines of the source
		text file.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h> 		/* change to <sys/fcntl.h> for System V */
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define TEN "10"

/* get the filename from sourcefile */
char *get_filename(char *instr) {
	char delim[2] = "/";
	char *temp;
	char *ptr;
	char result[1024];
	
	temp = strtok(instr, delim);
	
	while (temp != NULL) {
		strcpy(result,temp);
		temp = strtok(NULL, delim);
	}

	ptr = result;
	return ptr;
}

/* get the FIRST n lines */
int out_first(char *string, int n, int outmode, int outfile) {
	int len = strlen(string);
	char arr[len];
	char delim[2] = "\n";
	char *line;
	char temp[1024];
	int count = 0;
	
	if ((line = strtok(string, delim)) != NULL) {
		count ++;
		strcpy(temp, line);
		strcat(temp, "\n");
		strcpy(arr, temp);
	}

	while (line != NULL) {
		if (count >= n) {
			break;
		}
		
		line = strtok(NULL, delim);

		if (line != NULL) {
			strcpy(temp, line);
			strcat(temp, "\n");
			strcat(arr, temp);
			count++;
		}
	}
	/* output the result string */
	if (outmode == 0) {
		write(1, arr, strlen(arr));
	}
	else {
		lseek(outfile, 0, SEEK_SET);
		write(outfile, arr, strlen(arr));
		char msg[] = "Copy Successful.\n";
		write(1, msg, strlen(msg));
	}
	
	return 0;
}

/* get the LAST n lines */
int out_last(char *string, int n, int outmode, int outfile) {
	int len = strlen(string);
	char lines[n][1024]; /* used to store the LAST n lines */
	char delim[2] = "\n";
	char *line;
	int count = 0;
	
	line = strtok(string, delim);
	
	while (line != NULL) {

		if (count == n) {
			for (int i = 0; i < n-1; i++) {
				strcpy(lines[i],lines[i+1]);
			}
			strcpy(lines[n-1],line);
		}
		else {
			strcpy(lines[count],line);
			count++;
		}
		line = strtok(NULL, delim);
	}
	/* concatenate all the strings collected into one */
	char arr[len];
	for (int i = 0; i < count; i++) {
		strcat(lines[i],"\n");
		strcat(arr, lines[i]);
	}
	/* output to terminal/file after collecting the LAST n lines*/
	if (outmode == 0) {
		write(1, arr, strlen(arr));
	}
	else {
		lseek(outfile, 0, SEEK_END);
		write(outfile, arr, strlen(arr));
		char msg[] = "Copy Successful.\n";
		write(1, msg, strlen(msg));
	}
	return 0;
}

/* 
Main function that calls the operations
mode : 0 if -L is used, 1 if not used
outmode : 0 if terminal, 1 if file
*/
int read_lines(int infile, int n, int mode, int outmode, int outfile) {
	/* get the number of bytes in the file and store it in char[] of same size */
	off_t numbtye = lseek(infile, 0, SEEK_END);
	char buffer[numbtye];
	lseek(infile, 0, SEEK_SET);
	read(infile, &buffer, numbtye);

	if (mode == 0) {
		out_first(buffer, n, outmode, outfile);
	}
	else {
		out_last(buffer, n, outmode, outfile);
	}
	return 0;
}

/* Main entry of the program */
int main(int argc, char *argv[])
{
	char sourcefile[] = "sample.txt";
	char *destdir = NULL;
	char n[] = TEN;
	int opt;
	int first = 0; /* indicate if -L is used */
	int infile, outfile;

	while((opt = getopt(argc, argv, "-:d:n:L")) != -1) {
		switch(opt) {
			case 'd': 
			{
				destdir = optarg; /* get the destination directory if provided */
				break;
			}
			case 'n':
			{
				strcpy(n, optarg); /* number of lines required */
				break;
			}
			case 'L':
			{
				first = 1; /*indicate that this operation is for Last n lines*/
				break;
			}
			case '?':
			{	/* Show message on standard error output */
				char msg2[]  = "Unknown option : Only -d -n -L is supported.\n";
				write(2, msg2, strlen(msg2));
				exit(1);
				break;
			}
			case ':':
			{	/* Show message on standard error output */
				char msg3[100] = "Missing argument.\n";
				if (optopt == 'd') {
					char msg[] = "Missing argument for -d.\n";
					strcpy(msg3, msg);
				}
				else if(optopt == 'n') {
					char msg[] = "Missing argument for -n.\n";
					strcpy(msg3, msg);
				}
				write(2, msg3, strlen(msg3));
				exit(1);
				break;
			}
			case 1:
			{	/* if any sourcefile path is specified */
				strcpy(sourcefile,optarg);
			}
		}
	}

	/* open the sourcefile */
	if ((infile = open(sourcefile, O_RDONLY)) < 0) {
		char msg[] = "Source file does not exist. (";
		strcat(msg, sourcefile);
		strcat(msg, ")\n");
		write(2, msg, strlen(msg));
		exit(1);
	}
	
	/* open the destination file */
	if (destdir != NULL) {
		char *filename;
		filename = get_filename(sourcefile);
		
		/* check if there is an existing file with the same name at the destination */
		if ((outfile = open(strcat(destdir,filename), O_EXCL | O_CREAT | O_WRONLY, 0664)) < 0) {
			char msg[] = "Same filename already exist in the destination directory or the directory does not exist.";
			write(2, msg, strlen(msg));
			exit(2); 
		}
		read_lines(infile, atoi(n), first, 1, outfile);
	}
	else {
		read_lines(infile, atoi(n), first, 0, outfile);
	}
	/* closes all the opened files and exit the program successfully */
	close(infile);
	close(outfile);
	exit(0);
}
