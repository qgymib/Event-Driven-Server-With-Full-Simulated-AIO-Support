#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>

#include "clientbuffer.h"
#include "define.h"

struct SortTreeNode
{
    struct ClientBuffer *buffer;

    struct SortTreeNode *parrent;
    struct SortTreeNode *left;
    struct SortTreeNode *right;
};

static struct SortTreeNode *TREE_HEAD = NULL;
static pthread_mutex_t TREE_LOCKER = PTHREAD_MUTEX_INITIALIZER;

static void freeTreeNode(struct SortTreeNode* node);

struct ClientBuffer* getClientBuffer(int fd)
{
    struct ClientBuffer *result = NULL;
    struct SortTreeNode *head = TREE_HEAD;
    if (head == NULL)
    {
        return NULL;
    }

    for (;;)
    {
        if (fd == head->buffer->fd)
        {
            result = head->buffer;
            break;
        }
        else if (fd < head->buffer->fd)
        {
            if (head->left == NULL)
            {
                break;
            }
            else
            {
                head = head->left;
            }
        }
        else if (fd > head->buffer->fd)
        {
            if (head->right == NULL)
            {
                break;
            }
            else
            {
                head = head->right;
            }
        }
    }

    return result;
}

void insertClientBuffer(struct ClientBuffer* buffer)
{
    pthread_mutex_lock(&TREE_LOCKER);
    
    if (TREE_HEAD == NULL)
    {
        TREE_HEAD = (struct SortTreeNode*) malloc(sizeof (struct SortTreeNode));
        TREE_HEAD->buffer = buffer;
        TREE_HEAD->parrent = NULL;
        TREE_HEAD->left = NULL;
        TREE_HEAD->right = NULL;
    }
    else
    {
        struct SortTreeNode* head = TREE_HEAD;
        for (;;)
        {
            if (buffer->fd == head->buffer->fd)
            {
                head->buffer = buffer;
                break;
            }
            else if (buffer->fd < head->buffer->fd)
            {
                if (head->left == NULL)
                {
                    head->left = (struct SortTreeNode*) malloc(sizeof (struct SortTreeNode));
                    head->left->buffer = buffer;
                    head->left->parrent = head;
                    head->left->left = NULL;
                    head->left->right = NULL;
                    break;
                }
                else
                {
                    head = head->left;
                }
            }
            else if (buffer->fd > head->buffer->fd)
            {
                if (head->right == NULL)
                {
                    head->right = (struct SortTreeNode*) malloc(sizeof (struct SortTreeNode));
                    head->right->buffer = buffer;
                    head->right->parrent = head;
                    head->right->left = NULL;
                    head->right->right = NULL;
                    break;
                }
                else
                {
                    head = head->right;
                }
            }
        }
    }
    
    pthread_mutex_unlock(&TREE_LOCKER);
}

void deleteClientBuffer(int fd)
{
    pthread_mutex_lock(&TREE_LOCKER);
    
    if(TREE_HEAD == NULL)
    {
        return;
    }
    
    pthread_mutex_unlock(&TREE_LOCKER);
}

void freeClientBuffer()
{
    freeTreeNode(TREE_HEAD);
    TREE_HEAD = NULL;
}

static void freeTreeNode(struct SortTreeNode* node)
{
    if(node != NULL)
    {
        struct SortTreeNode* right = node->right;
        freeTreeNode(node->left);
        free(node->buffer);
        free(node);
        freeTreeNode(right);
    }
}
