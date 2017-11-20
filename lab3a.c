/* Team Names:
 * 	Arti Patankar
 * 	
 * 	Jun Kai Ong
 *	604606304
 *	junkai@g.ucla.edu
 * 
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int ifd;


int main(int argc, char* argv[]){
	char * img_name = NULL; 
	if(argc != 2)
		print_usage_details();
	
	img_name = argv[1];

	ifd = open(img_name, O_RDONLY);
	int errRead = errno;

	if (ifd >= 0) {
	    close(0);
	}

	else{
	    fprintf(stderr, "ERROR opening image file. Failed to open %s\n", img_name);
	    fprintf(stderr, "%s\n", strerror(errRead));
	    exit(2);
	}

	
}
	
