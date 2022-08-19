#ifndef colors_h_
#define colors_h_

#include <stdio.h>
#include <stdarg.h>

#ifndef _CL_HEADER
# define _CL_HEADER "\033[95m"
#endif
#ifndef _CL_OKBLUE
# define _CL_OKBLUE "\033[94m"
#endif
#ifndef _CL_OKCYAN
# define _CL_OKCYAN "\033[96m"
#endif
#ifndef _CL_OKGREEN
# define _CL_OKGREEN "\033[92m"
#endif
#ifndef _CL_WARNING
# define _CL_WARNING "\033[93m"
#endif
#ifndef _CL_FAIL
# define _CL_FAIL "\033[91m"
#endif
#ifndef _CL_ENDC
# define _CL_ENDC "\033[0m"
#endif
#ifndef _CL_BOLD
# define _CL_BOLD "\033[1m"
#endif
#ifndef _CL_UNDERLINE
# define _CL_UNDERLINE "\033[4m"
#endif

# define N_ARGS(args...) N_ARGS_HELPER1(args, 9, 8, 7, 6, 5, 4, 3, 2, 1)
# define N_ARGS_HELPER1(args...) N_ARGS_HELPER2(args)
# define N_ARGS_HELPER2(x1, x2, x3, x4, x5, x6, x7, x8, x9, n, x...) n

# define set_cl(args...) set_cl_helper(N_ARGS(args), args)

static inline void set_cl_helper(unsigned int n_args, ...) {
    char *arg;
    va_list ap;
    va_start(ap, n_args);
    for (int i = 0; i < n_args; ++i) {
        arg = va_arg(ap, char *);
        printf("%s", arg);
    }
    va_end(ap);
}

#endif
