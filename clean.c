#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CHUNK_SIZE (1024 * 1024) /* 1 MB */

typedef struct KeywordNode {
	char word[256];
	struct KeywordNode *next;
} KeywordNode;
/* One match found in the image */
typedef struct MatchNode {
	char keyword[256];
	long offset;
	char *context_before; /* heap-allocated, ctx_len bytes */
	char *context_after;  /* heap-allocated, ctx_len bytes */
	int context_len;
	struct MatchNode *next;
} MatchNode;

/* PART 1 */
FILE *open_image(const char *filename);
void read_chunks(FILE *fp);
KeywordNode *keyword_load(const char *filename);

/* PART 2 */
KeywordNode *keyword_push(KeywordNode *head, const char *word);
void keyword_print(const KeywordNode *head);
void keyword_free(KeywordNode *head);
MatchNode *match_push(MatchNode *head, const char *keyword, long offset,
                      const char *context_before, const char *context_after,
                      int ctx_len);
void match_print_report(const MatchNode *head);
void match_free(MatchNode *head);

/*
##################################################
#                   PART 1                       #
##################################################
*/
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

KeywordNode *keyword_load(const char *filename) {
	/*
	Complete keyword_load(), which opens a plain-text file (one keyword per
	line) and stores each word for later use. For now, simply print each word to
	the terminal. Requirements: • Skip blank lines (where line[0] == '\0' after
	stripping the newline). • Skip comment lines that start with #. • Strip
	trailing\r and \n characters before using the word. • Print a summary:
	[*] Loaded N keyword(s) from 'wordlist.txt'.
	*/

	/*
	Combine Exercises 1.3 and 2.1: modify keyword_load() to return a
	KeywordNode * instead of printing words immediately. Each valid line
	should be inserted into the list with keyword_push. KeywordNode
	*keyword_load(const char *filename);

	In main, call keyword_load and then keyword_print to verify the result.
	*/

	FILE *dict_fp = fopen(filename, "r");
	if (!dict_fp) {
		perror(filename);
		exit(EXIT_FAILURE);
	}
	char line[256];
	int count = 0;
	KeywordNode *KW_list = NULL;
	while (fgets(line, sizeof(line), dict_fp)) {
		/* TODO: strip newline */
		if (line[0] == '\0' || line[0] == '\n')
			continue;
		/* TODO: skip blank lines and comments */
		if (line[0] == '#')
			continue;
		KW_list = keyword_push(KW_list, line);
		count++;
	}
	printf("[*] Loaded %d keyword(s)\n", count);
	fclose(dict_fp);
	return KW_list;
}

/*
##################################################
#                   PART 2                       #
##################################################
*/

KeywordNode *keyword_push(KeywordNode *head, const char *word) {
	/* 1. Allocate a new KeywordNode with malloc */
	KeywordNode *new_node = (KeywordNode *)malloc(sizeof(KeywordNode));
	if (!new_node) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	/* 2. Copy word into node->word with strncpy */
	strncpy(new_node->word, word, sizeof(new_node->word) - 1);
	new_node->word[sizeof(new_node->word) - 1] = '\0';

	/* 3. Set node->next = head */
	new_node->next = head;

	/* 4. Return node (it is now the new head) */
	return new_node;
}

void keyword_print(const KeywordNode *head) {
	/* Use a local pointer: const KeywordNode *node = head; */
	/* Loop while node != NULL, then advance: node = node->next */

	/*
	Expected output (order reflects insertion order, i.e. reversed from the
	file): Keywords loaded:
	- token
	- secret
	- passwd
	- admin*/
	const KeywordNode *node = head;
	while (node != NULL) {
		printf("- %s", node->word);
		node = node->next;
	}
}

/*Every malloc must have a matching free. Implement keyword_free():*/
void keyword_free(KeywordNode *head) {
	KeywordNode *curr = head;
	KeywordNode *next;
	while (curr != NULL) {
		next = curr->next;
		free(curr);
		curr = next;
	}
	next = NULL;
	curr = NULL;
}

MatchNode *match_push(MatchNode *head, const char *keyword, long offset,
                      const char *context_before, const char *context_after,
                      int ctx_len) {
	/* 1. Allocate a MatchNode */
	MatchNode *new_node = (MatchNode *)malloc(sizeof(MatchNode));
	if (!new_node) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	/* 2. Fill in keyword, offset, context_len */
	strncpy(new_node->keyword, keyword, sizeof(new_node->keyword) - 1);
	new_node->keyword[sizeof(new_node->keyword) - 1] = '\0';
	new_node->offset = offset;

	/* 3. Allocate ctx_len bytes for context_before and context_after */
	new_node->context_len = ctx_len;
	new_node->context_before = (char *)malloc(ctx_len);
	new_node->context_after = (char *)malloc(ctx_len);

	if (!new_node->context_before || !new_node->context_after) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	/* 4. Copy the context data with memcpy */
	memcpy(new_node->context_before, context_before, ctx_len);
	memcpy(new_node->context_after, context_after, ctx_len);

	/* 5. Insert at head, return new head */
	new_node->next = head;
	return new_node;
}

void match_print_report(const MatchNode *head) {
	/*
	Implement match_print_report(). For each node in the match list, print:
	[MATCH #1]
	Keyword : "password"
	Offset : 0x0000220D (8717 bytes from start)
	Context : ...login: ad[password]: secret1...
	To display context bytes, replace non-printable characters (where isprint(c)
	== 0) with a . char- acter
	*/

	int match_number = 1;
	const MatchNode *node = head;
	while (node != NULL) {
		printf("[MATCH #%d]\n", match_number);
		printf("Keyword : \"%s\"\n", node->keyword);
		printf("Offset : 0x%08lX (%ld bytes from start)\n", node->offset,
		       node->offset);
		printf("Context : ...");
		for (int i = 0; i < node->context_len; i++) {
			putchar(isprint((unsigned char)node->context_before[i]) ? node->context_before[i] : '.');
		}
		printf("[%s]", node->keyword);
		for (int i = 0; i < node->context_len; i++) {
			putchar(isprint((unsigned char)node->context_after[i]) ? node->context_after[i] : '.');
		}
		printf("...\n");
		node = node->next;
		match_number++;
	}
}

void match_free(MatchNode *head) {
	/*
	Implement match_free(). Be careful: each MatchNode owns three heap
	allocations — the node itself, context_before, and context_after. All
	three must be freed in the correct order.
	*/
	MatchNode *curr = head;

	while (curr != NULL) {
		MatchNode *next = curr->next;
		free(curr->context_before);
		free(curr->context_after);
		free(curr);
		curr = next;
	}

	curr = NULL;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <image> <keyword_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	KeywordNode *keyword_list = keyword_load(argv[2]);
	FILE *img_fp = open_image(argv[1]);
	read_chunks(img_fp);

	/**/
	keyword_print(keyword_list);
	keyword_free(keyword_list);

	fclose(img_fp);

	return 0;
}