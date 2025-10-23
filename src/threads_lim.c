#include "threads_lim.h"
#include <windows.h>

static HANDLE sem_handle = NULL;

void threads_lim_init(int max_concurrent){
    sem_handle = CreateSemaphore(NULL, max_concurrent, max_concurrent, NULL);
}

void threads_lim_acquire(void){
    WaitForSingleObject(sem_handle, INFINITE);
}

void threads_lim_release(void){
    ReleaseSemaphore(sem_handle, 1, NULL);
}

void threads_lim_destroy(void){
    if(sem_handle) {
        CloseHandle(sem_handle);
        sem_handle = NULL;
    }
}