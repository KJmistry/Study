#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/my_shared_memory"
#define SHM_SIZE 1024

int   shm_fd;
char *shm_ptr;

// Signal handler for cleanup
void cleanup(int signum)
{
    printf("\nWriter terminating. Cleaning up...\n");
    if (munmap(shm_ptr, SHM_SIZE) == -1)
    {
        perror("munmap");
    }
    if (close(shm_fd) == -1)
    {
        perror("close");
    }
    if (shm_unlink(SHM_NAME) == -1)
    {
        perror("shm_unlink");
    }
    printf("Cleanup complete. Goodbye!\n");
    exit(0);
}

int main()
{
    // Set up signal handler
    signal(SIGINT, cleanup);

    // Create or open the shared memory object
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    // Set the size of the shared memory object
    if (ftruncate(shm_fd, SHM_SIZE) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    // Map the shared memory object into the process's address space
    shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    printf("Writer started. Enter text to write to shared memory.\n");

    while (1)
    {
        char buffer[SHM_SIZE];
        printf("Enter text: ");
        fgets(buffer, SHM_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline
        strncpy(shm_ptr, buffer, SHM_SIZE);
        printf("Data written: %s\n", buffer);
        sleep(1);  // Avoid excessive CPU usage
    }

    return 0;
}
