#include "util.h"
#include "program.h"

#include <stdlib.h>
#include <getopt.h>

#define STK_MIN 16
#define STK_MAX 64

#define BUF_MIN 0x4000
#define BUF_MAX 0xffff

size_t bfc_stack_size = STK_MIN;
size_t bfc_buffer_size = BUF_MIN;

Operation reduce_remainder(Operation *ops)
{
	// "[-]" -> calculate remainder from division by number of '-'s
	Operation op = {'%', ops[1].data};
	return op;
}

int main(int argc, char **argv)
{

	if (argc < 2)
		die("usage: bfc <input-file> [-sb]");

	char c;
	while ((c = getopt(argc, argv, "s:b:")) != -1)
	{
		switch(c) {
			case 's':
				bfc_stack_size = strtoul(optarg, NULL, 10);
				if (bfc_stack_size < STK_MIN || bfc_stack_size > STK_MAX)
					die("invalid stack size (min=%zu,max=%zu)", STK_MIN, STK_MAX);
				break;
			case 'b':
				bfc_buffer_size = strtoul(optarg, NULL, 10);
				if (bfc_buffer_size < BUF_MIN || bfc_buffer_size > BUF_MAX)
					die("invalid buffer size (min=%zu,max=%zu)", BUF_MIN, BUF_MAX);
				break;
		}
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
		die("failed to open input file");

	Program *p = Program_fromFile(fp);

	Program_reduce(p, "[-]", reduce_remainder);

	char *output_path = changeExtension(argv[1], "");
	Program_compile(p, output_path);
	free(output_path);

	Program_free(p);

	fclose(fp);

	return 0;
}
