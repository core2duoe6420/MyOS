inline int strlen(char *str)
{
    int i = 0;
    while(str[i++])
        ;
    return i - 1;
}

inline void memcpy(void *dest, const void *src, int count)
{
    if(dest < src)
        for(int i = 0; i < count; i++)
            *((char*)dest + i) = *((char*)src + i);
    else
        for(int i = count - 1; i >= 0; i--)
            *((char*)dest + i) = *((char*)src + i);
}

inline void memset(void* dest, char value, int count)
{
    for(int i = 0; i < count; i++)
        *((char*)dest + i) = value;
}
