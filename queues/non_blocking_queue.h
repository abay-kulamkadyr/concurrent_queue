#include "../common/allocator.h"
#include "../common/utils.h"

#define LFENCE asm volatile("lfence" : : : "memory")
#define SFENCE asm volatile("sfence" : : : "memory")

#define EXTRACT_MOST_16_BITS(ptr) ((uintptr_t)(ptr) >> 48)
#define EXTRACT_LEAST_48_BITS(ptr) ((uintptr_t)(ptr) & 0xFFFFFFFFFFFF) 
#define COMBINE_TO_64_BITS(ptr, count) ((uintptr_t(count) << 48) | uintptr_t(ptr))

template <class P>
struct pointer_t {
    P* ptr;

    P* address() { 
        return reinterpret_cast<P*> (EXTRACT_LEAST_48_BITS(ptr));
    }

    uint count() {
        return static_cast<uint> (EXTRACT_MOST_16_BITS(ptr));
    }
};

template <class T> 
class Node
{
public:
    T value; 
    pointer_t<Node<T>> next;
};


template <class T>
class NonBlockingQueue
{
    pointer_t<Node<T>> q_head;
    pointer_t<Node<T>> q_tail;
    CustomAllocator my_allocator_;

public:
    
    NonBlockingQueue() : my_allocator_()
    {
        std::cout << "Using NonBlockingQueue\n";
    }

    void initQueue(long t_my_allocator_size){
        my_allocator_.initialize(t_my_allocator_size, sizeof(Node<T>));
        // Initialize the queue head or tail here
        Node<T>* newNode = (Node<T>*)my_allocator_.newNode();
        newNode->next.ptr = NULL;
        q_head.ptr = q_tail.ptr = newNode;
        my_allocator_.freeNode(newNode);
    }
    /*
    * combine ptr bits as 48 least sig bits and count as 16 most sig bits
    */
    pointer_t<Node<T>> combineBits(Node<T>* ptr, uint count){
        uintptr_t combined_ptr = COMBINE_TO_64_BITS((ptr),(count)); 
        pointer_t<Node<T>> res;
        res.ptr = reinterpret_cast<Node<T>*>(combined_ptr);
        return res;
    }

    void enqueue(T value)
    {
        // Use LFENCE and SFENCE as mentioned in pseudocode
        Node<T>* node = (Node<T>* )my_allocator_.newNode();
        node->value = value;
        node->next.ptr = NULL;
        pointer_t<Node<T>> tail;
        SFENCE;
        while(true) {
            tail = q_tail;
            LFENCE;
            pointer_t<Node<T>> next = tail.address()->next;
            LFENCE;
            if (tail.ptr == q_tail.ptr ) {
                if (next.address() == NULL) {
                    if(CAS(&tail.address()->next, next, combineBits(node, next.count() + 1)))
                        break;
                }
                else
                    CAS(&q_tail, tail, combineBits(next.address(), tail.count() + 1)); 
            }
        }
        SFENCE;
        CAS(&q_tail, tail, combineBits(node, tail.count() + 1));
    }

    bool dequeue(T *value)
    {
        // Use LFENCE and SFENCE as mentioned in pseudocode
        pointer_t<Node<T>> head;
        pointer_t<Node<T>> tail;
        while(true){
            head = q_head;
            LFENCE;
            tail = q_tail;
            LFENCE;
            pointer_t<Node<T>> next = head.address()->next;
            LFENCE;
            if (head.ptr == q_head.ptr) {
                if(head.address() == tail.address()) {
                    if(next.address() == NULL)
                        return false;
                    CAS(&q_tail, tail, combineBits(next.address(), tail.count() + 1));                 }
                else {
                    *value = next.address()->value;
                    if(CAS(&q_head, head, combineBits(next.address(), head.count() + 1)))
                        break;
                }
            }
        }
        my_allocator_.freeNode(head.address());
        return true;
    }

    void cleanup()
    {
        my_allocator_.cleanup();
    } 

};

