#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "head.h"

struct Node_t* new_node  (int data);

int print_tree_preorder  (struct Node_t* root);

int print_tree_postorder (struct Node_t* root);

int print_tree_inorder   (struct Node_t* root);

int delete_sub_tree (struct Node_t* node);

void print_tree_preorder_for_file (struct Node_t* root, FILE* filename);

const char* graph_dump (struct Node_t* root);

FILE* open_log_file (const char* filename);

struct Node_t* insert_node (struct Node_t* node, int data);

struct Node_t* iterativly_insert (struct Node_t* node, int data);

int write_log_file (struct Node_t* root, const char* reason_bro/* in c++ " = doesn't_care_bro"*/);

FILE* LOG_FILE = NULL;


int main (void)
{
   open_log_file ("../build/dump.html");

    struct Node_t* root    = new_node (50);
    struct Node_t* node_12 = new_node (12);
    struct Node_t* node_70 = new_node (70);
    struct Node_t* node_5  = new_node ( 5);
    struct Node_t* node_15 = new_node (15);
    struct Node_t* node_60 = new_node (60);

    root->left     = node_12;
    root->right    = node_70;
    node_12->left  = node_5;
    node_12->right = node_15;
    node_70->left  = node_60;

    write_log_file (root, "before insert");

    printf ("\npreorder: ");
    print_tree_preorder  (root);

    printf ("\npostorder: ");
    print_tree_postorder (root);

    printf ("\ninorder: ");
    print_tree_inorder   (root);

    insert_node (root, 17);
    write_log_file (root, "after insert 17");

    insert_node (root, 52);
    write_log_file (root, "after insert 52");

    fclose (LOG_FILE);

    delete_sub_tree (root);

    return 0;
}

struct Node_t* new_node (int data)
{
    struct Node_t* node = (struct Node_t*) calloc (1, sizeof(*node));
    assert (node); //FIXME

    fprintf (stderr, "MEM 0x%p: calloc()\n", node);

    node->data  = data;
    node->left  = NULL;
    node->right = NULL;

    node->parent = NULL;

    return node;
}

struct Node_t* insert_node (struct Node_t* node, int data)
{
    if (data < node->data)
    {
        if (node->left)
            insert_node (node->left, data);
        else
            node->left  = new_node (data);
    }
    else
    {
        if (node->right)
            insert_node (node->right, data);
        else
            node->right = new_node (data);
    }

    return node;
}

struct Node_t* iterativly_insert (struct Node_t* node, int data)  // net seal
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

    fprintf (stderr, "MEM 0x%p: free()\n", node);

    free (node);

    return 0;
}

int print_tree_preorder (struct Node_t* root)
{
    if (!root)
        return 1; //FIXME

    printf ("(");

    printf ("%d ",  root->data);

    if (root->left)  print_tree_preorder (root->left);

    if (root->right) print_tree_preorder (root->right);

    printf (")");

    return 0;
}

int print_tree_postorder (struct Node_t* root)
{
    if (!root)
        return 1; //FIXME

    printf ("(");

    if (root->left)  print_tree_postorder (root->left);

    if (root->right) print_tree_postorder (root->right);

    printf ("%d ",  root->data);

    printf (")");

    return 0;
}

int print_tree_inorder (struct Node_t* root)
{
    if (!root)
        return 1; //FIXME

    printf ("(");

    if (root->left)  print_tree_inorder (root->left);

    printf ("%d ",  root->data);

    if (root->right) print_tree_inorder (root->right);

    printf (")");

    return 0;
}

const char* graph_dump (struct Node_t* root)
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

    fprintf (filename, "node%p [shape=Mrecord; label = \" { [%p] | data = %3d | { left = [%p] | right = [%p] } }\"; style = filled; fillcolor = \"#FFFFD0\"];\n",
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
