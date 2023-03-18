#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number. Required 2 - input and output files\n");
        return 1;
    }

    int read_pipe[2], write_pipe[2];
    char text_buffer[5000] = {};
    int literally_bool_buffer[1250] = {};

    pipe(read_pipe);
    pipe(write_pipe);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader is online\n");
        int file = open(argv[1], O_RDONLY);
        read(file, text_buffer, sizeof(text_buffer));

        write(read_pipe[1], text_buffer, sizeof(text_buffer));

        close(file);
        exit(0);
    } else {
        pid_t is_writer = fork();

        if (is_writer) {
            printf("Writer is online\n");
            read(write_pipe[0], literally_bool_buffer, sizeof(literally_bool_buffer));
            close(write_pipe[0]);
            close(write_pipe[1]);

            int file = open(argv[2], O_WRONLY | O_CREAT, 0666);


            dprintf(file, "%d\n",literally_bool_buffer[0]);
            printf("%d\n", literally_bool_buffer[0]);

            close(file);
            exit(0);
        } else {
            printf("Checker is online\n");
            read(read_pipe[0], text_buffer, sizeof(text_buffer));
            close(read_pipe[0]);
            close(read_pipe[1]);

            literally_bool_buffer[0] = 1;
            for (int i = 0; i < (strlen(text_buffer) - 1) / 2; ++i) {
                if (text_buffer[i] != text_buffer[strlen(text_buffer) - i - 2]) {
                    literally_bool_buffer[0] = 0;
                    break;
                }
            }
            write(write_pipe[1], literally_bool_buffer, sizeof(literally_bool_buffer));
            exit(0);
        }
    }
    return 0;
}
