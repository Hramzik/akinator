

#include "../lib/stack.hpp"
#include "../lib/logs.hpp"

#include "../headers/tree.hpp"


//static char GLOBAL_graph_dump_num [100] = "1";


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

/*
Return_code  tree_dtor  (Tree* tree) {

    ASSERT_TREE_OK (tree);


    free (tree->root);
    tree->root = nullptr;


    TREE_AFTER_OPERATION_DUMPING (tree);


    return SUCCESS;
}
*/

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


    if (tree->root) { fprintf (dump_file, "root [%p]:\n", tree->root); }
    else            { fprintf (dump_file, "root [nullptr]:\n"); }


    //print nodes


    fprintf (dump_file, "\n");


    fclose (dump_file);


    first_time_dumping = false;
}


Return_code tree_iterator_inc (Tree_iterator* tree_iterator) {

    if (!tree_iterator) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }

    if (!tree_iterator->node_stack.size) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; } // already at last node



    stack_pop (&tree_iterator->node_stack);

    if (!tree_iterator->current->right_son) {

        tree_iterator->current = (Node*) stack_pop (&tree_iterator->node_stack).value;
        stack_push (&tree_iterator->node_stack, tree_iterator->current);
        tree_iterator->depth -= 1;
        tree_iterator->index += 1;
        return SUCCESS;
    }



    tree_iterator->current = tree_iterator->current->right_son;
    stack_push (&tree_iterator->node_stack, tree_iterator->current);
    tree_iterator->depth += 1;

    while (tree_iterator->current->left_son) {

        tree_iterator->current = tree_iterator->current->left_son;
        stack_push (&tree_iterator->node_stack, tree_iterator->current);
        tree_iterator->depth += 1;
    }

    tree_iterator->index += 1;
    return SUCCESS;
}


Return_code tree_iterator_ctor (Tree_iterator* tree_iterator, Tree* tree) {

    if (!tree_iterator) { LOG_ERROR (BAD_ARGS); return BAD_ARGS; }


    tree_iterator->current = tree->root;
    stack_push (&tree_iterator->node_stack, tree_iterator->current);
    tree_iterator->depth = 0;


    while (tree_iterator->current->left_son) {

        tree_iterator->current = tree_iterator->current->left_son;
        stack_push (&tree_iterator->node_stack, tree_iterator->current);
        tree_iterator->depth += 1;
    }


    tree_iterator->index = 1;


    return SUCCESS;
}
/*
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


    fprintf (dump_file, "tree is");
    if (tree->is_linearized) { fprintf (dump_file,    " linearized\n"); }
    else                     { fprintf (dump_file, "n't linearized\n"); }


    fprintf (dump_file, "root     %d\n",   1);
    fprintf (dump_file, "size     %d\n",   tree->size);
    fprintf (dump_file, "capacity %d\n\n", tree->capacity);
    if (tree->root) { fprintf (dump_file, "nodes [%p]:\n", tree->root); }
    else            { fprintf (dump_file, "nodes [nullptr]:\n"); }


    for (int i = 0; i < tree->capacity; i++) {

        if (i == 0)                         { fprintf (dump_file, "(root)     "); }
        else if (tree->root [i].prev != -1) { fprintf (dump_file, "(in)       "); }
        else                                { fprintf (dump_file, "(out)      "); }

        fprintf (dump_file, "[%2d] ", i);
        fprintf (dump_file, "%8.2lf     ", tree->root [i].element.value);

        if (tree->root [i].element.poisoned) { fprintf (dump_file, "    (poisoned)     "); }
        else                                 { fprintf (dump_file, "(not poisoned)     "); }

        fprintf (dump_file, "next - [%2d],     prev - [%2d]\n", tree->root [i].next, tree->root [i].prev);
    }


    fprintf (dump_file, "\nfree elements:\n");
    for (int i = tree->top_free_ind; i != -1; i = tree->root [i].next) {
    
        if (tree->root [i].prev != -1) { fprintf (dump_file, "(in)       "); }
        else                           { fprintf (dump_file, "(out)      "); }

        fprintf (dump_file, "[%2d] ", i);
        fprintf (dump_file, "%8.2lf     ",  tree->root [i].element.value);

        if (tree->root [i].element.poisoned) { fprintf (dump_file, "    (poisoned)     "); }
        else                                 { fprintf (dump_file, "(not poisoned)     "); }

        fprintf (dump_file, "next - [%2d],     prev - [%2d]\n", tree->root [i].next, tree->root [i].prev);
    }


    fprintf (dump_file, "\n");


    char name [MAX_COMMAND_LEN] = tree_graph_file_name;
    strcat (name, GLOBAL_graph_dump_num);
    strcat (name, ".svg");
    fprintf (dump_file, "<img src=\"%s\" width=\"%zd\">", name, GRAPH_WIDTH);


    itoa ( (atoi (GLOBAL_graph_dump_num) + 1), GLOBAL_graph_dump_num, DEFAULT_COUNTING_SYSTEM); //incrementation of graph_dump_num


    fclose (dump_file);
}


void  tree_show_graph_dump  (void) {

    char command [MAX_COMMAND_LEN] = "start ";
    strcat (command, tree_graph_dump_file_name);
    strcat (command, ".html");
    system (command);
}

void  tree_generate_graph_describtion  (Tree* tree) {

    if (!tree) { return; }


    printf ("generating %s graph dump...\n", GLOBAL_graph_dump_num);


    char name [MAX_COMMAND_LEN] = "";
    strcat (name, tree_graph_describtion_file_name);
    strcat (name, GLOBAL_graph_dump_num);
    strcat (name, ".txt");

    FILE* graph_file = fopen (name, "w");
    if (graph_file == nullptr) { LOG_ERROR (FILE_ERR); return; }


    fprintf (graph_file, "digraph G {\n\n");


    fprintf (graph_file, "    ranksep = 0.5; splines = ortho\n    bgcolor = \"#%s\"\n", tree_dump_bgclr);
    fprintf (graph_file, "    edge [minlen = 3, penwidth = 3];\n    node [shape = record, style = rounded, fixedsize = true, height = 1, width = 2, fontsize = 10];\n\n");
    fprintf (graph_file, "    {rank = min; above_node [width = 3, style = invis];}\n\n");
    fprintf (graph_file, "    {rank = same;\n");

    for (int i = 0; i < tree->capacity; i++) {

        if (tree->root [i].prev != -1) {

            fprintf (graph_file, "        node%d [style = \"rounded, filled\", color=\"#%s\", ", i, tree_dump_nodeclr);
        }

        else {

            fprintf (graph_file, "        node%d [style = \"rounded, filled\", color=\"#%s\", ", i, tree_dump_freeclr);
        }

        fprintf (graph_file, "label = \"{[%i] | %.2lf | next = %d | prev = %d}\"];\n", i, tree->root [i].element.value, tree->root [i].next, tree->root [i].prev);
    }

    fprintf (graph_file, "    }\n\n    {rank = max; below_node[width = 3, style = invis]; }\n\n");

    fprintf (graph_file, "    above_node -> node0 [style = invis]; below_node -> node0 [style = invis];\n\n");


    for (int i = 0; i < tree->capacity; i++) {

        if (i + 1 != tree->capacity) {

            fprintf (graph_file, "    node%d -> node%d [style = invis, weight = 5]\n", i, i + 1);
        }

        if (tree->root [i].next != -1) {

            if (tree->root [i].prev != -1) {
            
                fprintf (graph_file, "    node%d -> node%d [color = \"#%s\"]\n", i, tree->root [i].next, tree_dump_arrwclr);
            }

            else {

                fprintf (graph_file, "    node%d -> node%d [color = \"#%s\"]\n", i, tree->root [i].next, tree_dump_freearrwclr);
            }
        }
    }


    fprintf (graph_file, "}");


    fclose (graph_file);
}


void  tree_generate_graph  (void) {

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

*/