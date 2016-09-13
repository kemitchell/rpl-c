#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 8192

char* make_temporary_file_name (const char* existing_file_name)
{
    size_t length;
    char* returned;

    length = strlen(existing_file_name);
    returned = (char*) malloc(length + 4 + 1);
    memcpy(returned, existing_file_name, length);
    strncat(returned, ".tmp", 4);
    return returned;
}

int filter(int from, int to)
{
    ssize_t bytes_read;
    ssize_t bytes_written;
    char buffer[BUFFER_SIZE];

    while((bytes_read = read(from, &buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(to, &buffer, (ssize_t) bytes_read);
        if(bytes_written != bytes_read) {
            perror("write");
            return 4;
        }
    }

    // Close files.
    close(from);
    close(to);

    return 0;
}

int main (int argc, char* argv[])
{
    int existing_file;
    int temporary_file;
    char* existing_file_name;
    char* temporary_file_name;
    int filter_result;

    if (argc != 4) {
        printf("Usage: <target> <replacement> <file>\n");
        return 1;
    }

    existing_file_name = argv[3];
    temporary_file_name = make_temporary_file_name(existing_file_name);

    // Open existing file.
    existing_file = open(existing_file_name, O_RDONLY);
    if (existing_file == -1) {
        free(temporary_file_name);
        perror("open");
        return 2;
    }

    // Open temporary file.
    temporary_file = open(temporary_file_name, O_WRONLY | O_CREAT, 0644);
    if (temporary_file == -1) {
        free(temporary_file_name);
        perror("open");
        return 2;
    }

    filter_result = filter(existing_file, temporary_file);
    if (filter_result != 0) {
        free(temporary_file_name);
        return filter_result;
    }

    // Overwrite the file with the temporary file.
    rename(temporary_file_name, existing_file_name);

    free(temporary_file_name);

    return 0;
}
