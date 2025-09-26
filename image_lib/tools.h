#ifndef TOLLS_H
#define TOOLS_H

bool is_little_endian() { //returns 1 if system is Little-Endian and 0 if not
    int i = 1;
    char *p = (char *)&i;
    return (p[0] == 1); 
}

#endif
