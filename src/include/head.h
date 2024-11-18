#ifndef HEAD_H_
#define HEAD_H_

struct Node_t
{
    char* data;

    struct Node_t* left;
    struct Node_t* right;
    struct Node_t* parent;

    int shoot_free;
};

struct Buffer_t
{
    char* buffer_ptr;
    char* current_ptr;


};

#endif
