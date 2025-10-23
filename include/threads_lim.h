#ifndef THREADS_LIM_H
#define THREADS_LIM_H

#ifdef __cplusplus
extern "C" {
#endif

void threads_lim_init(int max_concurrent);
void threads_lim_acquire(void);
void threads_lim_release(void);
void threads_lim_destroy(void);

#ifdef __cplusplus
}
#endif

#endif