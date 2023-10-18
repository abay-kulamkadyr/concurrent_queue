#include "../common/allocator.h"
#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
template <class T>
struct Node
{
    T value;
    Node<T>* next;
};

extern std::atomic<bool> no_more_enqueues;

template <class T>
class OneLockBlockingQueue
{
    Node<T>* q_head;
    Node<T>* q_tail;
    std::atomic<bool> wakeup_dq;
    std::mutex mutex;
    std::condition_variable queue_empty;
    CustomAllocator my_allocator_;
public:
    OneLockBlockingQueue() : my_allocator_()
    {
        std::cout << "Using OneLockBlockingQueue\n";
    }

    void initQueue(long t_my_allocator_size){
        std::cout << "Using Allocator\n";
        my_allocator_.initialize(t_my_allocator_size, sizeof(Node<T>));
        // Initialize the queue head or tail here
        
        Node<T>* newNode = (Node<T>*)my_allocator_.newNode();
        newNode->next = NULL;
        q_head = q_tail = newNode;
        wakeup_dq.store(false);
    }

    void enqueue(T value)
    {
        //creating a node
        Node<T>* newNode= (Node<T>*)my_allocator_.newNode();
        newNode->value=value;
        newNode->next=NULL;
        { 
        std::lock_guard<std::mutex> lock(mutex);
        //Append to q_tail and update the queue
        q_tail->next = newNode;
        q_tail = newNode;
        //Signal wakeup for waiting dequeue operations
        wakeup_dq.store(true);
        }
        queue_empty.notify_one(); 
    }

    bool dequeue(T *value)
    {
        { 
        std::unique_lock<std::mutex> lock(mutex);
        Node<T>* node = q_head;
        Node<T>* new_head= q_head->next;
         
        while(new_head == NULL){
            queue_empty.wait_for(lock, std::chrono::milliseconds(200), [this] {return wakeup_dq.load() || no_more_enqueues.load();}); 
            if(q_head->next != NULL) {
                node = q_head;
                new_head = q_head->next;
                wakeup_dq.store(false);
            } 
            if(q_head->next == NULL && no_more_enqueues.load()) {
                return false;
            }
        }

        *value=new_head->value;
        //Update q_head
        q_head = new_head;
        wakeup_dq.store(false);
        my_allocator_.freeNode(node);
        return true;
        } 
    }

    void cleanup()
    {
        my_allocator_.cleanup();
    }
};
