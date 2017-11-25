
#Team Names:
#	Arti Patankar
#	604594513
#	artipatankar@ucla.edu
# 	
#	Jun Kai Ong
#	604606304
#	junkai@g.ucla.edu 

.SILENT:

default:
	gcc -g -Wall -Wextra lab3a.c -o lab3a
clean: default
	-rm lab3a *tar.gz 
dist: default
	tar -czvf lab3a-604606304.tar.gz lab3a.c README Makefile ext2_fs.h
