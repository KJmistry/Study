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
    // Open or create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    // Set the size of shared memory
    ftruncate(shm_fd, sizeof(pthread_mutex_t));

    // Map the shared memory
    pthread_mutex_t *mutex = (pthread_mutex_t *)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (mutex == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    // Initialize the mutex in shared memory (this is done only once)
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex, &attr);

    printf("Process 1: Waiting to lock mutex...\n");

    // Lock the mutex
    pthread_mutex_lock(mutex);
    printf("Process 1: Locked the mutex.\n");

    // Simulate work
    sleep(10);

    printf("Process 1: Unlocking the mutex.\n");

    // Unlock the mutex
    pthread_mutex_unlock(mutex);

    // Clean up
    munmap(mutex, sizeof(pthread_mutex_t));
    close(shm_fd);

    // Unlink shared memory to remove the file from /dev/shm
    shm_unlink(SHM_NAME);
    return 0;
}
