#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

void print_usage(const char *prog_name)
{
    printf("Usage: %s -c|-d -o <output_file> <input_file>\n", prog_name);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        print_usage(argv[0]);
        return 1;
    }

    char *mode = argv[1];
    char *output_file = argv[3];
    char *input_file = argv[4];

    if (strcmp(mode, "-c") == 0)
    {
        printf("Mode: -c\n");
        printf("Input file: %s\n", input_file);
        printf("Output file: %s\n", output_file);
        if (huffman_compress(input_file, output_file) != 0)
        {
            fprintf(stderr, "Compression failed\n");
            return 1;
        }
    }
    else if (strcmp(mode, "-d") == 0)
    {
        printf("Mode: -d\n");
        printf("Input file: %s\n", input_file);
        printf("Output file: %s\n", output_file);
        if (huffman_decompress(input_file, output_file) != 0)
        {
            fprintf(stderr, "Decompression failed\n");
            return 1;
        }
    }
    else
    {
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
