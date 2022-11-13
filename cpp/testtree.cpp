

#include "../headers/tree.hpp"


int main (void) {

    //AKINATOR START


    Tree tree = {};
    TREE_CTOR (&tree);


    FTREE_GRAPHDUMP (&tree, "xuyovoe tree");


    tree_read (&tree);


    FTREE_GRAPHDUMP (&tree, "norm tree");


    return 0;
}