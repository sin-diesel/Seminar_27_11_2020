#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <poll.h>

#define MAX_SIZE 1024

int main (int argc, char** argv) {

    assert(argc > 1);

    int nfds = argc - 1;
    int* fds = (int*) calloc(nfds, sizeof(int));
    assert(fds);

    for (int i = 1; i < argc; ++i) {
        fds[i - 1] = open(argv[i], O_NONBLOCK | O_RDONLY);
        if (fds[i - 1] < 0) {
            perror("Error opening descriptor: ");
        }
    }

    struct pollfd* pollfds = (struct pollfd*) calloc(nfds, sizeof(struct pollfd));
    assert(pollfds);
    for (int i = 0; i < nfds; ++i) {
        pollfds[i].fd = fds[i];
        pollfds[i].events = POLLIN;
    }

    int resop = 0;
    char* buf = (char*) calloc(MAX_SIZE, sizeof(char));
    assert(buf);

    while ((resop = poll(pollfds, nfds, -1) != 0)) {
        for (int i = 0; i < nfds; ++i) {
            if (pollfds[i].revents == POLLIN) {

                int nread = read(fds[i], buf, MAX_SIZE);
                if (nread == -1) {
                    perror("Error reading from fds: ");
                }
                //if (nread != 0) { // do not write several times data from single fd
                int nwrite = write(STDOUT_FILENO, buf, MAX_SIZE);
                if (nwrite == -1) {
                    perror("Error writing to stdout: ");
                }
            }
        }
    }

    return 0;
}