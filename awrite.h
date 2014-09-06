
#ifndef AWRITE_DOT_H_INCLUDED
#define AWRITE_DOT_H_INCLUDED




#ifdef __cplusplus
extern "C" {
#endif
 void awrite(char *data, size_t len) ; int awrite_init(FILE *fd) ; void awrite_end() ;
#ifdef __cplusplus
}
#endif

#endif
