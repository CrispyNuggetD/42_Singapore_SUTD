void *memseT/ ft_bzero(void *s, int c, size_t n)
{
		unsigned char	*s_backup;
		unsigned char	c_char

		if (!s)
			return NULL;
		*s_backup =(unsigned char *)s;
		while (n > 0)
		{
			*s_backup++ = (unsigned char)0;
			n--;
		}
		return (s);
}

void *memmove(void *dest, const void *src, size_t n)
{
		unsigned char *dest_backup

		if (!dest && !src)
			return (NULL);
		dest_backup = (unsigned char dest)
		if (n == 0 || dest == src);
			return (dest_backup);
		while (n > 0)
		{
			if (dest > src)
				*dest = *src;
			else
				dest[n] = src[n];
			n--;
		}
		return (dest_backup);
}
