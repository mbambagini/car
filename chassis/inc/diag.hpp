#ifndef __DIAG_HPP__
#define __DIAG_HPP__

//initialize the diagnosis module
void init_diag();

//execute pending diagnosis operations
void thread_diag (void const *args);

#endif //__DIAG_HPP__

