//#define DEBUG_AKINATOR

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "head.h"
#include "log.h"
#include "color_print.h"
#include "stack.h"

#ifdef DEBUG_AKINATOR
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

struct Node_t* new_node  (char* data, struct Node_t* parent);

int print_tree_preorder  (struct Node_t* root, FILE* filename, int level);

int print_tree_postorder (struct Node_t* root);

int print_tree_inorder   (struct Node_t* root);

int delete_sub_tree      (struct Node_t* node);

int buffer_dtor (struct Buffer_t* buffer);

void print_tree_preorder_for_file (struct Node_t* root, FILE* filename);

int make_graph (struct Node_t* root);

struct Node_t* akinator_game (struct Node_t* node);

struct Node_t* iterativly_insert (struct Node_t* node, char* data);

struct Node_t* add_info (struct Node_t* node);

void clean_buffer(void);

char* get_and_prepare_string (const char* question, ...);

int write_database (struct Node_t* root);

struct Node_t* read_node (int level, struct Buffer_t* buffer, struct Node_t* parent);

struct Node_t* read_database (FILE* filename, struct Buffer_t* buffer);

void dump_in_log_file (struct Node_t* node, const char* reason);

void create_definition (struct Node_t* node);

struct Node_t* find_node (const char* object, struct Node_t* node);

void print_definition (struct Node_t* node, struct stack_str* stack);

// int write_log_file (struct Node_t* root, const char* reason_bro/* in c++ " = doesn't_care_bro"*/);

int main (void)
{
    FILE* database = fopen ("database.txt", "r");
    struct Buffer_t buffer = {};

    struct Node_t* root = read_database (database, &buffer);
    if (root == NULL)
        return 1;

    ON_DEBUG ( printf ("GNU = %d.%d\n", __GNUC__, __GNUC_MINOR__);
               printf ("%ld", _POSIX_C_SOURCE);                    )

    open_log_file ("../build/dump.html");

    dump_in_log_file (root, "before insert");

    ON_DEBUG ( printf ("\npreorder:\n");
               print_tree_preorder  (root, stdout, 0);

               printf ("\npostorder: ");
               print_tree_postorder (root);

               printf ("\ninorder: ");
               print_tree_inorder   (root);
               printf ("\n");                           )

    int loop = 1;
    while (loop)
    {
        printf ("what do you wanna doing?\n");
        printf ("[a]kinator game, [d]efinition, [q]uit and write to database, [Q]uit\n");

        int answer_for_mode = getchar();
        clean_buffer ();

        switch (answer_for_mode)
        {
            case 'a':
            {
                akinator_game (root);
                break;
            }

            case 'd':
            {
                create_definition (root);
                break;
            }

            case 'q':
            {
                write_database (root);
                loop = 0;
                break;
            }

            case 'Q':
            {
                loop = 0;
                break;
            }

            default:
            {
                printf ("bro.... put the right mode\n");
                break;
            }
        }
    }

    dump_in_log_file (root, "end of programm");

    close_log_file ();

    delete_sub_tree (root);

    buffer_dtor (&buffer);

    return 0;
}

void dump_in_log_file (struct Node_t* node, const char* reason)
{
    make_graph (node);
    write_log_file (node, reason);
}

struct Node_t* read_database (FILE* file, struct Buffer_t* buffer)
{
    struct stat st = {};
    fstat (fileno (file), &st);
    long file_size = st.st_size;

    buffer->buffer_ptr = calloc ( (size_t) file_size + 1, sizeof(buffer->buffer_ptr[0]));

    ON_DEBUG ( printf ("\n\n\n\nbuffer->buffer_ptr = [%p]\n\n\n\n", buffer->buffer_ptr); )

    size_t count = fread (buffer->buffer_ptr, sizeof(buffer->buffer_ptr[0]), (size_t) file_size, file);
    if ((long) count != file_size)
        printf ("count = %zu != file_size = %ld", count, file_size);

    fclose (file);

    buffer->current_ptr = buffer->buffer_ptr;

    return read_node (0, buffer, NULL);
}

#define INDENT printf ("%*s", level*2, "")

struct Node_t* read_node (int level, struct Buffer_t* buffer, struct Node_t* parent)
{
    ON_DEBUG ( printf ("\n"); )
    ON_DEBUG ( INDENT; printf ("Starting read_node(). Cur = %.40s..., [%p]. buffer_ptr = [%p]\n", buffer->current_ptr,  buffer->current_ptr, buffer->buffer_ptr); )

    int n = -1;
    sscanf (buffer->current_ptr, " { %n", &n);
    if (n < 0) { ON_DEBUG ( INDENT; printf ("No '{' found. Return NULL.\n"); ) return NULL; }

    buffer->current_ptr += n;

    ON_DEBUG ( INDENT; printf ("Got an '{'. Creating a node. Cur = %.40s..., [%p]. buffer_ptr = [%p]\n", buffer->current_ptr,  buffer->current_ptr, buffer->buffer_ptr); )

    struct Node_t* node = {};

    if (level == 0)
        node = new_node (NULL, NULL);
    else
        node = new_node (NULL, parent);

    n = -1;
    int bgn = 0;
    int end = 0;
    sscanf (buffer->current_ptr, " \"%n%*[^\"]%n\" %n", &bgn, &end, &n);
    if (n < 0) { ON_DEBUG ( INDENT; printf ("No DATA found. Return NULL.\n"); ) return NULL; }

    *(buffer->current_ptr + end) = '\0';
    node->data = buffer->current_ptr + bgn;

    ON_DEBUG ( INDENT; printf ("Got a NAME: '%s'. Cur = %.40s..., [%p]. buffer_ptr = [%p]\n", node->data, buffer->current_ptr, buffer->current_ptr, buffer->buffer_ptr); )

    buffer->current_ptr += n;

    ON_DEBUG ( INDENT; printf ("Shifted CURRENT_PTR: '%s'. Cur = %.40s..., [%p]. buffer_ptr = [%p]\n", node->data, buffer->current_ptr, buffer->current_ptr, buffer->buffer_ptr); )

    n = -1;
    char chr = '\0';
    sscanf (buffer->current_ptr, " %c %n", &chr, &n);
    if (n < 0) { ON_DEBUG ( INDENT; printf ("No ending symbol (1) found. Return NULL.\n"); ) return NULL; }

    if (chr == '}')
    {
        buffer->current_ptr += n;

        ON_DEBUG ( INDENT; printf ("Got a '}', SHORT Node END (data = '%s'). Return node. Cur = %.40s..., [%p]. buffer_ptr = [%p]\n", node->data, buffer->current_ptr, buffer->current_ptr, buffer->buffer_ptr); )

        return node;
    }

    ON_DEBUG ( INDENT; printf ("'}' NOT found. Supposing a left/right subtree. Reading left node. Cur = %.40s..., [%p]. buffer_ptr = [%p]\n", buffer->current_ptr, buffer->current_ptr, buffer->buffer_ptr); )

    node->left = read_node (level + 1, buffer, node);

    ON_DEBUG ( INDENT; printf ("\n" "LEFT subtree read. Data of left root = '%s'\n\n", node->left->data); )

    ON_DEBUG ( printf ("Reading right node. Cur = %.40s...\n", buffer->current_ptr); )

    node->right = read_node (level + 1, buffer, node);

    ON_DEBUG ( INDENT; printf ("\n" "RIGHT subtree read. Data of right root = '%s'\n", node->right->data); )

    chr = '\0';
    sscanf (buffer->current_ptr, " %c %n", &chr, &n);
    if (n < 0) { ON_DEBUG ( INDENT; printf ("No ending symbol (2) found. Return NULL.\n"); ) return NULL; }

    if (chr == '}')
    {
        buffer->current_ptr += n;

        ON_DEBUG ( INDENT; printf ("Got a '}', FULL Node END (data = '%s'). Return node. Cur = %.40s..., [%p]. buffer_ptr = [%p]\n", node->data, buffer->current_ptr, buffer->current_ptr, buffer->buffer_ptr); )

        return node;
    }

    ON_DEBUG ( INDENT; printf ("Does NOT get '}'. Syntax error. Return NULL. Cur = %.20s..., [%p]. buffer_ptr = [%p]\n", buffer->current_ptr, buffer->current_ptr, buffer->buffer_ptr); )

    return NULL;
}

struct Node_t* new_node (char* data, struct Node_t* parent)
{
    struct Node_t* node = (struct Node_t*) calloc (1, sizeof(*node));
    assert (node); //FIXME - check NULL pointers

    node->data  = data;
    node->left  = NULL;
    node->right = NULL;

    node->shoot_free = 0;

    node->parent = parent;

    return node;
}

#if (__GNUC__ < 13 && __GNUC_MINOR__ < 2)
    #error error subsystem please reinstall subsystem.
#endif

char* get_and_prepare_string (const char* question, ...)
{
    va_list args;
    va_start (args, question);

    vprintf (question, args);

    va_end (args);

    char* object = NULL;
    size_t size_max = 0;

    getline (&object, &size_max, stdin);

    size_t size = strlen (object);

    if (size == 0)
        return NULL;

    if (object[size - 1] == '\n')
        object[size - 1] =  '\0';

    printf ("str = \"%s\"\n", object);

    return object;
}

struct Node_t* add_info (struct Node_t* node)
{
    struct Node_t* ptr_left  = new_node ("0", node); //FIXME - check NULL pointers
    struct Node_t* ptr_right = new_node ("0", node);

    node->left  = ptr_left;
    node->right = ptr_right;

    char* new_object  = get_and_prepare_string ("who tf is this?\n");

    ptr_left->data = new_object;
    ptr_left->shoot_free = 1;
    ptr_right->data = node->data;

    if (node->shoot_free == 1)
        ptr_right->shoot_free = 1;

    char* diff_object = get_and_prepare_string ("whats the difference between %s and %s. %s is...\n",
                                                ptr_left->data, node->data, ptr_left->data);

    node->data = diff_object;
    node->shoot_free = 1;

    dump_in_log_file (node, "adding a node");

    return node;
}

void clean_buffer(void)
{
    while((getchar()) != '\n') {;}
}

struct Node_t* akinator_game (struct Node_t* node)
{
    while (node && node->left && node->right)
    {
        printf ("\"%s\"?\n", node->data);
        printf ("[y/n]\n");

        int answer = getchar();
        clean_buffer ();       //TODO - func = getchar + clean_buffer

        if      (answer == 'y') node = node->left;
        else if (answer == 'n') node = node->right;
        else printf ("it isn't 'y' or 'n' --- error.\n");
    }

    printf ("Is it the RIGHT answer: \"%s\"?\n[y/n]\n", node->data);

    int final_answer = getchar();
    clean_buffer ();

    if (final_answer == 'y')
    {
        printf ("lol, it was so easy...\n");
        return node;
    }

    add_info (node);

    return node;
}

struct Node_t* iterativly_insert (struct Node_t* node, char* data)
{
    while (true)
    {
        if (data < node->data)
        {
            if (node->left)
                node = node->left;
            else
                return node->left = new_node (data, node);
        }
        else
        {
            if (node->right)
                node = node->right;
            else
                return node->right = new_node (data, node);
        }
    }
}

int delete_sub_tree (struct Node_t* node)
{
    node->parent = NULL;

    if (node->left)  delete_sub_tree (node->left);

    if (node->right) delete_sub_tree (node->right);

    if (node->shoot_free == 1)
        free (node->data);

    node->shoot_free = 0;

    free (node);

    return 0;
}

int buffer_dtor (struct Buffer_t* buffer)
{
    buffer->current_ptr = NULL;

    free (buffer->buffer_ptr);

    return 0;
}

int write_database (struct Node_t* root)
{
    FILE* database = fopen ("database.txt", "w");

    assert (database); //FIXME

    print_tree_preorder (root, database, 0);

    fclose (database);

    return 0;
}

int print_tree_preorder (struct Node_t* root, FILE* filename, int level)
{
    if (!root)
        return 1; //FIXME

    fprintf (filename, "%*s{ ", level * 4, "");

    fprintf (filename, "\"%s\" ",  root->data);

    if (root->left)
        fprintf (filename, "\n");

    if (root->left)  print_tree_preorder (root->left,  filename, level + 1);

    if (root->right) print_tree_preorder (root->right, filename, level + 1);

    fprintf (filename, "%*s} \n", (root->left) ? level * 4 : 0, "");

    return 0;
}

int print_tree_postorder (struct Node_t* root)
{
    if (!root)
        return 1; //FIXME

    printf ("{ ");

    if (root->left)  print_tree_postorder (root->left);

    if (root->right) print_tree_postorder (root->right);

    printf ("\"%s\" ",  root->data);

    printf ("} ");

    return 0;
}

int print_tree_inorder (struct Node_t* root)
{
    if (!root)
        return 1; //FIXME

    printf ("{ ");

    if (root->left)  print_tree_inorder (root->left);

    printf ("\"%s\" ",  root->data);

    if (root->right) print_tree_inorder (root->right);

    printf ("} ");

    return 0;
}

int make_graph (struct Node_t* root)
{
    assert (root);

    FILE* graph_file = fopen ("../build/graph_tree.dot", "w");
    if (graph_file == NULL)
    {
        printf("ERROR open graph_file\n");
        return 1;
    }

    fprintf (graph_file, "digraph\n{\n");

    fprintf (graph_file, "bgcolor=\"transparent\"\n");

    //fprintf (graph_file, "rankdir = \"LR\";\n");

    print_tree_preorder_for_file (root, graph_file);
    fprintf (graph_file, "\n");

    fprintf (graph_file, "}");
    fprintf (graph_file, "\n");

    fclose  (graph_file);

    return 0;
}

void print_tree_preorder_for_file (struct Node_t* root, FILE* filename)
{
    if (!root)
        return ; //FIXME

    if (root->shoot_free == 0)
        fprintf (filename, "node%p [shape=Mrecord; label = \" { ADDR = [%p] | data = %3s | shoot_free = %d | parent = [%p] | { left = [%p] | right = [%p] } }\"; style = filled; fillcolor = \"#FFFFD0\"];\n",
             root, root, root->data, root->shoot_free, root->parent, root->left, root->right);
    else
        fprintf (filename, "node%p [shape=Mrecord; label = \" { ADDR = [%p] | data = %3s | shoot_free = %d | parent = [%p] | { left = [%p] | right = [%p] } }\"; style = filled; fillcolor = \"#FFE0E0\"];\n",
             root, root, root->data, root->shoot_free, root->parent, root->left, root->right);

    if (root->left)
        fprintf (filename, "node%p -> node%p\n;", root, root->left);

    if (root->right)
        fprintf (filename, "node%p -> node%p\n;", root, root->right);

    if (root->left)  print_tree_preorder_for_file (root->left , filename);

    if (root->right) print_tree_preorder_for_file (root->right, filename);
}

void create_definition (struct Node_t* node)
{
    char* object = NULL;
    size_t size_max = 0;

    printf ("enter the object: ");
    getline (&object, &size_max, stdin);

    size_t size = strlen (object);
    assert (size); //FIXME -

    if (object[size - 1] == '\n')
        object[size - 1] =  '\0';

    fprintf (stderr, "%s\n", object);

    struct Node_t* our_node = find_node (object, node); //FIXME - check NULL pointers
    fprintf (stderr, "our_node = [%p]  our_node->data = %s\n", our_node, our_node->data);
    //if (our_node == NULL)
    our_node->data = object;
    our_node->shoot_free = 1;

    fprintf (stderr, "object = %s, our_node->data = %s   \n", object, our_node->data);

    struct stack_str stack = {};
    stack_ctor (&stack, 10);

    print_definition (our_node, &stack);
}

struct Node_t* find_node (const char* object, struct Node_t* node)
{
    assert (node && "node is null");

    ON_DEBUG ( fprintf (stderr, "find_node starting...  node->data = '%s'    object = '%s'\n\n", node->data, object); )

    if (strcmp (object, node->data) == 0)
        return node;

    if (node->left)
    {
        ON_DEBUG ( fprintf (stderr, "going left: node->left->data = '%s'\n", node->left->data); )
        struct Node_t* node_left = find_node (object, node->left);
        if (node_left != NULL)
            return node_left;
    }

    if (node->right)
    {
        ON_DEBUG ( fprintf (stderr, "going right: node->right->data = '%s'\n", node->right->data); )
        struct Node_t* node_right = find_node (object, node->right);
        if (node_right != NULL)
            return node_right;
    }

    return NULL;
}

void print_definition (struct Node_t* node, struct stack_str* stack)
{
    fprintf (stderr, "\n\nDEFINITION: "YELLOW_TEXT("%s is "), node->data);

    while (true)
    {
        stack_push (stack, node);

        node = node->parent;

        if (node->parent == NULL)
            break;
    }

    stack_push (stack, node);

    int i = stack->size - 1;

    while (true)
    {
        ON_DEBUG ( stack_dump (stack, __FILE__, __LINE__, __FUNCTION__); )

        struct Node_t* node_n   = stack_look (stack, i); //FIXME - check NULL pointers

        ON_DEBUG ( fprintf (stderr, "i = %d\n", i);
                   stack_dump (stack, __FILE__, __LINE__, __FUNCTION__);
                   fprintf (stderr, "\nnode_n = [%p]\n", node_n);
                   fprintf (stderr, "node_n->right = [%p]\n", node_n->right); )

        struct Node_t* node_n_1 = stack_look (stack, i - 1); //FIXME - check NULL pointers

        ON_DEBUG ( fprintf (stderr, "i = %d\n", i);
                   stack_dump (stack, __FILE__, __LINE__, __FUNCTION__);
                   fprintf (stderr, "node_n_1 = [%p]\n",  node_n_1);
                   fprintf (stderr, "node_n_1->right = [%p]\n", node_n_1->right); )

        ON_DEBUG ( stack_dump (stack, __FILE__, __LINE__, __FUNCTION__);

                   fprintf (stderr, "<<< i = %d >>> if (node_n->right = [%p, %s] =="
                                                       "node_n_1      = [%p, %s] \n\n",
                                                     i, node_n->right, node_n->data,
                                                        node_n_1,      node_n_1->data); )

        if (node_n->right == node_n_1)
            fprintf (stderr, YELLOW_TEXT("not a "));

        fprintf (stderr, YELLOW_TEXT("%s%c "), node_n->data, (i != 1) ? ',' : '.');

        if (--i == 0)
            break;
    }

    fprintf (stderr, "\n\n");

    stack_dtor (stack);
}
