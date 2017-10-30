/* Handle I/O redirection between child processes and files.
	 Author: Marko Nygård, oi12mnd@cs.umu.se
	 Date: 6.10.2017
*/

#include "execute.h"

/* Duplicate a pipe to a standard I/O file descriptor, and close both pipe ends
 * Arguments:	pip	the pipe
 *		end	tells which end of the pipe shold be dup'ed; it can be
 *			one of READ_END or WRITE_END
 *		destfd	the standard I/O file descriptor to be replaced
 * Returns:	-1 on error, else destfd
 */
int dupPipe(int pip[2], int end, int destfd){
	if(end == READ_END){
		dup2(pip[READ_END], destfd);
	}

	else if(end == WRITE_END){
		dup2(pip[WRITE_END], destfd);
	}

	else{
		fprintf(stderr, "Invalid pipe end.\n");
		return -1;
	}

	return destfd;
}

/* Redirect a standard I/O file descriptor to a file
 * Arguments:	filename	the file to/from which the standard I/O file
 * 				descriptor should be redirected
 * 		flags	indicates whether the file should be opened for reading
 * 			or writing
 * 		destfd	the standard I/O file descriptor which shall be
 *			redirected
 * Returns:	-1 on error, else destfd
 */
int redirect(char *filename, int flags, int destfd){

	// Open file descriptor for writing
	if(flags == STDOUT_FILENO){
		  destfd = open(filename, O_WRONLY| O_CREAT, 0666);
			dup2(destfd, STDOUT_FILENO);
			close(destfd);
			return destfd;
	}

	// Open for reading
	else if(flags == STDIN_FILENO){
		destfd = open(filename, O_RDONLY, 0666);
		dup2(destfd, STDIN_FILENO);
		close(destfd);
		return destfd;
	}

	// Undefined flag
	else{
		fprintf(stderr, "No such flag. Must be 0 (writing) or 1 (reading).\n");
		return -1;
	}
}
