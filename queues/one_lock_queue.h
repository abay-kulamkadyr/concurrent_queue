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
class OneLockQueue
{
    Node<T>* q_head;
    Node<T>* q_tail;
    std::mutex mutex; 
    CustomAllocator my_allocator_;
public:
    OneLockQueue() : my_allocator_()
    {
        std::cout << "Using OneLockQueue\n";
    }

    void initQueue(long t_my_allocator_size){
        std::cout << "Using Allocator\n";
        my_allocator_.initialize(t_my_allocator_size, sizeof(Node<T>));
        // Initialize the queue head or tail here
        Node<T>* newNode= (Node<T>*)my_allocator_.newNode();
        newNode->next=NULL;
        q_head=newNode;
        q_tail=newNode;
    }

    void enqueue(T value)
    {
        //creating a node
        Node<T>* newNode= (Node<T>*)my_allocator_.newNode();
        newNode->value=value;
        newNode->next=NULL;
        // appending to tail and updating taiel
        
        mutex.lock();
        q_tail->next= newNode;
        q_tail= newNode;
       
        mutex.unlock();
    }

    bool dequeue(T *value)
    {
        
        mutex.lock();
        Node<T>* node=q_head;
        Node<T>* new_head= q_head->next;
        
        if(new_head==NULL){
            mutex.unlock();
            return false;
        }
        *value=new_head->value;
        //Update q_head
        q_head= new_head;
        mutex.unlock();
        my_allocator_.freeNode(node);
        return true;
    }

    void cleanup()
    {
        my_allocator_.cleanup();
    }
};