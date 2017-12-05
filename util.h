#ifndef _UTIL_H
#define _UTIL_H

#define info(fmt, ...) fprintf(stderr, "[INFO][%s][%d]"fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define debug(fmt, ...) fprintf(stderr, "[DEBUG][%s][%d]"fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define error(fmt, ...) fprintf(stderr, "[ERROR][%s][%d]"fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif

