#if defined(__cplusplus)
extern "C" {
#endif
typedef void (*t_debug_switch)(U8, U8);

extern void debug_register (t_debug_switch f);
extern void debug_switch (U8 in, U8 out);

#if defined(__cplusplus)
}
#endif
