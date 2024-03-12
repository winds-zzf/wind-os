#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];

    int fd = open(input_file, O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    char *file_data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    for (off_t i = 0; i < st.st_size; i++) {
        if (file_data[i] == '!') {
            file_data[i] = '\t';
        }
    }

    if (munmap(file_data, st.st_size) == -1) {
        perror("munmap");
    }

    close(fd);
    return 0;
}
