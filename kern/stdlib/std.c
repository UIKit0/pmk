#include <types.h>
#include <stdlib.h>
#include <limits.h>

/*
 * Portions of this code are taken from the OpenBSD libc, released under the
 * following license:
 *
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in the
 *	documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *	must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *	may be used to endorse or promote products derived from this software
 *	without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Returns the length of the string passed in str in bytes before a \0 terminator.
 */ 
size_t strlen(char* str) {
	size_t ret = 0;
	
	while (str[ret] != 0x00) {
		ret++;
	}
	
	return ret;
}

/*
 * Separates string s by delim, returning either NULL if there is no more 
 * strings that can be split out, or the next split if there is any.
 */
char* strtok(char *s, const char *delim) {
	char *spanp;
	int c, sc;
	char *tok;
	static char *last;

	if (s == NULL && (s = last) == NULL)
		return NULL;

	// Skip (span) leading delimiters (s += strspn(s, delim), sort of).
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) { // no non-delimiter characters
		last = NULL;
		return NULL;
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *) delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return tok;
			}
		} while (sc != 0);
	}
}

/*
 * Returns a pointer to the first occurrence of character c in string s.
 */
char* strchr(const char *s, int c) {
	const char ch = c;

	for ( ; *s != ch; s++)
		if (*s == '\0')
			return NULL;
	return (char *) s;
}

/*
 * Get next token from string *stringp, where tokens are possibly-empty
 * strings separated by characters from delim.  
 *
 * Writes NULs into the string at *stringp to end tokens.
 * delim need not remain constant from call to call.
 * On return, *stringp points past the last NUL written (if there might
 * be further tokens), or is NULL (if there are definitely no more tokens).
 *
 * If *stringp is NULL, strsep returns NULL.
 */
char* strsep(char **stringp, const char *delim) {
	char *s;
	const char *spanp;
	int c, sc;
	char *tok;

	if ((s = *stringp) == NULL) {
		return NULL;
	}

	for (tok = s;;) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0) {
					s = NULL;
				} else {
					s[-1] = 0;
				}

				*stringp = s;
				return tok;
			}
		} while (sc != 0);
	}
}

/*
 * Compares two strings.
 */
int strcmp(const char* s1, const char* s2) {
	while(*s1 && (*s1==*s2)) {
		s1++, s2++;
	}

	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/*
 * Compares n bytes of the two strings.
 */
int strncmp(const char* s1, const char* s2, size_t n) {
	while(n--) {
		if(*s1++!=*s2++) {
			return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
		}
	}
	
	return 0;
}

/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison. The mappings are
 * based upon ASCII character sequences.
 */
static const unsigned char strcasecmp_charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
	'\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
	'\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
	'\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

/*
 * Performs a case-insensitive comparison between the two strings. Functions
 * otherwise identically to strcmp.
 */
int strcasecmp(const char *s1, const char *s2) {
	const unsigned char *cm = strcasecmp_charmap,
			*us1 = (const unsigned char *) s1,
			*us2 = (const unsigned char *) s2;

	while (cm[*us1] == cm[*us2++]) {
		if (*us1++ == '\0') {
			return 0;
		}
	}

	return (cm[*us1] - cm[*--us2]);
}

/*
 * Performs a case-insensitive comparison between the two strings. Functions
 * otherwise identically to strncmp.
 *
 * However, only n bytes are checked.
 */
int strncasecmp(const char *s1, const char *s2, size_t n) {
	if (n != 0) {
		const unsigned char *cm = strcasecmp_charmap,
				*us1 = (const unsigned char *) s1,
				*us2 = (const unsigned char *) s2;

		do {
			if (cm[*us1] != cm[*us2++]) { 
				return (cm[*us1] - cm[*--us2]);
			}

			if (*us1++ == '\0') {
				break;
			}
		} while (--n != 0);
	}

	return 0;
}

/*
 * Copies the input string into the buffer pointed to by destination. Does NOT
 * perform bound checking.
 */
char *strcpy(char *dest, const char* src) {
	char *ret = dest;
	
	while ((*dest++ = *src++));

	return ret;
}

/*
 * Copies n bytes from the source string to the destination buffer, filling the
 * destination with zeros if source ends prematurely.
 */
char *strncpy(char *dest, const char *src, size_t n) {
	char *ret = dest;
	do {
		if (!n--) {
			return ret;
		}
	} while ((*dest++ = *src++));
	
	while (n--) {
		*dest++ = 0;
	}

	return ret;
}

/*
 * Appends the null-terminated string pointed to by src to the one pointed to
 * by dest.
 */
char *strcat(char *dest, const char *src) {
	char *ret = dest;
	while (*dest) {
		dest++;
	}
	
	while ((*dest++ = *src++));
	return ret;
}

/*
 * Converts an ASCII string of base-10 numbers to an integer.
 */
int atoi(const char *str) {
	return((int) strtol(str, (char **)NULL, 10));
}

/*
 * Convert a string to a signed long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long strtol(const char *nptr, char **endptr, int base) {
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+') {
		c = *s++; 
	}

	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	
	if (base == 0) {
		base = c == '0' ? 8 : 10;
	}

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long) LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long) base;
	cutoff /= (unsigned long) base;

	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c)) {
			c -= '0';
		} else if (isalpha(c)) {
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		} else {
			break;
		}

		if (c >= base) {
			break;
		}
		
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
//		errno = ERANGE;
	} else if (neg) {
		acc = -acc;
	}
	
	if (endptr != 0) {
		*endptr = (char *)(any ? s - 1 : nptr);
	}

	return acc;
}

/*
 * Convert a string to an unsigned long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
unsigned long strtoul(const char *nptr, char **endptr, int base) {
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *s++;
	} while (isspace(c));

	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}

	if (base == 0) {
		base = c == '0' ? 8 : 10;
	}

	cutoff = (unsigned long) ULONG_MAX / (unsigned long) base;
	cutlim = (unsigned long) ULONG_MAX % (unsigned long) base;

	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c)) {
			c -= '0';
		} else if (isalpha(c)) {
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		} else {
			break;
		}

		if (c >= base) {
			break;
		} if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if (any < 0) {
		acc = ULONG_MAX;
//		errno = ERANGE;
	} else if (neg) {
		acc = -acc;
	}

	if (endptr != 0) {
		*endptr = (char *)(any ? s - 1 : nptr);
	}
	
	return acc;
}

/*
 * Finds the first occurrence of value in the first num bytes of ptr.
 */
void* memchr(void* ptr, uint8_t value, size_t num) {
	uint8_t *read = (uint8_t *) ptr;

	for(int i = 0; i < num; i++) {
		if(read[i] == value) return &read[i];
	}

	return NULL;
}

/*
 * Compares the first num bytes in two blocks of memory.
 * Returns 0 if equal, a value greater than 0 if the first byte in ptr1 is
 * greater than the first byte in ptr2; and a value less than zero if the
 * opposite. Note that these comparisons are performed on uint8_t types.
 */
int memcmp(const void* ptr1, const void* ptr2, size_t num) {
	uint8_t *read1 = (uint8_t *) ptr1;
	uint8_t *read2 = (uint8_t *) ptr2;

	for(int i = 0; i < num; i++) {
		if(read1[i] != read2[i]) {
			if(read1[i] > read2[i]) return 1;
			else return -1;
		}
	}

	return 0;
}

/*
 * Copies num bytes from source to destination.
 *
 * This uses several optimisations for the Pentium 3 and above: copies less
 * than 64 bytes use a simple loop with "rep movsd," whereas anything above
 * uses a slightly more complicated version that prefetches large chunks of
 * data at a time.
 */
void* memcpy(void* destination, void* source, size_t num) {
	// Prefetch the first 32 bytes
	__asm__ volatile("prefetchnta (%0)\n" : : "r" (source));

	// Pointer to return at end of function
	void *returnPtr = destination;

	// Is copy less than 64 bytes?
	if(num <= 64) {
		// If the copy is more than 32 bytes, prefetch the next 32 bytes
		if(likely(num > 32)) {
			__asm__ volatile("prefetchnta 32(%0)\n" : : "r" (source));			
		}

		// Copy entire dwords
		unsigned int dwords = ((num & 0x3F) / 4);
		register unsigned long int dummy;
		__asm__ volatile("rep; movsd" :
			"=&D"(destination), "=&S"(source), "=&c"(dummy) :
			"0" (destination), "1" (source), "2" (dwords) :
			"memory");

		// Copy any remaining bytes
		unsigned int remaining = (num & 0x03);

		if(unlikely(remaining)) {
			__asm__ volatile("rep; movsb" :
				"=&D"(destination), "=&S"(source), "=&c"(dummy) :
				"0" (destination), "1" (source),"2" (remaining) :
				"memory");
		}
	} else {
		// Size less than 4K?
		if(num < 4096) {
			// Determine number of 32-byte blocks to prefetch
			unsigned int blocks = (num - 32) / 32;

			for(unsigned int i = 0; i < blocks; i++) {
				__asm__ volatile("prefetchnta (%0)\n" : : "r" (source + (i * 32) + 32));				
			}

			// Copy dwords
			unsigned int dwords = (num / 4);
			register unsigned long int dummy;
			__asm__ volatile("rep movsd" :
				"=&D"(destination), "=&S"(source), "=&c"(dummy) :
				"0" (destination), "1" (source),"2" (dwords) :
				"memory");

			// Any remaining bytes?
			unsigned int remaining = num & 0x03;

			if(unlikely(remaining)) {
				__asm__ volatile("rep movsb" :
					"=&D"(destination), "=&S"(source), "=&c"(dummy) :
					"0" (destination), "1" (source),"2" (remaining) :
					"memory");
			}
		} else {
			// Number of 4k blocks
			unsigned int blocks = (num + 4095) / 4096;

			// Process each block
			for(unsigned int block = 0; block < blocks; block++) {
				// We can copy an entire 4k block
				if(likely(num > 4096)) {
					// Prefetch 4K
					for(unsigned int i = 0; i < (num + 31) / 32; i++) {
						__asm__ volatile("prefetchnta (%0)\n" : : "r" (source + (i * 32) + 32));				
					}

					// Copy a 4k block
					register unsigned long int dummy;
					__asm__ volatile("rep; movsd" :
						"=&D"(destination), "=&S"(source), "=&c"(dummy) :
						"0" (destination), "1" (source), "2" (1024) :
						"memory");
				} else { // < 4096 bytes left
					// Prefetch
					for(unsigned int i = 0; i < 128; i++) {
						__asm__ volatile("prefetchnta (%0)\n" : : "r" (source + (i * 32) + 32));				
					}

					// Copy dwords
					unsigned int dwords = (num / 4);
					register unsigned long int dummy;
					__asm__ volatile("rep; movsd" :
						"=&D"(destination), "=&S"(source), "=&c"(dummy) :
						"0" (destination), "1" (source),"2" (dwords) :
						"memory");

					// Copy remaining bytes
					unsigned int remaining = num & 0x03;

					if(unlikely(remaining)) {
						__asm__ volatile("rep; movsb" :
							"=&D"(destination), "=&S"(source), "=&c"(dummy) :
							"0" (destination), "1" (source),"2" (remaining) :
							"memory");
					}

					// Done.
					return returnPtr;
				}

				// Decrement bytes remaining
				num -= 4096;
			}
		}
	}

	return returnPtr;
}

/*
 * Fills a given segment of memory with a specified value.
 */
void* memset(void* ptr, uint8_t value, size_t num) {
	// make zero fills faster
	if(unlikely(value == 0x00)) {
		return memclr(ptr, num);
	}

	uint8_t *write = (uint8_t *) ptr;

	for(int i = 0; i < num; i++) {
		write[i] = value;
	}

	return ptr;
}

/*
 * Clears count bytes of memory, starting at start, with 0x00.
 */
void* memclr(void* start, size_t count) {
	// Programmer is an idiot
	if(!count) return start;

	__asm__("rep stosl;"::"a"(0),"D"((size_t) start),"c"(count / 4));
	__asm__("rep stosb;"::"a"(0),"D"(((size_t) start) + ((count / 4) * 4)),"c"(count - ((count / 4) * 4)));

	return (void *) ((size_t) start + count);
}

void *memmove(void *dest, const void *src, size_t n) {
	uint8_t tmp[n];
	memcpy(tmp, (void *) src, n);
	memcpy(dest, tmp, n);
	return dest;
}

/*
 * Performs a population count calculation over input x. The population count
 * is the total number of set bits in the 32-bit integer. Note that the input
 * is modified.
 */
unsigned int std_popCnt(uint32_t x) {
	// Count set bits in 2 bit chunk
	x = x - ((x >> 1) & 0x55555555);
	// Do 4 bit chunk
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	// Do 8 bit chunk
	x = x + (x >> 4);

	// Get rid of garbage and make a final value
	x &= 0xF0F0F0F;
	return (x * 0x01010101) >> 24;
}

/*
 * Counts the occurrences of character in the input string.
 */
unsigned int std_strcnt(const char *str, char character) {
	const char *p = str;
	unsigned int count = 0;

	do {
		if (*p == character)
			count++;
	} while (*(p++));

	return count;
}