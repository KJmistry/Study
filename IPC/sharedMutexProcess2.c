#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/mutex_shm"

int main()
{
    // Open the existing shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    // Map the shared memory
    pthread_mutex_t *mutex = (pthread_mutex_t *)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (mutex == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    printf("Process 2: Waiting to lock mutex...\n");

    // Lock the mutex
    pthread_mutex_lock(mutex);
    printf("Process 2: Locked the mutex.\n");

    // Simulate work
    sleep(2);

    printf("Process 2: Unlocking the mutex.\n");

    // Unlock the mutex
    pthread_mutex_unlock(mutex);

    // Clean up
    munmap(mutex, sizeof(pthread_mutex_t));
    close(shm_fd);

    // Unlink shared memory to remove the file from /dev/shm
    shm_unlink(SHM_NAME);
    return 0;
}
