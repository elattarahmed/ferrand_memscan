# RESPONSES TO THE QUESTIONS IN THE FILES

## PART 1

### a. Why is the buffer declared as `unsigned char *` rather than `char *`?

Since we are reading a binary file, we cannot assume that the data will be null-terminated.

### b. What does `fread` return when it reaches the end of the file?

It returns the number of elements successfully read.

### c. What would happen if you used `fopen(path, "r")` instead of `"rb"` on Windows?

It would read the file in text mode, which would convert line endings to the system's native format.
