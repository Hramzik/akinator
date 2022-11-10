

#include "../headers/tree.hpp"
#include "../lib/logs.hpp"


int main (void) {

    Tree tree = {};
    TREE_CTOR (&tree, "xuy");


    tree_push_left (&tree, tree.root, "lol");


    Tree_iterator tree_iterator;
    try (tree_iterator_ctor (&tree_iterator, &tree));
/*

    printf ("%s\n", tree_iterator);
    FTREE_DUMP (&tree);
*/

    return 0;
}