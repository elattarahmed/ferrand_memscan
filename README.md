# RESPONSES TO THE QUESTIONS IN THE FILES

## PART 1

### a. Why is the buffer declared as `unsigned char *` rather than `char *`?

Since we are reading a binary file, we cannot assume that the data will be null-terminated.

### b. What does `fread` return when it reaches the end of the file?

It returns the number of elements successfully read.

### c. What would happen if you used `fopen(path, "r")` instead of `"rb"` on Windows?

It would read the file in text mode, which would convert line endings to the system's native format.

## PART 2

### Draw the list state before and after inserting "secret" into a list that already contains ["admin", "passwd"].

Before: head → ["admin"] → ["passwd"] → NULL
After: head → ["secret"] → ["admin"] → ["passwd"] → NULL

### Head insertion is O(1).

### Why would tail insertion be O(N)?

Tail insertion requires traversing the entire list to find the end.

### When would you prefer one over the other?

Prefer head insertion when you need to add elements at the beginning of the list.
Prefer tail insertion when you need to add elements at the end of the list.

### You cannot write `free(head); head = head->next;` — why not? What must you do instead?

When you free a variable the memory allocated to it is deallocated. Meaning you lose all control over that variable.

Instead, you could use the variable before free-ing it, like saving it's value into another variable before freeing it :)
