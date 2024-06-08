#include "HuffmanNode.h"
#include <stdlib.h>

// HuffmanNode *create_node(char data, unsigned freq)
// {
//     HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
//     node->data = data;
//     node->freq = freq;
//     node->left = node->right = NULL;
//     return node;
// }

void free_huffman_tree(HuffmanNode *root) {
  if (root == NULL)
    return;
  free_huffman_tree(root->left);
  free_huffman_tree(root->right);
  free(root);
}
