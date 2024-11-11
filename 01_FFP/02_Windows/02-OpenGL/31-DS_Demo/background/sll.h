#ifndef _SLL_H
#define _SLL_H

#define SUCCESS 1
#define TRUE 1
#define FALSE 0
#define LIST_EMPTY 2
#define LIST_DATA_NOT_FOUND 3
#define LIST_NOT_EMPTY 4

typedef int status_t;
typedef void(*data_t)();
typedef struct node node_t;
typedef node_t sll_t;
typedef int len_t;

struct node
{
    data_t data;
    struct node *next;
};

sll_t *create_list(void);
status_t insert_start(sll_t *p_list, data_t new_data);
status_t insert_end(sll_t *p_list, data_t new_data);
status_t insert_after(sll_t *p_list, data_t e_data, data_t new_data);
status_t insert_before(sll_t *p_list, data_t e_data, data_t new_data);

void show_list(sll_t *p_list, const char *msg);

status_t remove_start(sll_t *p_list);
status_t remove_end(sll_t *p_list);
status_t remove_data(sll_t *p_list, data_t r_data);
status_t remove_node(sll_t *p_list, node_t *r_node);

status_t get_start(sll_t *p_list, data_t *p_start_data);
status_t get_end(sll_t *p_list, data_t *p_end_data);

status_t pop_start(sll_t *p_list, data_t *p_start_data);
status_t pop_end(sll_t *p_list, data_t *p_end_data);

status_t is_empty(sll_t *p_list);
status_t find(sll_t *p_list, data_t f_data);
len_t get_list_length(sll_t *p_list);

status_t clear_list(sll_t *p_list);

status_t destroy_list(sll_t **p_list);

// inter list routines
sll_t *get_concatenated_list(sll_t *p_list_1, sll_t *p_list_2);
status_t concat_lists(sll_t *p_list_1, sll_t *p_list_2);

sll_t *merge_lists(sll_t *p_list_1, sll_t *p_list_2);

data_t *to_array(sll_t *p_list, int *pN);
sll_t *to_list(data_t *p_arr, int N);

sll_t *get_reversed_list(sll_t *p_list);

status_t reverse_list(sll_t *p_list);

// Auxillary Function
static sll_t *get_node(data_t new_data);
static void *xMalloc(size_t size_in_bytes);

#endif
