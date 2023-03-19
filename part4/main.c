#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number. Required 2 - input and output files\n");
        return 1;
    }

    char text_buffer[5000] = {};
    int literally_bool_buffer[1250] = {};

    mknod("/tmp/read_pipe", S_IFIFO | 0666, 0);
    mknod("/tmp/write_pipe", S_IFIFO | 0666, 0);

    pid_t is_reader_writer = fork();

    if (is_reader_writer) {
        printf("Reader-writer is online\n");

        int file = open(argv[1], O_RDONLY);

        read(file, text_buffer, sizeof(text_buffer));
        close(file);


        int read_pipe = open("/tmp/read_pipe", O_WRONLY);
        int write_pipe = open("/tmp/write_pipe", O_RDONLY);

        write(read_pipe, text_buffer, sizeof(text_buffer));
        read(write_pipe, literally_bool_buffer, sizeof(literally_bool_buffer));

        file = open(argv[2], O_WRONLY | O_CREAT, 0666);


        dprintf(file, "%d\n", literally_bool_buffer[0]);
        printf("%d\n", literally_bool_buffer[0]);

        close(file);
        close(read_pipe);
        close(write_pipe);

        unlink("/tmp/read_pipe");
        unlink("/tmp/write_pipe");
    } else {
        printf("Checker is online\n");

        int read_pipe = open("/tmp/read_pipe", O_RDONLY);
        int write_pipe = open("/tmp/write_pipe", O_WRONLY);

        read(read_pipe, text_buffer, sizeof(text_buffer));

        literally_bool_buffer[0] = 1;
        for (int i = 0; i < strlen(text_buffer) / 2; ++i) {
            if (text_buffer[i] != text_buffer[strlen(text_buffer) - i - 1]) {
                literally_bool_buffer[0] = 0;
                break;
            }
        }

        write(write_pipe, literally_bool_buffer, sizeof(literally_bool_buffer));
        close(read_pipe);
        close(write_pipe);
    }
    return 0;
}