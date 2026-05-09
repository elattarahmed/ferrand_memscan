#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE (1024 * 1024) /* 1 MB */

FILE *open_image(const char *filename);
void read_chunks(FILE *fp);
void keyword_load(const char *filename);

FILE *open_image(const char *filename) {
	/*
	1: Open the file given by filename in binary read mode.
	2: If fopen returns NULL, call perror(filename) and exit with EXIT_FAILURE.
	3: Use fseek and ftell to determine the total size of the file in bytes,
	    then rewind to the beginning with rewind.
	4: Print the file size to
	    stdout in the format: [*] Image: 'usb.img' size: 1048576 bytes (1 MB)
	5: Return the open FILE *.
	*/

	/* open the file given by filename in binary read mode */
	FILE *img_fp = fopen(filename, "rb");
	/* if fopen returns NULL, call perror(filename) and exit with EXIT_FAILURE.
	 */
	if (img_fp == NULL) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
	/* Use fseek and ftell to determine the total size of the file in bytes,
	then rewind to the beginning with rewind. */
	fseek(img_fp, 0, SEEK_END);
	long size = ftell(img_fp);
	rewind(img_fp);
	/* Print the file size to stdout in the format: [*] Image: 'usb.img'
	size: 1048576 bytes(1 MB) */
	printf("[*] Image: '%s' size: %ld bytes (%ld MB)\n", filename, size,
	       size / (1024 * 1024));
	/* Return the open FILE *. */
	return img_fp;
}

void read_chunks(FILE *img_fp) {
	/*
	Binary files may be hundreds of megabytes or larger. Reading them byte
	by byte with fgetc would be prohibitively slow. Instead, allocate a
	fixed-size buffer and fill it with fread in a loop.
	*/

	/*
	When a match is found, investigators need its exact position in the file so
	they can point their hex editor to that location. Modify your read_chunks()
	loop to maintain a variable file_pos that tracks the cumulative number of
	bytes processed so far. For each chunk, the absolute offset of byte buf[i]
	in the file is: offset = file_pos + 𝑖 Print the offset of the first byte of
	each chunk in hexadecimal:
	[*] Processing chunk at offset 0x00000000 (1048576 bytes)
	[*] Processing chunk at offset 0x00100000 (1048576 bytes)
	...

	*/

	long file_pos = 0;

	unsigned char *buf = malloc(CHUNK_SIZE);
	if (!buf) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	size_t bytes_read;
	while ((bytes_read = fread(buf, 1, CHUNK_SIZE, img_fp)) > 0) {
		file_pos += bytes_read;
		/* TODO: process buf[0 .. bytes_read-1] */
		printf("[*] Processing chunk at offset 0x%lx (%ld bytes)\n", file_pos,
		       bytes_read);
	}
	printf("[*] Total bytes read: %ld\n", file_pos);
	free(buf);
}

void keyword_load(const char *filename) {
	/*
	Complete keyword_load(), which opens a plain-text file (one keyword per
	line) and stores each word for later use. For now, simply print each word to
	the terminal. Requirements: • Skip blank lines (where line[0] == '\0' after
	stripping the newline). • Skip comment lines that start with #. • Strip
	trailing\r and \n characters before using the word. • Print a summary:
	[*] Loaded N keyword(s) from 'wordlist.txt'.
	*/

	FILE *dict_fp = fopen(filename, "r");
	if (!dict_fp) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
	char line[256];
	int count = 0;
	while (fgets(line, sizeof(line), dict_fp)) {
		/* TODO: strip newline */
		if (line[0] == '\0' || line[0] == '\n')
			continue;
		/* TODO: skip blank lines and comments */
		if (line[0] == '#')
			continue;
		printf(" keyword: %s", line);
		count++;
	}
	printf("[*] Loaded %d keyword(s)\n", count);
	fclose(dict_fp);
}

/*
When a match is found, investigators need its exact position in the file so they
can point their hex editor to that location. Modify your read_chunks() loop to
maintain a variable file_pos that tracks the cumulative number of bytes
processed so far. For each chunk, the absolute offset of byte buf[i] in the file
is: offset = file_pos + 𝑖 Print the offset of the first byte of each chunk in
hexadecimal:
[*] Processing chunk at offset 0x00000000 (1048576 bytes)
[*] Processing chunk at offset 0x00100000 (1048576 bytes)
...

*/

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <image> <keyword_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	keyword_load(argv[2]);
	FILE *img_fp = open_image(argv[1]);
	read_chunks(img_fp);

	fclose(img_fp);
	return 0;
}