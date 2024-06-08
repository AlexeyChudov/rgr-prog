#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

#define MAX_TREE_HT 256

typedef struct HuffmanNode
{
    char data;
    unsigned freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct
{
    unsigned size;
    unsigned capacity;
    HuffmanNode **array;
} MinHeap;

HuffmanNode *create_node(char data, unsigned freq)
{
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

MinHeap *create_min_heap(unsigned capacity)
{
    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (HuffmanNode **)malloc(minHeap->capacity * sizeof(HuffmanNode *));
    return minHeap;
}

void swap_min_heap_node(HuffmanNode **a, HuffmanNode **b)
{
    HuffmanNode *t = *a;
    *a = *b;
    *b = t;
}

void min_heapify(MinHeap *minHeap, int idx)
{
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx)
    {
        swap_min_heap_node(&minHeap->array[smallest], &minHeap->array[idx]);
        min_heapify(minHeap, smallest);
    }
}

int is_size_one(MinHeap *minHeap)
{
    return (minHeap->size == 1);
}

HuffmanNode *extract_min(MinHeap *minHeap)
{
    HuffmanNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    min_heapify(minHeap, 0);
    return temp;
}

void insert_min_heap(MinHeap *minHeap, HuffmanNode *minHeapNode)
{
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq)
    {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

void build_min_heap(MinHeap *minHeap)
{
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        min_heapify(minHeap, i);
}

int is_leaf(HuffmanNode *root)
{
    return !(root->left) && !(root->right);
}

MinHeap *create_and_build_min_heap(char data[], int freq[], int size)
{
    MinHeap *minHeap = create_min_heap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = create_node(data[i], freq[i]);
    minHeap->size = size;
    build_min_heap(minHeap);
    return minHeap;
}

HuffmanNode *build_huffman_tree(char data[], int freq[], int size)
{
    HuffmanNode *left, *right, *top;
    MinHeap *minHeap = create_and_build_min_heap(data, freq, size);
    while (!is_size_one(minHeap))
    {
        left = extract_min(minHeap);
        right = extract_min(minHeap);
        top = create_node('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insert_min_heap(minHeap, top);
    }
    return extract_min(minHeap);
}

void print_codes(HuffmanNode *root, int arr[], int top)
{
    if (root->left)
    {
        arr[top] = 0;
        print_codes(root->left, arr, top + 1);
    }

    if (root->right)
    {
        arr[top] = 1;
        print_codes(root->right, arr, top + 1);
    }

    if (is_leaf(root))
    {
        printf("%c: ", root->data);
        for (int i = 0; i < top; ++i)
            printf("%d", arr[i]);
        printf("\n");
    }
}

void huffman_codes(HuffmanNode *root, int arr[], int top, char codes[][MAX_TREE_HT], char *map)
{
    if (root->left)
    {
        arr[top] = 0;
        huffman_codes(root->left, arr, top + 1, codes, map);
    }

    if (root->right)
    {
        arr[top] = 1;
        huffman_codes(root->right, arr, top + 1, codes, map);
    }

    if (is_leaf(root))
    {
        for (int i = 0; i < top; ++i)
        {
            codes[(int)root->data][i] = arr[i] + '0';
        }
        codes[(int)root->data][top] = '\0';
        map[(int)root->data] = 1;
    }
}

void calculate_frequency(FILE *fp, int freq[], char data[], int *size)
{
    int ch;
    while ((ch = fgetc(fp)) != EOF)
    {
        freq[ch]++;
    }

    for (int i = 0; i < 256; i++)
    {
        if (freq[i] > 0)
        {
            data[(*size)++] = (char)i;
        }
    }
}

int write_compressed_file(const char *input_filename, const char *output_filename, char codes[][MAX_TREE_HT], char *map)
{
    FILE *input_fp = fopen(input_filename, "r");
    FILE *output_fp = fopen(output_filename, "wb");

    if (!input_fp || !output_fp)
    {
        fprintf(stderr, "Error: Cannot open input or output file\n");
        return 1;
    }

    fwrite(map, sizeof(char), 256, output_fp);

    int ch;
    int bit_count = 0;
    unsigned char buffer = 0;

    while ((ch = fgetc(input_fp)) != EOF)
    {
        char *code = codes[ch];
        for (int i = 0; code[i] != '\0'; i++)
        {
            buffer = buffer << 1 | (code[i] - '0');
            bit_count++;
            if (bit_count == 8)
            {
                fputc(buffer, output_fp);
                bit_count = 0;
                buffer = 0;
            }
        }
    }

    if (bit_count > 0)
    {
        buffer = buffer << (8 - bit_count);
        fputc(buffer, output_fp);
    }

    fclose(input_fp);
    fclose(output_fp);

    return 0;
}

int read_compressed_file(const char *input_filename, const char *output_filename, HuffmanNode *root)
{
    FILE *input_fp = fopen(input_filename, "rb");
    FILE *output_fp = fopen(output_filename, "w");

    if (!input_fp || !output_fp)
    {
        fprintf(stderr, "Error: Cannot open input or output file\n");
        return 1;
    }

    char map[256];
    fread(map, sizeof(char), 256, input_fp);

    HuffmanNode *current = root;
    int ch;
    while ((ch = fgetc(input_fp)) != EOF)
    {
        for (int i = 7; i >= 0; i--)
        {
            int bit = (ch >> i) & 1;
            if (bit == 0)
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }

            if (is_leaf(current))
            {
                fputc(current->data, output_fp);
                current = root;
            }
        }
    }

    fclose(input_fp);
    fclose(output_fp);

    return 0;
}

int huffman_compress(const char *input_filename, const char *output_filename)
{
    FILE *input_fp = fopen(input_filename, "r");
    if (!input_fp)
    {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_filename);
        return 1;
    }

    int freq[256] = {0};
    char data[256] = {0};
    int size = 0;
    calculate_frequency(input_fp, freq, data, &size);
    fclose(input_fp);

    if (size == 0)
    {
        fprintf(stderr, "Error: Input text is empty!\n");
        return 1;
    }

    HuffmanNode *root = build_huffman_tree(data, freq, size);

    int arr[MAX_TREE_HT], top = 0;
    char codes[256][MAX_TREE_HT] = {{0}};
    char map[256] = {0};
    huffman_codes(root, arr, top, codes, map);

    return write_compressed_file(input_filename, output_filename, codes, map);
}

int huffman_decompress(const char *input_filename, const char *output_filename)
{
    FILE *input_fp = fopen(input_filename, "rb");
    if (!input_fp)
    {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_filename);
        return 1;
    }

    char map[256];
    fread(map, sizeof(char), 256, input_fp);
    fclose(input_fp);

    int freq[256] = {0};
    char data[256] = {0};
    int size = 0;
    for (int i = 0; i < 256; i++)
    {
        if (map[i])
        {
            data[size++] = (char)i;
        }
    }

    if (size == 0)
    {
        fprintf(stderr, "Error: Huffman tree is empty!\n");
        return 1;
    }

    HuffmanNode *root = build_huffman_tree(data, freq, size);
    return read_compressed_file(input_filename, output_filename, root);
}
