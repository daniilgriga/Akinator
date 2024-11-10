#ifndef HEAD_H_
#define HEAD_H_

struct Node_t
{
    char* data;

    struct Node_t* left;
    struct Node_t* right;
    struct Node_t* parent;
};

#endif
