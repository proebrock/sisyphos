#include "globals.h"



#ifndef NDEBUG

void assert_failed(const char *test, const char *file,
	int line, const char *message)
{
	if (message != (const char *)0)
	{
		fprintf(stderr, "%s:%i: Assertion Failed: %s (%s)\n", file, line, test, message);
	}
	else
	{
		fprintf(stderr, "%s:%i: Assertion Failed: %s\n", file, line, test);
	}
	abort();
}



void check_bitoperations()
{
	uint64 n;
	unsigned int i, j;


	/* check the CLEARNEXTBIT macro: clear bits in n one by one and check result */
	/* if problems occure here: wrong architecture? or perhaps BSR instead of BSF?*/
	n = UINT64CONST(0xffffffffffffffff);
	for (i = 0; i < 64; i++)
	{
		CLEARNEXTBIT(n, j);
		ASSERT(i == j);
	}
	ASSERT(n == 0);
}

#endif	/* NDEBUG */



void si_print(FILE *f, uint64 n)
{
	const char * const si_prefixes = "kMGTPE";
	unsigned int prefix;
	uint64 denom;

	if (n < 1000)
	{
		fprintf(f, "%lli", n);
	}
	else
	{
		denom = 1000;
		prefix = 0;
		while((n / denom) >= 1000)
		{
			denom *= 1000;
			prefix++;
		}
		ASSERT(denom < strlen(si_prefixes));
		fprintf(f, "%.1f%c", ((1.0) * n) / denom, si_prefixes[prefix]);
	}
}

