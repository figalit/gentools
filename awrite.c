

/* awrite by Sukru Cinar */

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>



#include "awrite.h"

typedef struct buffer {
  /* if len==0, the write thread exits */
  struct buffer *prev,*next;
  size_t len;
  /* tam page boyu olsun diye 4084 yaptim.
     senin sistem 64 bitse 4072 yapabilin */
  //char data[4072];
  char data[40960];
} buffer_t;

#define buffer_size  sizeof( ((buffer_t*) 0)->data )
#define max_buffers 1000

static  void put_buffer(buffer_t *b); 
static  buffer_t *alloc_buffer(); 
static  void *writer_loop(void *unused_arg); 
static  buffer_t *get_buffer(); 
static  void write_data(char *data, size_t len); 
static  void destroy_mutex_and_cond_vars();
static  buffer_t *half_cooked; 
static  pthread_mutex_t mutex; 
static  pthread_cond_t not_full, not_empty;
static buffer_t queue= { &queue, &queue };
static int n_buffers;
static  FILE *aw_fd; static  pthread_t writer_thread;




void awrite(char *data, size_t len) {
	
  while(len) {
    size_t avail= buffer_size-half_cooked->len;
    size_t cp;
    cp= avail < len ? avail : len ;
    memcpy(half_cooked->data+half_cooked->len, data, cp);
    half_cooked->len+= cp;
    if (half_cooked->len==buffer_size) {
      put_buffer(half_cooked);
      half_cooked= alloc_buffer();
    }
    len -= cp; 
    data+= cp;
  }
}

static  void put_buffer(buffer_t *b) {
	
  pthread_mutex_lock(&mutex);
  while (! ((n_buffers<max_buffers))) {
    pthread_cond_wait (& not_full, &mutex); 
  }


  {
    (b)->prev= ((&queue))->prev; 
    (b)->next= ((&queue)); 
    ((&queue))->prev->next= (b); 
    ((&queue))->prev= (b); 
  }


  n_buffers++;
	
  pthread_mutex_unlock (&mutex);
  pthread_cond_signal (& not_empty );

}







static  buffer_t *alloc_buffer() {
  buffer_t *b;
	
  (b)= (typeof(b)) malloc(sizeof(*(b)));


  b->len= 0;
  return b;
}









int awrite_init(FILE *fd) {
	

  aw_fd= fd;

  pthread_mutex_init(&mutex,NULL);
  pthread_cond_init(&not_empty, NULL);
  pthread_cond_init(&not_full, NULL);

	
  if (pthread_create(&writer_thread, NULL, writer_loop, 0)) {
    destroy_mutex_and_cond_vars();
    return 1;
  }
  n_buffers= 0;
  half_cooked= alloc_buffer();
  return 0;
}

void awrite_end() {
  if (half_cooked->len) {
    put_buffer(half_cooked);
    half_cooked= alloc_buffer();
  }
  put_buffer(half_cooked);
  pthread_join(writer_thread, 0);
  destroy_mutex_and_cond_vars();
}

static  void *writer_loop(void *unused_arg) {
  while(1) {
    size_t L;
    buffer_t *b= get_buffer();
    if (b->len!=0)
      write_data(b->data, b->len);
    L= b->len;
    free(b);
    if (!L)
      break;
  }
  return 0;
}

static  buffer_t *get_buffer() {
  buffer_t *b;
	
  pthread_mutex_lock(&mutex);
  while (! ((n_buffers!=0))) {
    pthread_cond_wait (& not_empty, &mutex); 
  }


  b= queue.next;
  {
    (b)->prev->next= (b)->next; 
    (b)->next->prev= (b)->prev; 
  }


  n_buffers--;
	
  pthread_mutex_unlock (&mutex);
  pthread_cond_signal (& not_full );

  return b;
}

static  void write_data(char *data, size_t len) {
  while(len) {
    ssize_t r;
    r= fwrite(data, len, 1, aw_fd);
    //r= write(aw_fd, data, len);
    if (r<0) {
      switch(errno) {
      case EINTR: continue;
      case ENOSPC:
	exit(fprintf(stderr,"No space left on device!!\n"));
      case EBADF:
      case EFAULT:
      case EINVAL:
	exit(fprintf(stderr,"This program is broken!!\n"
		     "errno= %s\n", strerror(errno)));
      default:
	exit(fprintf(stderr,"I didn't see that coming.\n"
		     "errno= %d : %s\n", errno, strerror(errno)));
      }
    } else {
      //fflush(aw_fd);
      data += r*len;
      len-= r*len;
    }
  }
}

static  void destroy_mutex_and_cond_vars() {
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&not_full);
  pthread_cond_destroy(&not_empty);
}





