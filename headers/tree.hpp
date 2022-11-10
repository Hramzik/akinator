#ifndef TREE_HPP_INCLUDED
#define TREE_HPP_INCLUDED









#include <sys\stat.h>
#include <locale.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>


#include "../lib/types/Stack.hpp"
#include "../lib/types/Return_code.hpp"

#include "../lib/types/Elements/use_Element_const_str.hpp"
#include "../lib/types/Elements/Element_const_str.hpp"


//-------------------- SETTINGS --------------------
#define ON_TREE_ERROR_DUMPING
#define ON_TREE_AFTER_OPERATION_DUMPIN

#define tree_dump_bgclr       "9aabba"
#define tree_dump_nodeclr     "8bbfec"
#define tree_dump_freeclr     "58758e"
#define tree_dump_arrwclr     "0368c0"
#define tree_dump_freearrwclr "000000"

#define tree_dump_file_name              "work/dump.txt"
#define tree_graph_dump_file_name        "work/graph_dump"
#define tree_graph_file_name             "graph"
#define tree_graph_describtion_file_name "work/graph_describtion"

const size_t GRAPH_WIDTH = 1460;


const size_t DEFAULT_COUNTING_SYSTEM = 10;
//--------------------------------------------------



#define  TREE_CTOR(x, ...)  _tree_ctor   (x, #x + (#x[0] == '&'), __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#define FTREE_DUMP(x)       _ftree_dump  (x, tree_dump_file_name, __FILE__, __PRETTY_FUNCTION__, __LINE__)


#define FTREE_GRAPHDUMP(x, ...)\
\
    tree_generate_graph_describtion (x);\
    tree_generate_graph ();\
    _ftree_graphdump (x, tree_graph_dump_file_name, __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)


#ifdef ON_TREE_ERROR_DUMPING

    #define ASSERT_TREE_OK(x) if (tree_damaged (x)) { FTREE_DUMP (x); LOG_ERROR (BAD_ARGS); return BAD_ARGS; }

    #define TREE_ERROR_DUMP(x) FTREE_DUMP(x)

#else

    #define ASSERT_TREE_OK(x) if (tree_damaged (x)) {                 LOG_ERROR (BAD_ARGS); return BAD_ARGS; }

    #define TREE_ERROR_DUMP(x)

#endif


#ifdef ON_TREE_AFTER_OPERATION_DUMPING
    #define TREE_AFTER_OPERATION_DUMPING(x) FTREE_DUMP (x)
#else
    #define TREE_AFTER_OPERATION_DUMPING(x)
#endif



typedef int Tree_state;
enum        Tree_state_flags  {

    TR_OK                            = 0, //TR - short for Tree_state
    TR_NULLPTR                       = 1,
    TR_NULLPTR_ROOT                  = 2,
    TR_INCORRECT_POISON_DISTRIBUTION = 4,
};



typedef struct Tree_Node_structure Node;
struct         Tree_Node_structure  {

    Element element;
    Node*  left_son;
    Node* right_son;
};
const size_t NODE_SIZE = sizeof (Node);



typedef struct Tree_structure      Tree;
typedef struct Tree_info_structure Tree_info;
struct         Tree_info_structure  {

    const  char* name;
    Tree*        adress;
    const  char* birth_file;
    const  char* birth_func;
    int          birth_line;
};

struct  Tree_structure  {

    Node* root;

    Tree_info debug_info;
};
const size_t TREE_SIZE = sizeof (Tree);


typedef struct Tree_iterator_structure Tree_iterator;
struct         Tree_iterator_structure  {

    Node*  current;
    size_t index;
    size_t depth;
    Stack  node_stack;
};



Return_code _tree_ctor          (Tree* tree, const char* name, const char* file, const char* func, int line, Element_value root_str = nullptr);
Return_code  tree_dtor          (Tree* tree);

Return_code  tree_push_left     (Tree* tree, Node* node, Element_value new_element_value);
Return_code  tree_push_right    (Tree* tree, Node* node, Element_value new_element_value);

Tree_state  tree_damaged                     (Tree* tree);
Tree_state _tree_poison_distribution          (Tree* tree);
Tree_state _node_poison_distribution          (Node* node);

void       _ftree_dump                       (Tree* tree, const char* file_name, const char* file, const char* func, int line);
void       _ftree_graphdump                  (Tree* tree, const char* file_name, const char* file, const char* func, int line, const char* additional_text = "");
void        tree_show_graph_dump             (void);
void        tree_generate_graph_describtion  (Tree* tree);
void        tree_generate_graph              (void);

Return_code tree_iterator_inc  (Tree_iterator* tree_iterator);
Return_code tree_iterator_ctor (Tree_iterator* tree_iterator, Tree* tree);









#endif