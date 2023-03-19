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
    char rp[] = "/tmp/read_pipe";
    char wp[] = "/tmp/write_pipe";

    char text_buffer[5000] = {};
    int literally_bool_buffer[1250] = {};

    mknod(rp, S_IFIFO | 0666, 0);
    mknod(wp, S_IFIFO | 0666, 0);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader is online\n");
        int file = open(argv[1], O_RDONLY);
        read(file, text_buffer, sizeof(text_buffer));

        int read_pipe = open(rp, O_WRONLY);

        write(read_pipe, text_buffer, sizeof(text_buffer));

        close(file);
        close(read_pipe);
    } else {
        pid_t is_writer = fork();

        if (is_writer) {
            printf("Writer is online\n");

            int write_pipe = open(wp, O_RDONLY);

            read(write_pipe, literally_bool_buffer, sizeof(literally_bool_buffer));

            int file = open(argv[2], O_WRONLY | O_CREAT, 0666);

            dprintf(file, "%d\n",literally_bool_buffer[0]);
            printf("%d\n", literally_bool_buffer[0]);

            close(file);
            close(write_pipe);

            unlink(rp);
            unlink(wp);
        } else {
            printf("Checker is online\n");

            int read_pipe = open(rp, O_RDONLY);
            int write_pipe = open(wp, O_WRONLY);

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
    }
    return 0;
}
