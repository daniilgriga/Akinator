#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include "head.h"

struct Node_t* new_node  (char* data);

int print_tree_preorder  (struct Node_t* root, FILE* filename, int level);

int print_tree_postorder (struct Node_t* root);

int print_tree_inorder   (struct Node_t* root);

int delete_sub_tree (struct Node_t* node);

void print_tree_preorder_for_file (struct Node_t* root, FILE* filename);

char* graph_dump (struct Node_t* root);

FILE* open_log_file (const char* filename);

struct Node_t* akinator_game (struct Node_t* node);

struct Node_t* iterativly_insert (struct Node_t* node, char* data);

struct Node_t* add_info (struct Node_t* node);

void clean_buffer(void);

char* get_and_prepare_string (const char* question);

int write_database (struct Node_t* root);

struct Node_t* read_node (int level);

struct Node_t* read_database (FILE* filename);

int write_log_file (struct Node_t* root, const char* reason_bro/* in c++ " = doesn't_care_bro"*/);

char* CURRENT_PTR = NULL;

FILE* LOG_FILE = NULL;

int main (void)
{
    FILE* database = fopen ("database.txt", "r");

    struct Node_t* root = read_database (database);

    printf ("[%p]\n", root);

    printf ("GNU = %d.%d\n", __GNUC__, __GNUC_MINOR__);

    printf ("%ld", _POSIX_C_SOURCE);
    open_log_file ("../build/dump.html");

    write_log_file (root, "before insert");

    printf ("\npreorder: ");
    print_tree_preorder  (root, stdout, 0);

    printf ("\npostorder: ");
    print_tree_postorder (root);

    printf ("\ninorder: ");
    print_tree_inorder   (root);
    printf ("\n");

    int loop = 1;
    while (loop)
    {
        printf ("what do you wanna doing?\n[a]kinator game, [q]uit and write to database, [Q]uit\n");
        int answer_for_mode = getchar();
        clean_buffer ();

        switch (answer_for_mode)
        {
            case 'a':
            {
                akinator_game (root);
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
                printf ("bro....");
                break;
            }
        }
    }

    write_log_file (root, "end of programm");

    fclose (LOG_FILE);

    delete_sub_tree (root);

    return 0;
}

struct Node_t* read_database (FILE* file)
{
    struct stat st = {};
    fstat (fileno (file), &st);
    long file_size = st.st_size;

    char* buffer = calloc ( (size_t) file_size + 1, sizeof(buffer[0]));

    size_t count = fread (buffer, sizeof(buffer[0]), (size_t) file_size, file);
    if ((long) count != file_size)
        printf ("count = %zu != file_size = %ld", count, file_size);

    fclose (file);

    CURRENT_PTR = buffer;

    printf ("CURRENT_PTR = [%p]\n", CURRENT_PTR);

    return read_node (0);
}

#define INDENT printf ("%*s", level*2, "")

struct Node_t* read_node (int level)
{
    printf ("\n");
    INDENT; printf ("Starting read_node(). Cur = %.40s...\n", CURRENT_PTR);

    int n = -1;
    sscanf (CURRENT_PTR, " { %n", &n);
    if (n < 0) { INDENT; printf ("No '{' found. Return NULL.\n"); return NULL; }

    CURRENT_PTR += n;

    INDENT; printf ("Got an '{'. Creating a node. Cur = %.40s...\n", CURRENT_PTR);

    struct Node_t* node = new_node (NULL);

    n = -1;
    char data[50] = "";
    sscanf (CURRENT_PTR, " \"%[^\"]\" %n", data, &n);
    if (n < 0) { INDENT; printf ("No DATA found. Return NULL.\n"); return NULL; }

    CURRENT_PTR += n;

    INDENT; printf ("Got a NAME: '%s'. Cur = %.40s...\n", data, CURRENT_PTR);

    node->data = strdup (data);

    n = -1;
    char chr = '\0';
    sscanf (CURRENT_PTR, " %c %n", &chr, &n);
    if (n < 0) { INDENT; printf ("No ending symbol (1) found. Return NULL.\n"); return NULL; }

    if (chr == '}')
        {
        CURRENT_PTR += n;

        INDENT; printf ("Got a '}', SHORT Node END (data = '%s'). Return node. Cur = %.40s...\n", data, CURRENT_PTR);

        return node;
        }

    INDENT; printf ("'}' NOT found. Supposing a left/right subtree. Reading left node. Cur = %.40s...\n", CURRENT_PTR);

    node->left = read_node (level+1);

    INDENT; printf ("\n" "LEFT subtree read. Data of left root = '%s'\n\n", node->left->data);

    printf ("Reading right node. Cur = %.40s...\n", CURRENT_PTR);

    node->right = read_node (level+1);

    INDENT; printf ("\n" "RIGHT subtree read. Data of right root = '%s'\n", node->right->data);

    chr = '\0';
    sscanf (CURRENT_PTR, " %c %n", &chr, &n);
    if (n < 0) { INDENT; printf ("No ending symbol (2) found. Return NULL.\n"); return NULL; }

    if (chr == '}')
        {
        CURRENT_PTR += n;

        INDENT; printf ("Got a '}', FULL Node END (data = '%s'). Return node. Cur = %.40s...\n", data, CURRENT_PTR);

        return node;
        }

    INDENT; printf ("Does NOT get '}'. Syntax error. Return NULL. Cur = %.20s...\n", CURRENT_PTR);

    return NULL;
}

struct Node_t* new_node (char* data)
{
    struct Node_t* node = (struct Node_t*) calloc (1, sizeof(*node));
    assert (node); //FIXME

    node->data  = data;
    node->left  = NULL;
    node->right = NULL;

    node->parent = NULL;

    return node;
}

#if (__GNUC__ < 13 && __GNUC_MINOR__ < 2)
    #error error subsystem please reinstall subsystem.
#endif

char* get_and_prepare_string (const char* question)
{
    printf ("%s", question);

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
    struct Node_t* ptr_left  = new_node ("0");
    struct Node_t* ptr_right = new_node ("0");

    node->left  = ptr_left;
    node->right = ptr_right;

    char* new_object  = get_and_prepare_string ("who tf is this?\n");

    ptr_left->data = new_object;
    ptr_right->data = node->data;

    char* diff_object = get_and_prepare_string ("how are they different?\n");

    node->data = diff_object;

    write_log_file (node, "adding a node");

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
                return node->left = new_node (data);
        }
        else
        {
            if (node->right)
                node = node->right;
            else
                return node->right = new_node (data);
        }
    }
}

int delete_sub_tree (struct Node_t* node)
{
    if (node->left)  delete_sub_tree (node->left);

    if (node->right) delete_sub_tree (node->right);

    node->parent = NULL;

    free (node->data);

    free (node);

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

char* graph_dump (struct Node_t* root)
{
    assert (root);

    FILE* graph_file = fopen ("../build/graph_tree.dot", "w");
    if (graph_file == NULL)
    {
        printf("ERROR open graph_file\n");
        return NULL;
    }

    fprintf (graph_file, "digraph\n{\n");

    fprintf (graph_file, "bgcolor=\"transparent\"\n");

    //fprintf (graph_file, "rankdir = \"LR\";\n");

    print_tree_preorder_for_file (root, graph_file);
    fprintf (graph_file, "\n");

    fprintf (graph_file, "}");
    fprintf (graph_file, "\n");

    fclose  (graph_file);

    static int dump_number = 1;
    static char filename[50] = {};
    char    command_name[100] = {};

    sprintf (filename, "../build/graph_tree%d.svg", dump_number++);
    sprintf (command_name, "dot ../build/graph_tree.dot -Tsvg -o %s", filename);

    system  (command_name);

    return filename;
}

void print_tree_preorder_for_file (struct Node_t* root, FILE* filename)
{
    if (!root)
        return ; //FIXME

    fprintf (filename, "node%p [shape=Mrecord; label = \" { [%p] | data = %3s | { left = [%p] | right = [%p] } }\"; style = filled; fillcolor = \"#FFFFD0\"];\n",
             root, root, root->data, root->left, root->right);

    if (root->left)
        fprintf (filename, "node%p -> node%p\n;", root, root->left);

    if (root->right)
        fprintf (filename, "node%p -> node%p\n;", root, root->right);

    if (root->left)  print_tree_preorder_for_file (root->left , filename);

    if (root->right) print_tree_preorder_for_file (root->right, filename);
}

FILE* open_log_file (const char* filename)
{
    LOG_FILE = fopen (filename, "w");
    if (LOG_FILE == NULL)
    {
        printf("ERROR open dump file\n");
        return NULL;
    }

    fprintf (LOG_FILE, "<pre>\n");

    return LOG_FILE;
}

int write_log_file (struct Node_t* root, const char* reason_bro)
{
    assert (root);

    fprintf (LOG_FILE, "<body style=\"background-color: #AFEEEE\">");

    fprintf (LOG_FILE, "<hr> <h2> %s </h2> <br> <hr>\n\n", reason_bro);

    const char* filename = graph_dump (root);

    fprintf (LOG_FILE, "\n\n<img src=\"%s\">", filename);

    return 0;
}
