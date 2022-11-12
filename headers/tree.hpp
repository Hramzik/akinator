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
#define tree_dump_leafclr     "88CDBE"
#define tree_dump_arrwclr     "0368c0"
#define tree_dump_freearrwclr "000000"

#define tree_default_save_file_name      "work/base.txt"
#define tree_dump_file_name              "work/tree_dump.txt"
#define tree_graph_dump_file_name        "work/graph_dump"
#define tree_graph_file_name             "graph"
#define tree_graph_describtion_file_name "work/graph_describtion"

const size_t GRAPH_WIDTH = 1460;

const bool POLITE_MODE = false;

const size_t MAX_GRAPH_DUMP_NUM = 100;
const size_t MAX_QUESTION_LEN   = 100;
const size_t MAX_ANSWER_LEN     = 100;
const size_t MAX_COMMAND_LEN    = 100;
const size_t DEFAULT_COUNTING_SYSTEM = 10;
//--------------------------------------------------



#define  TREE_CTOR(x, root_value)  _tree_ctor   (x, #x + (#x[0] == '&'), __FILE__, __PRETTY_FUNCTION__, __LINE__, root_value)
#define FTREE_DUMP(x)              _ftree_dump  (x, tree_dump_file_name, __FILE__, __PRETTY_FUNCTION__, __LINE__)


#define FTREE_GRAPHDUMP(x, ...)\
\
    _tree_generate_graph_describtion (x);\
    _tree_generate_graph             ();\
    _ftree_graphdump                 (x, tree_graph_dump_file_name, __FILE__, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__);\
    _tree_show_graph_dump            ()


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


enum User_answer {

    UA_FALSE = 0,
    UA_TRUE  = 1,
    UA_UNDEF = 2,
};


enum Akinator_mode {

    AK_GUESS   = 0,
    AK_DEFINE  = 1,
    AK_COMPARE = 2,
    AK_DUMP    = 3,
    AK_UNKNOWN = 4,
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
    Stack* node_stack;

    const char* mode;
};


typedef struct Entity_trait_struct Entity_trait;
struct         Entity_trait_struct {

    const char* trait;
    bool        has_it;
    bool        have_both;
};
const size_t ENTITY_TRAIT_SIZE = sizeof (Entity_trait);


typedef struct Entity_definition_struct Entity_definition;
struct         Entity_definition_struct {

    Entity_trait* traits_array;
    size_t        num_traits;
};


Return_code _tree_ctor (Tree* tree, const char* name, const char* file, const char* func, int line, Element_value root_str = nullptr);
Return_code  tree_dtor (Tree* tree);
Return_code _node_dtor (Node* node);

Return_code  tree_push_left  (Tree* tree, Node* node, Element_value new_element_value);
Return_code  tree_push_right (Tree* tree, Node* node, Element_value new_element_value);

size_t tree_depth (Tree* tree);

Tree_state  tree_damaged             (Tree* tree);
Tree_state _tree_poison_distribution (Tree* tree);
Tree_state _node_poison_distribution (Node* node);

void        _ftree_dump                      (Tree* tree, const char* file_name, const char* file, const char* func, int line);
Return_code _fdump_nodes                     (FILE* dump_file, Tree* tree, const char* mode = "pre");
Return_code _fprint_node                     (FILE* file, Node* node, size_t depth);
void        _fprint_tabs                     (FILE* file, size_t num);
void        _ftree_graphdump                 (Tree* tree, const char* file_name, const char* file, const char* func, int line, const char* additional_text = "");
void         _tree_show_graph_dump            (void);
void         _tree_generate_graph_describtion (Tree* tree);
void         tree_generate_nodes_describtion (Tree* tree, FILE* file);
void         _tree_generate_graph             (void);

Return_code  tree_save (Tree* tree, const char* file_name = tree_default_save_file_name);
Return_code  tree_read (Tree* tree, const char* file_name = tree_default_save_file_name);
Return_code _node_read (FILE* file, Tree* tree, Node* node);

Return_code tree_iterator_inc            (Tree_iterator* tree_iterator);
Return_code _tree_iterator_inc_in_order  (Tree_iterator* tree_iterator);
Return_code _tree_iterator_inc_pre_order (Tree_iterator* tree_iterator);
Return_code tree_iterator_ctor           (Tree_iterator* tree_iterator, Tree* tree, const char* inc_mode = "pre");
Return_code tree_iterator_dtor           (Tree_iterator* tree_iterator);

Return_code _tree_iterator_ctor_fill_stack_in_order  (Tree_iterator* tree_iterator, Tree* tree);
Return_code _tree_iterator_ctor_fill_stack_pre_order (Tree_iterator* tree_iterator, Tree* tree);

Return_code akinator_guess_mode                    (void);
void        akinator_guess_mode_greetings          (void);
Return_code akinator_guess_mode_ask                (const char* question);
User_answer akinator_guess_mode_get_answer         (void);
void        akinator_guess_mode_bad_answer_message (void);
bool        akinator_guess_mode_check_answer       (const char* answer_string);
void        akinator_guess_mode_win_message        (void);
void        akinator_guess_mode_lose_message       (void);
Return_code akinator_guess_mode_add_to_base        (Tree* tree, Node* answer_node);
char*       akinator_guess_mode_get_string         (void);
bool        akinator_guess_mode_ask_to_add         (void);

Return_code  akinator_define_mode           (void);
void         akinator_define_mode_greetings (void);
void         akinator_define_mode_out       (Stack* stack);

Return_code  akinator_compare_mode           (void);
void         akinator_compare_mode_greetings (void);
void         akinator_compare_mode_out       (Stack* stack);

Return_code akinator_dump_mode (void);


Return_code   akinator_start      (void);
void          akinator_greetings  (void);
Akinator_mode get_akinator_mode   (void);
Akinator_mode reget_akinator_mode (void);


bool _ispositive (const char* answer);
bool _isnegative (const char* answer);
bool _isleaf     (Node* node);

Return_code entity_definition_ctor    (Entity_definition* entity_definition, Stack* stack);
Return_code entity_definition_dtor    (Entity_definition* entity_definition);
Return_code entity_definition_builder (Entity_definition* entity_definition, Tree* tree, char* character);









#endif