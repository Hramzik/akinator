

#include "../headers/tree.hpp"


int main (void) {

    Tree tree = {};
    TREE_CTOR (&tree, nullptr);
    tree_read (&tree);


    akinator_define_mode ();

    // FTREE_GRAPHDUMP (&tree, "first dump");


    printf ("0");
    return 0;
}