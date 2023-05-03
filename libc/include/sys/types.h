#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

typedef int __pid_t;
typedef unsigned int __id_t;

#ifndef __pid_t_defined
typedef __pid_t pid_t;
#define __pid_t_defined
#endif

#ifndef __id_t_defined
typedef __id_t id_t;
#define __id_t_defined
#endif

#endif
