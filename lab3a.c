/* Team Names:
 * 	Arti Patankar
 * 	604594513
 * 	artipatankar@ucla.edu
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
#include <time.h>

#define BLOCKSIZE 1024

int ifd;
char* img_file;
struct ext2_super_block superblock;
struct ext2_group_desc group_desc;

char* format_time(u_int32_t t){
	time_t raw_time = t;
	struct tm* timeinfo = gmtime(&raw_time); 
	char* time_string = malloc(20*sizeof(char));
	//char time_string[80];
	if (strftime(time_string, 20, "%m/%d/%y %H:%M:%S", timeinfo) == 0) {
		fprintf(stderr, "%s\n", "strftime() returned 0");
		exit(EXIT_FAILURE);
	}
	return time_string;
}


void superblock_summary(){
	pread(ifd, &superblock, 1024, 1024);
	// 2.
	int n_blocks = superblock.s_blocks_count;
	// 3.
	int n_inodes = superblock.s_inodes_count;
	// 4.
	int block_size = 1024 << superblock.s_log_block_size;
	// 5.
	int inode_size = superblock.s_inode_size;
	// 6.
	int blocks_per_group = superblock.s_blocks_per_group;
	// 7.
	int inodes_per_group = superblock.s_inodes_per_group;
	// 8.
	int first_inode = superblock.s_first_ino;
	fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", n_blocks, n_inodes, block_size, inode_size, blocks_per_group, inodes_per_group, first_inode);		

}

void group_summary() {
// case of only 1 group
	// 2.
	int group_num = 0;
	// 3.
	int n_blocks = superblock.s_blocks_count;
	// 4.
	int n_inodes = superblock.s_inodes_count;
	// 5.
	int n_free_blocks = superblock.s_free_blocks_count;
	// 6.
	int n_free_inodes = superblock.s_free_inodes_count;

	pread(ifd, &group_desc, 32, 2048);
	// 7.
	int block_bitmap_num = group_desc.bg_block_bitmap;
	// 8.
	int inode_bitmap_num = group_desc.bg_inode_bitmap;
	// 9.
	int inode_table_num = group_desc.bg_inode_table;
	fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", group_num, n_blocks, n_inodes, n_free_blocks, n_free_inodes, block_bitmap_num, inode_bitmap_num, inode_table_num);	
}

void bfree() {
	int num_bytes = superblock.s_blocks_count / 8;
	char* block_array = (char*)malloc(num_bytes);
	pread(ifd, block_array, num_bytes, group_desc.bg_block_bitmap * (1024 << superblock.s_log_block_size)); // offset is block number * block size
	
	for (int i = 0; i < num_bytes; i++) {
		for (int j = 0; j < 8; j++) {
			int temp = (block_array[i] >> j) & 1;
			if (temp == 0) {
				// int block_num = (i * 8) + (7 - j);
				int block_num = (i * 8) + (1 + j); // bits not sequentially assigned
				fprintf(stdout, "BFREE,%d\n", block_num); 
			}
		}	
	}

	free(block_array);
}

void ifree() {
	int num_bytes = superblock.s_blocks_count / 8;
	char* inode_array = (char*)malloc(num_bytes);
	pread(ifd, inode_array, num_bytes, group_desc.bg_inode_bitmap * (1024 << superblock.s_log_block_size)); // offset is block number * block size
	
	for (int i = 0; i < num_bytes; i++) {
		for (int j = 0; j < 8; j++) {
			int temp = (inode_array[i] >> j) & 1;
			if (temp == 0) {
				// int inode_num = (i * 8) + (7 - j);
				int inode_num = (i * 8) + (1 + j); // bits not sequentially assigned
				fprintf(stdout, "IFREE,%d\n", inode_num);
			}
		}
	}
	
	free(inode_array);

}

void inode_summary(){
	struct ext2_inode inode;
	unsigned int i = 0;
	for (i = 0; i < superblock.s_inodes_count; i++){
		pread(ifd, &inode, sizeof(struct ext2_inode), 5 * BLOCKSIZE + i * sizeof(struct ext2_inode));
		// inode number
		int inode_number = i + 1;
		
		// file type
		char file_type;
		short mode = inode.i_mode;
		if (mode & 0x8000) {
			file_type = 'f';
		}

		else if (mode & 0x4000) {
			file_type = 'd';
		}

		else if (mode & 0xA000) {
			file_type = 's';
		}

		else file_type = '?'; 
	
		mode = mode & 0xFFF;	
		int owner = inode.i_uid;
		int group = inode.i_gid;
		int link_count = inode.i_links_count;
		char* time_last_change = format_time(inode.i_ctime);
		char* mod_time = format_time(inode.i_mtime);
		char* time_last_access = format_time(inode.i_atime);
		int file_size = inode.i_size;
		int num_blocks = inode.i_blocks;
			
		if (file_type != '?')
			fprintf(stdout, "INODE,%d,%c,0%o,%d,%d,%d,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", inode_number, file_type, mode, owner, group, link_count, time_last_change, mod_time, time_last_access, file_size, num_blocks, inode.i_block[0], inode.i_block[1], inode.i_block[2], inode.i_block[3], inode.i_block[4], inode.i_block[5], inode.i_block[6], inode.i_block[7], inode.i_block[8], inode.i_block[9], inode.i_block[10], inode.i_block[11], inode.i_block[12], inode.i_block[13], inode.i_block[14]);
	}

}


int main(int argc, char* argv[]) {
	img_file = NULL; 
	if(argc != 2) {
		fprintf(stderr, "Correct usage: ./lab3a [filename]\n");
		exit(1);
	}
	
	img_file = (char*)malloc(strlen(argv[1]) + 1);
	img_file = argv[1];

	ifd = open(img_file, O_RDONLY);
	int errRead = errno;

	if(ifd < 0) {
		fprintf(stderr, "ERROR opening image file. Failed to open %s\n", img_file);
		fprintf(stderr, "%s\n", strerror(errRead));
		exit(2);
	}
	superblock_summary();
	group_summary();
	bfree();
	ifree();
	inode_summary();
}
	
