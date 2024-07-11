CC = gcc

hash_table: hash_table.o
	$(CC) -o hash_table hash_table.o
	
hash_table.o: hash_table.c hash_table.h
	$(CC) -c hash_table.c