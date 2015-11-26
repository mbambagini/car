#ifndef __DIAG_HPP__
#define __DIAG_HPP__

/* \brief initialize the diagnosis module
 *
 */
void init_diag();

/*! \brief execute pending diagnosis operations
 * This thread checks periodically if the driver has required a diagnosis
 * operation.
 * So far, only ECHO is implemented (a message is sent to state the node
 * is alive)
 */
void thread_diag (void const *args);

#endif //__DIAG_HPP__

