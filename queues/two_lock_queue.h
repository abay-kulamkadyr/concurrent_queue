#include "../common/allocator.h"
#include <iostream> 
#include <mutex>
template <class T>
struct Node
{
    T value;
    Node<T>* next; 
};

template <class T>
class TwoLockQueue
{
    Node<T>* q_head; 
    Node<T>* q_tail;
    std::mutex enq_mutex;
    std::mutex deq_mutex; 
    CustomAllocator my_allocator_;

public:
    TwoLockQueue() : my_allocator_()
    {
        std::cout << "Using TwoLockQueue\n";
    }

    void initQueue(long t_my_allocator_size){
        std::cout << "Using Allocator\n";
        my_allocator_.initialize(t_my_allocator_size, sizeof(Node<T>));
        // Initialize the queue head or tail here

        Node<T>* newNode = (Node<T>*)my_allocator_.newNode();
        newNode->next=NULL;
        q_head=newNode;
        q_tail=newNode;
    }

    void enqueue(T value)
    {
        Node<T>* newNode= (Node<T>*)my_allocator_.newNode();
        newNode->value=value;
        newNode->next=NULL;
        enq_mutex.lock();
        q_tail->next= newNode;
        q_tail=newNode;
        enq_mutex.unlock();
    }

    bool dequeue(T *value)
    {
        deq_mutex.lock();
        Node<T>* node= q_head;
        Node<T>* new_head= q_head->next;
        if(new_head==NULL){
            deq_mutex.unlock();
            return false;
        }
        *value=new_head->value;
        q_head= new_head;
        deq_mutex.unlock();
        my_allocator_.freeNode(node);
        return true;
    }

    void cleanup()
    {
        my_allocator_.cleanup();
    }
};