#ifndef __GLOBAL_H__
#define __GLOBAL_H__

struct global {
  int unused;
};

extern struct global global;

void global_init(void);

#endif/*__GLOBAL_H__*/
