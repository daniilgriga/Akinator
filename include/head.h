#ifndef HEAD_H_
#define HEAD_H_

struct Node_t
{
    int data;

    struct Node_t* left;
    struct Node_t* right;
    struct Node_t* parent;
};

#endif
