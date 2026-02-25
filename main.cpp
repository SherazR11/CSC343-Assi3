#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <limits.h>

int main() {

    int arr[20];
    int arrsize = 20;

    srand(time(NULL));

    printf("Array elements:\n");
    for (int i = 0; i < arrsize; i++) {
        arr[i] = rand() % 100;  
        printf("%d ", arr[i]);
    }
    printf("\n");

    int fd[2];

    if (pipe(fd) == -1) {
        return 1;
    }

    int id = fork();

    if (id == -1) {
        return 2;
    }

    int start, end;
    int min = INT_MAX;

    if (id == 0) {
        // Child → second half
        start = arrsize / 2;
        end = arrsize;

        for (int i = start; i < end; i++) {
            if (arr[i] < min) {
                min = arr[i];
            }
        }

        printf("Child process ID: %d\n", getpid());
        printf("Child minimum (second half): %d\n", min);

        write(fd[1], &min, sizeof(min));
        close(fd[1]);
    }
    else {
        // Parent → first half
        start = 0;
        end = arrsize / 2;

        for (int i = start; i < end; i++) {
            if (arr[i] < min) {
                min = arr[i];
            }
        }

        printf("Parent process ID: %d\n", getpid());
        printf("Parent minimum (first half): %d\n", min);

        close(fd[1]);

        int childMin;
        read(fd[0], &childMin, sizeof(childMin));
        close(fd[0]);

        wait(NULL);

        int overallMin = (min < childMin) ? min : childMin;

        printf("Overall minimum of array: %d\n", overallMin);
    }

    return 0;
}
