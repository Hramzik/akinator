

#include "../headers/tree.hpp"


int main (void) {

    //AKINATOR START


    Tree tree = {};
    TREE_CTOR (&tree);
    tree_read (&tree);


    FTREE_GRAPHDUMP (&tree);


    return 0;
}