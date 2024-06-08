#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

typedef struct HuffmanNode {
  char data;
  unsigned freq;
  struct HuffmanNode *left, *right;
} HuffmanNode;

// HuffmanNode *create_node(char data, unsigned freq);
void free_huffman_tree(HuffmanNode *root);

#endif
