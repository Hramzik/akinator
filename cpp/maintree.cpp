

#include "../headers/tree.hpp"


int main (void) {

    Tree tree = {};
    TREE_CTOR (&tree, "xuy");


    tree_push_left (&tree, tree.root, "lol");


    FTREE_DUMP (&tree);


    return 0;
}