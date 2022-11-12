

#include "../lib/stack.hpp"
#include "../lib/logs.hpp"

#include "../headers/tree.hpp"


static char GLOBAL_graph_dump_num [MAX_GRAPH_DUMP_NUM] = "1";


Return_code  _tree_ctor  (Tree* tree, const char* name, const char* file, const char* func, int line, Element_value root_str) {

    assert ( (file) && (name) && (func) && (line > 0) );
    if (tree == nullptr) { LOG_ERROR (BAD_ARGS); TREE_ERROR_DUMP (tree); return BAD_ARGS; }


    tree->root = (Node*) calloc (NODE_SIZE, 1);


    tree->root->element   = { root_str, false };
    tree->root->left_son  = nullptr;
    tree->root->right_son = nullptr;;


    tree->debug_info.name       = name;
    tree->debug_info.birth_file = file;
    tree->debug_info.birth_func = func;
    tree->debug_info.birth_line = line;
    tree->debug_info.adress     = tree;


    TREE_AFTER_OPERATION_DUMPING (tree);


    return SUCCESS;
}


Return_code  tree_dtor  (Tree* tree) {

    ASSERT_TREE_OK (tree);


    _node_dtor (tree->root);


    return SUCCESS;
}


Return_code  _node_dtor  (Node* node) {

    if (!node) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    if (node->left_son)  { try (_node_dtor (node->left_son));  }
    if (node->right_son) { try (_node_dtor (node->right_son)); }

    free (node);


    return SUCCESS;
}


Return_code  tree_push_left  (Tree* tree, Node* node, Element_value new_element_value) {

    if (!tree || !node) { LOG_ERROR (BAD_ARGS); FTREE_DUMP (tree); return BAD_ARGS; };

    node->left_son = (Node*) calloc (NODE_SIZE, 1);

    node->left_son->element   = { new_element_value, false };
    node->left_son->left_son  = nullptr;
    node->left_son->right_son = nullptr;


    return SUCCESS;
}


Return_code  tree_push_right  (Tree* tree, Node* node, Element_value new_element_value) {

    if (!tree || !node) { LOG_ERROR (BAD_ARGS); FTREE_DUMP (tree); return BAD_ARGS; };

    node->right_son = (Node*) calloc (NODE_SIZE, 1);

    node->right_son->element   = { new_element_value, false };
    node->right_son->left_son  = nullptr;
    node->right_son->right_son = nullptr;


    return SUCCESS;
}


Tree_state  tree_damaged  (Tree* tree) {

    Tree_state tree_state = TR_OK;


    if (!tree) { tree_state |= TR_NULLPTR; return tree_state; }


    if (!tree->root) { tree_state |= TR_NULLPTR_ROOT; }

    tree_state |= _tree_poison_distribution (tree);

    return tree_state;
}


Tree_state _tree_poison_distribution (Tree* tree) {

    return _node_poison_distribution (tree->root);
}


Tree_state _node_poison_distribution (Node* node) {

    if (!node) { return TR_OK; }


    if (node->element.poisoned) {

        return TR_INCORRECT_POISON_DISTRIBUTION;
    }

    if ( node->left_son && _node_poison_distribution (node->left_son) ) {

        return TR_INCORRECT_POISON_DISTRIBUTION;
    }

    if ( node->right_son && _node_poison_distribution (node->right_son) ) {

        return TR_INCORRECT_POISON_DISTRIBUTION;
    }


    return TR_OK;
}


void  _ftree_dump  (Tree* tree, const char* file_name, const char* file, const char* func, int line) {

    assert ( (file_name) && (file) && (func) && (line > 0) );


    const  char* file_mode          = nullptr;
    static bool  first_time_dumping = true;

    if (first_time_dumping) { file_mode = "w"; }
    else                    { file_mode = "a"; }


    FILE* dump_file = fopen (file_name, file_mode);
    if (dump_file == nullptr) { LOG_ERROR (FILE_ERR); return; }


    setvbuf (dump_file, NULL, _IONBF, 0);


    fprintf (dump_file, "--------------------\n");
    fprintf (dump_file, "Dumping tree at %s in function %s (line %d)...\n\n", file, func, line);


    if (!tree) { fprintf (dump_file, "Tree pointer is nullptr!\n\n"); return; }


    fprintf (dump_file, "this tree has name ");
    if (tree->debug_info.name != nullptr) { fprintf (dump_file, "%s ", tree->debug_info.name); }
    else                                  { fprintf (dump_file, "UNKNOWN NAME "); }
    fprintf (dump_file, "[%p]\n", tree->debug_info.adress);

    fprintf (dump_file, "it was created in file ");
    if (tree->debug_info.birth_file != nullptr) { fprintf (dump_file, "%s\n", tree->debug_info.birth_file); }
    else                                        { fprintf (dump_file, "UNKNOWN NAME\n"); }

    fprintf (dump_file, "in function ");
    if (tree->debug_info.birth_func != nullptr) { fprintf (dump_file, "%s ", tree->debug_info.birth_func); }
    else                                        { fprintf (dump_file, "UNKNOWN NAME "); }

    fprintf (dump_file, "(line %d)\n\n", tree->debug_info.birth_line);


    fprintf (dump_file, "tree is ");
    Tree_state tree_state = tree_damaged (tree);
    if (tree_state) { fprintf (dump_file, "damaged (damage code %u)\n", tree_state); }
    else            { fprintf (dump_file, "ok\n"); }


    if (tree->root) { fprintf (dump_file, "root [%p].\n\n", tree->root); }
    else            { fprintf (dump_file, "root [nullptr].\n\n"); }



    fprintf (dump_file, "nodes:\n\n\n");

    fprintf (dump_file, "pre-order:\n\n");
    _fdump_nodes (dump_file, tree, "pre");

    fprintf (dump_file, "\n\nin-order:\n\n");
    _fdump_nodes (dump_file, tree, "in");

    fprintf (dump_file, "\n\nas in save file:\n\n");
    _fprint_node (dump_file, tree->root, 0);



    fprintf (dump_file, "\n");


    fclose (dump_file);


    first_time_dumping = false;
}


Return_code  tree_iterator_inc  (Tree_iterator* tree_iterator) {

    if (!tree_iterator || !tree_iterator->mode) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    if (!strcmp (tree_iterator->mode, "in"))  { return _tree_iterator_inc_in_order  (tree_iterator); }
    if (!strcmp (tree_iterator->mode, "pre")) { return _tree_iterator_inc_pre_order (tree_iterator); }


    LOG_ERROR (BAD_ARGS);
    return BAD_ARGS;
}


Return_code  _tree_iterator_inc_in_order  (Tree_iterator* tree_iterator) {

    if (!tree_iterator) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    if (tree_iterator->current->right_son) {

        tree_iterator->current = tree_iterator->current->right_son;
        stack_push (tree_iterator->node_stack, tree_iterator->current);
        tree_iterator->depth += 1;

        while (tree_iterator->current->left_son) {

            tree_iterator->current = tree_iterator->current->left_son;
            stack_push (tree_iterator->node_stack, tree_iterator->current);
            tree_iterator->depth += 1;
        }

        tree_iterator->index += 1;
        return SUCCESS;
    }

    Node* old = tree_iterator->current;
    stack_pop (tree_iterator->node_stack);
    tree_iterator->current = (Node*) stack_pop (tree_iterator->node_stack).value;
    tree_iterator->depth -= 1;
    stack_push (tree_iterator->node_stack, tree_iterator->current);



    while (tree_iterator->current->right_son == old) {//пришел справа

        if (tree_iterator->depth == 0) { return BAD_ARGS; } // already was at last element


        old = tree_iterator->current;
        stack_pop (tree_iterator->node_stack);

        tree_iterator->current = (Node*) stack_pop (tree_iterator->node_stack).value;
        stack_push (tree_iterator->node_stack, tree_iterator->current);
        tree_iterator->depth -= 1;
    }

//пришел слева
    tree_iterator->index += 1;
    return SUCCESS;
}


Return_code  _tree_iterator_inc_pre_order  (Tree_iterator* tree_iterator) {

    if (!tree_iterator) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }



    if (tree_iterator->current->left_son) {

        tree_iterator->current = tree_iterator->current->left_son;
        stack_push (tree_iterator->node_stack, tree_iterator->current);
        tree_iterator->depth += 1;
        tree_iterator->index += 1;
        return SUCCESS;
    }



    Node* old = tree_iterator->current;
    while (true) {

        old = tree_iterator->current;
        stack_pop (tree_iterator->node_stack);
        tree_iterator->current = (Node*) stack_pop (tree_iterator->node_stack).value;
        stack_push (tree_iterator->node_stack, tree_iterator->current);


        if (!tree_iterator->current) { return BAD_ARGS; } //pop returned nullptr <=> root was already popped <=> chain was started by last node


        tree_iterator->depth -= 1;


        if (tree_iterator->current->left_son == old) { //пришли слева

            stack_push (tree_iterator->node_stack, tree_iterator->current->right_son);
            tree_iterator->current = tree_iterator->current->right_son;

            tree_iterator->depth += 1;
            break;
        }
    }


    tree_iterator->index += 1;
    return SUCCESS;
}


Return_code  tree_iterator_ctor  (Tree_iterator* tree_iterator, Tree* tree, const char* mode) {

    if (!tree_iterator || !mode || tree_damaged (tree)) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    tree_iterator->node_stack = (Stack*) calloc (STACK_SIZE, 1);
    STACK_CTOR (tree_iterator->node_stack);

    if (!strcmp (mode, "in")) {

        _tree_iterator_ctor_fill_stack_in_order (tree_iterator, tree);
    }

    if (!strcmp (mode, "pre")) {

        _tree_iterator_ctor_fill_stack_pre_order (tree_iterator, tree);
    }


    tree_iterator->index = 0;


    tree_iterator->mode = mode;


    return SUCCESS;
}


Return_code  _tree_iterator_ctor_fill_stack_in_order  (Tree_iterator* tree_iterator, Tree* tree) {

    if (!tree_iterator || tree_damaged (tree)) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    tree_iterator->current = tree->root;


    stack_push (tree_iterator->node_stack, tree_iterator->current);
    tree_iterator->depth = 0;


    while (tree_iterator->current->left_son) {

        tree_iterator->current = tree_iterator->current->left_son;
        stack_push (tree_iterator->node_stack, tree_iterator->current);
        tree_iterator->depth += 1;
    }


    return SUCCESS;
}


Return_code  _tree_iterator_ctor_fill_stack_pre_order  (Tree_iterator* tree_iterator, Tree* tree) {

    if (!tree_iterator || tree_damaged (tree)) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    tree_iterator->current = tree->root;


    stack_push (tree_iterator->node_stack, tree_iterator->current);
    tree_iterator->depth = 0;


    return SUCCESS;
}


Return_code  tree_iterator_dtor (Tree_iterator* tree_iterator) {

    if (!tree_iterator) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    stack_dtor (tree_iterator->node_stack);
    free       (tree_iterator->node_stack);


    return SUCCESS;
}


bool  _isleaf  (Node* node) {

    assert (node);


    if (!node->left_son && !node->right_son) { return true; }


    return false;
}


Return_code  _fprint_node  (FILE* file, Node* node, size_t depth) {

    if (!file || !node) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    _fprint_tabs (file, depth);



    if (_isleaf (node)) { fprintf (file, "{%s}\n", node->element.value); return SUCCESS; }



    fprintf (file, "{%s\n", node->element.value);

    try (_fprint_node (file, node->left_son,  depth + 1));
    try (_fprint_node (file, node->right_son, depth + 1));

    _fprint_tabs (file, depth);
    fprintf (file, "}\n");


    return SUCCESS;
}


void  _fprint_tabs  (FILE* file, size_t num) {

    for (size_t i = 0; i < num; i++) {
    
        fprintf (file, "    ");
    }
}


Return_code  _fdump_nodes  (FILE* dump_file, Tree* tree, const char* mode) {

    if (!dump_file || !tree || !mode) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    Tree_iterator tree_iterator = {};
    tree_iterator_ctor (&tree_iterator, tree, mode);

    do {

        _fprint_tabs (dump_file, tree_iterator.depth);

        fprintf (dump_file, "[%zd] = %s (depth %zd)\n", tree_iterator.index, tree_iterator.current->element.value, tree_iterator.depth);
    }
    while (!tree_iterator_inc (&tree_iterator));

    tree_iterator_dtor (&tree_iterator);


    return SUCCESS;
}


Return_code  tree_save  (Tree* tree, const char* file_name) {

    if (!tree || !file_name) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    FILE* file = fopen (file_name, "w");
    if (!file) { LOG_ERROR (FILE_ERR); return FILE_ERR; }


    _fprint_node (file, tree->root, 0);


    return SUCCESS;
}


Return_code  tree_read  (Tree* tree, const char* file_name) {

    if (!tree || !file_name) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    FILE* file = fopen (file_name, "r");
    if (!file) { LOG_ERROR (FILE_ERR); return FILE_ERR; }


    _node_read (file, tree, tree->root);


    return SUCCESS;
}


Return_code  _node_read  (FILE* file, Tree* tree, Node* node) {

    if (!file || !node) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    while (fgetc (file) != '{') { continue; }


    size_t num_read_sons = 0;
    bool read_question = false;
    char* question = (char*) calloc (MAX_QUESTION_LEN, 1);
    question [0]   = '\0';
    int c = 1;

    while (c != EOF) {

        c = fgetc (file);

        if (c == '}') {

            if (!read_question) { node->element.value = question; }
            break;
        }

        if (c == '{') {

            if (num_read_sons >= 2) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


            ungetc (c, file);

            if (num_read_sons == 1) { //already read left son

                tree_push_right (tree, node, nullptr);
                _node_read      (file, tree, node->right_son);
                num_read_sons += 1;
                continue;
            }

            tree_push_left (tree, node, nullptr); //reading left son
            _node_read     (file, tree, node->left_son);
            num_read_sons += 1;
            continue;
        }

        if (c == '\n' && !read_question) { node->element.value = question; read_question = true; }

        if (!read_question) {

            question [strlen (question) + 1] = '\0';
            question [strlen (question)]     = (char) c;
        }
    }


    return SUCCESS;
}


Return_code  akinator_guess_mode  (void) {

    Tree tree = {};
    TREE_CTOR (&tree, nullptr);

    tree_read (&tree);

    akinator_guess_mode_greetings ();
    Node* current_question_node = tree.root;
    while (!_isleaf (current_question_node)) {

        akinator_guess_mode_ask (current_question_node->element.value);
        User_answer answer = akinator_guess_mode_get_answer ();

        switch (answer) {

            case UA_TRUE:

                current_question_node = current_question_node->right_son;
                break;

            case UA_FALSE:

                current_question_node = current_question_node->left_son;
                break;

            case UA_UNDEF:

                akinator_guess_mode_bad_answer_message ();
                break;

            default:

                akinator_guess_mode_bad_answer_message ();
                break;
        }
    }


    bool good_outcome = akinator_guess_mode_check_answer (current_question_node->element.value);


    if (good_outcome) { akinator_guess_mode_win_message (); return SUCCESS; }


    bool add_flag = akinator_guess_mode_ask_to_add ();

    if (!add_flag) { akinator_guess_mode_lose_message (); return SUCCESS; }

    akinator_guess_mode_add_to_base (&tree, current_question_node);


    return SUCCESS;
}


void  akinator_guess_mode_greetings  (void)  {

    printf ("\n");
    printf ("-------------------------------------------\n");
    printf ("Think of an entity, i will try to guess it!\n");
    printf ("-------------------------------------------\n");
}


Return_code  akinator_guess_mode_ask  (const char* question) {

    if (!question) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    printf ("\ndoes your character %s?\n\n", question);


    return SUCCESS;
}


User_answer akinator_guess_mode_get_answer (void) {

    char answer_string   [MAX_ANSWER_LEN + 1] = "";

    gets (answer_string);

    if (_ispositive (answer_string))   { return UA_TRUE; }

    if (_isnegative (answer_string))   { return UA_FALSE; }


    return UA_UNDEF;
}


bool  _ispositive  (const char* answer) {

    assert (answer);


    if (!stricmp (answer, "y"))     { return true; }
    if (!stricmp (answer, "yes"))   { return true; }
    if (!stricmp (answer, "true"))  { return true; }
    if (!stricmp (answer, "da"))    { return true; }
    if (!stricmp (answer, "right")) { return true; }


    return false;
}


bool  _isnegative  (const char* answer) {

    assert (answer);


    if (!stricmp (answer, "n"))     { return true; }
    if (!stricmp (answer, "no"))    { return true; }
    if (!stricmp (answer, "false")) { return true; }
    if (!stricmp (answer, "net"))   { return true; }
    if (!stricmp (answer, "wrong")) { return true; }


    return false;
}


void  akinator_guess_mode_bad_answer_message  (void) {

    printf ("\ni couldn't understand your answer... \n\ntype 'y' for yes, 'n' for no\n\n");
}


bool  akinator_guess_mode_check_answer  (const char* answer_string) {

    assert (answer_string);


    printf ("\nis your character %s?\n\n", answer_string);

    User_answer answer = akinator_guess_mode_get_answer ();
    switch (answer) {

            case UA_TRUE:

                return true;

            case UA_FALSE:

                return false;

            case UA_UNDEF:

                akinator_guess_mode_bad_answer_message ();
                return akinator_guess_mode_check_answer (answer_string);

            default:

                akinator_guess_mode_bad_answer_message ();
                return akinator_guess_mode_check_answer (answer_string);
        }
}


void  akinator_guess_mode_win_message  (void) {

    if (POLITE_MODE) {

        printf ("\nmy calculations are always correct!\n\n");
    }

    else {
        printf ("\nGG EZ GET F***ED LMAO\n\n");
    }
}


void  akinator_guess_mode_lose_message  (void) {

    if (POLITE_MODE) {

        printf ("\nkeep your secrets...\n\n");
    }

    else {
        printf ("\nno one cares, lol. who asked? +ratio\n\n");
    }
}


Return_code  akinator_guess_mode_add_to_base  (Tree* tree, Node* answer_node) {

    printf ("\nwho is your character?\n\n");


    char character [MAX_ANSWER_LEN + 1] = "";
    gets (character);


    tree_push_left  (tree, answer_node, answer_node->element.value);
    tree_push_right (tree, answer_node, character);

    printf ("\nwhat does %s different of %s?\n\n", character, answer_node->element.value);


    char difference [MAX_ANSWER_LEN + 1] = "";
    gets (difference);


    answer_node->element.value = difference;


    tree_save (tree);

    if (POLITE_MODE) {

        printf ("\nnext time i'll be stronger\n\n");
    }

    else {
        printf ("\nnext time i'll beat you'r m/f a**\n\n");
    }

    return SUCCESS;
}


bool  akinator_guess_mode_ask_to_add  (void) {

    printf ("\nwould you like to add your character to base?\n\n");

    User_answer answer = akinator_guess_mode_get_answer ();
    switch (answer) {

            case UA_TRUE:

                return true;

            case UA_FALSE:

                return false;

            case UA_UNDEF:

                akinator_guess_mode_bad_answer_message ();
                return akinator_guess_mode_ask_to_add  ();

            default:

                akinator_guess_mode_bad_answer_message ();
                return akinator_guess_mode_ask_to_add  ();
        }
}


Return_code  akinator_define_mode  (void) {

    akinator_define_mode_greetings ();


    char character [MAX_ANSWER_LEN + 1] = "";
    gets (character);


    Tree tree = {};
    TREE_CTOR (&tree, nullptr);
    tree_read (&tree);


    Tree_iterator tree_iterator = {};
    tree_iterator_ctor (&tree_iterator, &tree, "pre");

    do {

        if (!_isleaf (tree_iterator.current)) { continue; }

        if (!stricmp (character, tree_iterator.current->element.value)) {

            akinator_define_mode_out (tree_iterator.node_stack);
            tree_iterator_dtor (&tree_iterator);
            return SUCCESS;
        }
    }
    while (!tree_iterator_inc (&tree_iterator));


    printf ("\n\nunfortunately, i don't know who %s is\n\n", character);


    tree_iterator_dtor (&tree_iterator);


    return SUCCESS;
}


void  akinator_define_mode_greetings  (void)  {

    printf ("\n");
    printf ("--------------------------------\n");
    printf ("Enter an entity, i'll define it!\n");
    printf ("--------------------------------\n\n");
}


void  akinator_define_mode_out  (Stack* stack) {

    assert (stack);


    printf ("\n\n");


    Node* character_node = (Node*) stack_pop (stack).value;
    stack_push (stack, character_node);
    const char* character = character_node->element.value;


    Entity_definition character_definition = {};
    entity_definition_ctor (&character_definition, stack);

    for (size_t i = 0; i < character_definition.num_traits; i++) {
    
        if (character_definition.traits_array [i].has_it) { printf ("%s does %s\n\n",       character, character_definition.traits_array [i].trait); }
        else                                              { printf ("%s doesn't %s\n\n",    character, character_definition.traits_array [i].trait); }
    }


    entity_definition_dtor (&character_definition);  
}


Return_code  entity_definition_ctor  (Entity_definition* entity_definition, Stack* stack) {

    if (!entity_definition || !stack) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    entity_definition->traits_array = (Entity_trait*) calloc ((stack->size - 1) * ENTITY_TRAIT_SIZE, 1);
    entity_definition->num_traits   = stack->size - 1;


    Node* current_question = (Node*) stack_pop (stack).value;
    Node* old_question     = nullptr;
    size_t i = 0;
    while (true) {

        old_question = current_question;
        current_question = (Node*) stack_pop (stack).value;

        if (!current_question) { break; }

        if (current_question->left_son == old_question) { entity_definition->traits_array [i] = { current_question->element.value, false, false }; }
        else                                            { entity_definition->traits_array [i] = { current_question->element.value, true , false }; }

        i++;
    }


    return SUCCESS;
}


Return_code  entity_definition_dtor  (Entity_definition* entity_definition) {

    if (!entity_definition) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    free (entity_definition->traits_array);


    return SUCCESS;
}


void  _ftree_graphdump  (Tree* tree, const char* file_name, const char* file, const char* func, int line, const char* additional_text) {

    assert ( (file_name) && (file) && (func) && (line > 0) );


    char file_path [MAX_COMMAND_LEN] = "";
    strcat (file_path, file_name);
    strcat (file_path, ".html");


    const char* file_mode = nullptr;
    if ( !strcmp (GLOBAL_graph_dump_num, "1")) { file_mode = "w"; }
    else                                       { file_mode = "a"; }


    FILE* dump_file = fopen (file_path, file_mode);
    if (dump_file == nullptr) { LOG_ERROR (FILE_ERR); return; }


    setvbuf (dump_file, NULL, _IONBF, 0);


    fprintf (dump_file, "<pre><h1>");
    fprintf (dump_file,"%s", additional_text);
    fprintf (dump_file, "</h1>");
    fprintf (dump_file, "<h2>Dumping tree at %s in function %s (line %d)...</h2>\n\n", file, func, line);


    if (!tree) { fprintf (dump_file, "Tree pointer is nullptr!\n\n"); return; }


    fprintf (dump_file, "this tree has name ");
    if (tree->debug_info.name != nullptr) { fprintf (dump_file, "%s ", tree->debug_info.name); }
    else                                  { fprintf (dump_file, "UNKNOWN NAME "); }
    fprintf (dump_file, "[%p]\n", tree->debug_info.adress);

    fprintf (dump_file, "it was created in file ");
    if (tree->debug_info.birth_file != nullptr) { fprintf (dump_file, "%s\n", tree->debug_info.birth_file); }
    else                                        { fprintf (dump_file, "UNKNOWN NAME\n"); }

    fprintf (dump_file, "in function ");
    if (tree->debug_info.birth_func != nullptr) { fprintf (dump_file, "%s ", tree->debug_info.birth_func); }
    else                                        { fprintf (dump_file, "UNKNOWN NAME "); }

    fprintf (dump_file, "(line %d)\n\n", tree->debug_info.birth_line);


    fprintf (dump_file, "tree is ");
    Tree_state tree_state = tree_damaged (tree);
    if (tree_state) { fprintf (dump_file, "damaged (damage code %u)\n", tree_state); }
    else            { fprintf (dump_file, "ok\n"); }


    if (tree->root) { fprintf (dump_file, "root [%p].\n\n", tree->root); }
    else            { fprintf (dump_file, "root [nullptr].\n\n"); }



    fprintf (dump_file, "nodes:\n\n\n");

    fprintf (dump_file, "pre-order:\n\n");
    _fdump_nodes (dump_file, tree, "pre");

    fprintf (dump_file, "\n\nin-order:\n\n");
    _fdump_nodes (dump_file, tree, "in");

    fprintf (dump_file, "\n\nas in save file:\n\n");
    _fprint_node (dump_file, tree->root, 0);


    fprintf (dump_file, "\n");


    char name [MAX_COMMAND_LEN] = tree_graph_file_name;
    strcat (name, GLOBAL_graph_dump_num);
    strcat (name, ".svg");
    fprintf (dump_file, "<img src=\"%s\" width=\"%zd\">", name, GRAPH_WIDTH);


    itoa ( (atoi (GLOBAL_graph_dump_num) + 1), GLOBAL_graph_dump_num, DEFAULT_COUNTING_SYSTEM); //incrementation of graph_dump_num


    fclose (dump_file);
}


void  _tree_show_graph_dump  (void) {

    char command [MAX_COMMAND_LEN] = "start ";
    strcat (command, tree_graph_dump_file_name);
    strcat (command, ".html");
    system (command);
}


void  _tree_generate_graph_describtion  (Tree* tree) {

    if (!tree) { return; }


    Tree_iterator tree_iterator = {};
    tree_iterator_ctor (&tree_iterator, tree, "pre");


    printf ("generating %s graph dump...\n", GLOBAL_graph_dump_num);


    char name [MAX_COMMAND_LEN] = "";
    strcat (name, tree_graph_describtion_file_name);
    strcat (name, GLOBAL_graph_dump_num);
    strcat (name, ".txt");

    FILE* graph_file = fopen (name, "w");
    if (graph_file == nullptr) { LOG_ERROR (FILE_ERR); return; }


    fprintf (graph_file, "digraph G {\n\n");


    fprintf (graph_file, "    bgcolor = \"#%s\"\n", tree_dump_bgclr);
    fprintf (graph_file, "    edge [minlen = 1, penwidth = 0.3, arrowsize = 0.3];\n    node [shape = record, style = rounded, fixedsize = true, height = 0.1, width = 0.6, fontsize = 3];\n\n");
    fprintf (graph_file, "    {rank = min; above_node [width = 3, style = invis];}\n\n");


    tree_generate_nodes_describtion (tree, graph_file);


    fprintf (graph_file, "\n\n    {rank = max; below_node[width = 3, style = invis]; }\n\n");

    fprintf (graph_file, "    above_node -> node0_0 [style = invis]; below_node -> node%zd_0 [style = invis];\n\n", tree_depth (tree));


    fprintf (graph_file, "}");


    fclose (graph_file);
}


void tree_generate_nodes_describtion (Tree* tree, FILE* file) {

    assert (tree && file);


    for (size_t depth = 0; depth <= tree_depth (tree); depth++) {

        size_t on_level_index   = 0;
        size_t next_level_index = 0;

        Tree_iterator tree_iterator = {};
        tree_iterator_ctor (&tree_iterator, tree, "in");



        fprintf (file, "    {\n");

        do {

            if (tree_iterator.depth != depth) { continue; }


            if (_isleaf (tree_iterator.current)) { 

                fprintf (file, "        node%zd_%zd [style = \"rounded, filled\", color=\"#%s\", ", depth, on_level_index, tree_dump_leafclr);
                fprintf (file, "label = \"%s\"];\n\n", tree_iterator.current->element.value);
                on_level_index += 1;
                continue;
            }


            fprintf (file, "        node%zd_%zd [style = \"rounded, filled\", color=\"#%s\", ", depth, on_level_index, tree_dump_nodeclr);
            fprintf (file, "label = \"%s\"];\n\n", tree_iterator.current->element.value);




                fprintf (file, "        node%zd_%zd -> node%zd_%zd\n", depth, on_level_index, depth + 1, next_level_index);
                next_level_index += 1;

                fprintf (file, "        node%zd_%zd -> node%zd_%zd\n", depth, on_level_index, depth + 1, next_level_index);
                next_level_index += 1;


            on_level_index += 1;
        }

        while (!tree_iterator_inc (&tree_iterator));

        fprintf (file, "    }\n");
    }
}


void  _tree_generate_graph  (void) {

    char command [MAX_COMMAND_LEN] = "dot -Tsvg ";
    strcat (command, tree_graph_describtion_file_name);
    strcat (command, GLOBAL_graph_dump_num);
    strcat (command, ".txt");
    strcat (command, " -o ");
    strcat (command, "work/");
    strcat (command, tree_graph_file_name);
    strcat (command, GLOBAL_graph_dump_num);
    strcat (command, ".svg");
    system (command);
}


size_t  tree_depth  (Tree* tree) {

    size_t depth = 0;


    Tree_iterator tree_iterator = {};
    tree_iterator_ctor (&tree_iterator, tree, "in");


    do {

            if (tree_iterator.depth > depth) { depth = tree_iterator.depth; }
        }
        while (!tree_iterator_inc (&tree_iterator));


    return depth;
}


Return_code  akinator_compare_mode  (void) {

    akinator_compare_mode_greetings ();


    char character1 [MAX_ANSWER_LEN + 1] = "";
    gets (character1);
    Entity_definition character1_definition = {nullptr, 0};

    char character2 [MAX_ANSWER_LEN + 1] = "";
    gets (character2);
    Entity_definition character2_definition = {nullptr, 0};



    Tree tree = {};
    TREE_CTOR (&tree, nullptr);
    tree_read (&tree);


    try ( entity_definition_builder (&character1_definition, &tree, character1) );
    try ( entity_definition_builder (&character2_definition, &tree, character2) );


    for (size_t i = 0; i < character1_definition.num_traits; i++) {
    
        for (size_t j = 0; j < character2_definition.num_traits; j++) {
        
            if ( character1_definition.traits_array[i].has_it == character2_definition.traits_array[j].has_it && 
                !strcmp (character1_definition.traits_array[i].trait, character2_definition.traits_array[j].trait)) {

                    character1_definition.traits_array[i].have_both = true;
                    character2_definition.traits_array[j].have_both = true;
            }
        }
    }

    printf ("\n\n--------------------------------------\n");
    printf ("both %s and %s:\n\n", character1, character2);

    for (size_t i = 0; i < character1_definition.num_traits; i++) {
    
        if (!character1_definition.traits_array[i].have_both) { continue; }


        if (character1_definition.traits_array[i].has_it) { printf ("do "); }
        else                                              { printf ("don't "); }

        printf ("%s\n\n", character1_definition.traits_array[i].trait);
    }


    if (character1_definition.num_traits) {

        printf ("but %s:\n\n", character1);
        for (size_t i = 0; i < character1_definition.num_traits; i++) {

            if (character1_definition.traits_array[i].have_both) { continue; }

            if (character1_definition.traits_array[i].has_it) { printf ("does "); }
            else                                              { printf ("doesn't "); }

            printf ("%s\n\n", character1_definition.traits_array[i].trait);
        }
    }


    if (character2_definition.num_traits) {

        printf ("and %s:\n\n", character2);
        for (size_t i = 0; i < character2_definition.num_traits; i++) {

            if (character2_definition.traits_array[i].have_both) { continue; }

            if (character2_definition.traits_array[i].has_it) { printf ("does "); }
            else                                              { printf ("doesn't "); }

            printf ("%s\n\n", character2_definition.traits_array[i].trait);
        }
    }

    entity_definition_dtor (&character1_definition);
    entity_definition_dtor (&character2_definition);


    return SUCCESS;
}


Return_code  entity_definition_builder (Entity_definition* entity_definition, Tree* tree, char* character) {

    if (!entity_definition || !tree || !character) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    Tree_iterator tree_iterator = {};
    tree_iterator_ctor (&tree_iterator, tree, "pre");

    do {

        if (!_isleaf (tree_iterator.current)) { continue; }

        if (!stricmp (character, tree_iterator.current->element.value)) {

            entity_definition_ctor (entity_definition, tree_iterator.node_stack);
        }
    }
    while (!tree_iterator_inc (&tree_iterator));


    tree_iterator_dtor (&tree_iterator);


    if (entity_definition->num_traits == 0) { printf ("\n\nunfortunately, i don't know who %s is\n\n", character); return BAD_ARGS; }


    for (size_t i = 0; i < entity_definition->num_traits; i++) {
    
        printf ("%s - %d %s\n", character, entity_definition->traits_array [i].has_it, entity_definition->traits_array [i].trait);
    }
    
    
    return SUCCESS;
}


void  akinator_compare_mode_greetings  (void) {

    printf ("\n");
    printf ("--------------------------------------\n");
    printf ("Enter two entities, i'll compare them!\n");
    printf ("--------------------------------------\n\n");
}


Return_code akinator_dump_mode (void) {

    Tree tree = {};
    try (TREE_CTOR (&tree, nullptr));
    try (tree_read (&tree));


    printf ("\n");
    FTREE_GRAPHDUMP (&tree, "Here is your dump:");
    printf ("\n");


    try (tree_dtor (&tree));


    return SUCCESS;
}


Return_code akinator_start (void) {

    akinator_greetings ();

    Akinator_mode mode = get_akinator_mode ();

    switch (mode) {

        case AK_GUESS:

            try (akinator_guess_mode ());
            break;

        case AK_DEFINE:

            try (akinator_define_mode ());
            break;

        case AK_COMPARE:

            try (akinator_compare_mode ());
            break;

        case AK_DUMP:

            try (akinator_dump_mode ());
            break;

        case AK_UNKNOWN:

            return SUCCESS;

        default:

            return SUCCESS;
    }


    return SUCCESS;
}


void akinator_greetings (void) {

    printf ("\nWelcome to the akinator.\nChoose one of the following modes:\n\n");
    printf ("[g]uess\n\n");
    printf ("[d]efine\n\n");
    printf ("[c]ompare\n\n");
    printf ("[b]ase dump\n\n");
}

Akinator_mode get_akinator_mode (void) {

    char mode    [MAX_COMMAND_LEN + 1] = "";
    char entered [MAX_ANSWER_LEN + 1] = "";
    char should_be_blank [2] = "";

    gets (entered);
    sscanf (entered, "%s %1s", mode, should_be_blank);

    if (strcmp (should_be_blank, "")) { return reget_akinator_mode (); }

    if (!stricmp (mode, "g") || !stricmp (mode, "guess"))                                    { return AK_GUESS; }
    if (!stricmp (mode, "d") || !stricmp (mode, "define"))                                   { return AK_DEFINE; }
    if (!stricmp (mode, "c") || !stricmp (mode, "compare"))                                  { return AK_COMPARE; }
    if (!stricmp (mode, "b") || !stricmp (mode, "base dump") || !stricmp (mode, "basedump")) { return AK_DUMP; }


    return reget_akinator_mode ();
}

Akinator_mode reget_akinator_mode (void) {

    printf ("\n\nwrong input, please re-enter akinator mode\n\n");


    return get_akinator_mode ();
}

