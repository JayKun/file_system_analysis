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
#include "ext2_fs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int ifd;


int main(int argc, char* argv[]){
	char * img_name = NULL; 
	if(argc != 2)
		//print_usage_details();
	
	img_name = (char*)malloc(strlen(argv[1])+1);
	img_name = argv[1];

	fprintf(stderr, "%s\n", img_name);
	ifd = open(img_name, O_RDONLY);
	int errRead = errno;

	if(ifd <0){
	    fprintf(stderr, "ERROR opening image file. Failed to open %s\n", img_name);
	    fprintf(stderr, "%s\n", strerror(errRead));
	    exit(2);
	}
	// SUPERBLOCK INFO
	// read file block
	void *p_superblock;
	p_superblock = calloc(1024, sizeof(char));
	pread(ifd, p_superblock, 1024, 1024);
	struct ext2_super_block* superblock;
	superblock = (struct ext2_super_block *)&p_superblock;
	// 2.
	int n_blocks = superblock->s_blocks_count;
	// 3.
	int n_inodes = superblock->s_inodes_count;
	// 4.
	int block_size = 1 << superblock->s_log_block_size;
	// 5.
	int inode_size = superblock->s_inode_size;
	// 6.
	int blocks_per_group = superblock->s_blocks_per_group;
	// 7.
	int inodes_per_group = superblock->s_inodes_per_group;
	// 8.
	int first_inode = superblock->s_first_ino;
	fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d", n_blocks, n_inodes, block_size, inode_size, blocks_per_group, inodes_per_group, first_inode);		
}
	
