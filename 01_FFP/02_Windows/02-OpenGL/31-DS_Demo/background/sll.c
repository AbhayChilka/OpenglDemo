#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sll.h"

sll_t *create_list(void)
{
    return get_node(0);
}

status_t insert_start(sll_t *p_list, data_t new_data)
{
    node_t *new_node = NULL;
    new_node = get_node(new_data);

    new_node->next = p_list->next;
    p_list->next = new_node;

    return SUCCESS;
}

status_t insert_end(sll_t *p_list, data_t new_data)
{
    node_t *run = NULL;
    run = p_list;

    while (run->next != NULL)
    {
        run = run->next;
    }

    run->next = get_node(new_data);

    return SUCCESS;
}

status_t insert_after(sll_t *p_list, data_t e_data, data_t new_data)
{
    node_t *new_node = NULL;
    node_t *run = NULL;

    run = p_list->next;
    while (run != NULL)
    {
        if (run->data == e_data)
        {
            new_node = get_node(new_data);
            new_node->next = run->next;
            run->next = new_node;

            return SUCCESS;
        }
        run = run->next;
    }

    return LIST_DATA_NOT_FOUND;
}

status_t insert_before(sll_t *p_list, data_t e_data, data_t new_data)
{
    node_t *new_node = NULL;
    node_t *run = NULL;
    node_t *prev = NULL;
    prev = p_list;
    run = p_list->next;

    while (run != NULL)
    {
        if (run->data == e_data)
        {
            new_node = get_node(new_data);
            new_node->next = run;
            prev->next = new_node;

            return SUCCESS;
        }
        prev = run;
        run = run->next;
    }

    return LIST_DATA_NOT_FOUND;
}

void show_list(sll_t *p_list, const char *msg)
{
    if (msg)
        puts(msg);

    sll_t *run = p_list->next;

    printf("[Start]->");
    while (run != NULL)
    {
        printf("[%p]->", run->data);
        run = run->next;
    }
    printf("[END]\n");
}

status_t remove_start(sll_t *p_list)
{
    node_t *r_node = NULL;
    if (p_list->next == NULL)
        return LIST_EMPTY;

    r_node = p_list->next;
    p_list->next = r_node->next;
    r_node->next = NULL;
    free(r_node);

    return SUCCESS;
}

status_t remove_end(sll_t *p_list)
{
    node_t *prev_node = NULL;
    node_t *run_node = NULL;

    if (p_list->next == NULL)
        return LIST_EMPTY;

    prev_node = p_list;
    run_node = p_list->next;

    while (run_node->next != NULL)
    {
        prev_node = run_node;
        run_node = run_node->next;
    }

    free(run_node);
    run_node = NULL;

    prev_node->next = NULL;
    return SUCCESS;
}

status_t remove_data(sll_t *p_list, data_t r_data)
{
    node_t *run_prev = NULL;
    node_t *run = NULL;

    run_prev = p_list;
    run = p_list->next;

    while (run != NULL)
    {
        if (run->data == r_data)
        {
            run_prev->next = run->next;
            free(run);
            run = NULL;
            return SUCCESS;
        }
        run_prev = run;
        run = run->next;
    }

    return LIST_DATA_NOT_FOUND;
}

status_t remove_node(sll_t *p_list, node_t *r_node)
{
    node_t *run = p_list;

    while (run != NULL)
    {
        if (run->next == r_node)
        {
            run->next = r_node->next;
            free(r_node);
            return SUCCESS;
        }

        run = run->next;
    }

    return LIST_DATA_NOT_FOUND;
}

status_t destroy_list(sll_t **p_list)
{
    assert(*p_list != NULL);
    sll_t *r_node = NULL;

    while ((*p_list) != NULL)
    {
        r_node = (*p_list);
        (*p_list) = (*p_list)->next;
        free(r_node);
    }
    return SUCCESS;
}

status_t get_start(sll_t *p_list, data_t *p_start_data)
{
    if (p_list->next == NULL)
        return LIST_EMPTY;

    *p_start_data = p_list->next->data;

    return SUCCESS;
}

status_t get_end(sll_t *p_list, data_t *p_end_data)
{
    node_t *run = NULL;

    if (p_list->next == NULL)
        return LIST_EMPTY;

    run = p_list->next;

    while (run->next != NULL)
    {
        run = run->next;
    }
    *p_end_data = run->data;

    return SUCCESS;
}

status_t pop_start(sll_t *p_list, data_t *p_start_data)
{
    node_t *run = NULL;

    if (p_list->next == NULL)
        return LIST_EMPTY;

    run = p_list->next;
    p_list->next = run->next;

    *p_start_data = run->data;
    free(run);
    run = NULL;
    return SUCCESS;
}

status_t pop_end(sll_t *p_list, data_t *p_end_data)
{
    node_t *run = NULL;
    node_t *prev_run = NULL;

    if (p_list->next == NULL)
        return LIST_EMPTY;

    run = p_list->next;
    prev_run = p_list;

    while (run->next != NULL)
    {
        prev_run = run;
        run = run->next;
    }

    *p_end_data = run->data;
    free(run);
    prev_run->next = NULL;

    return SUCCESS;
}

status_t is_empty(sll_t *p_list)
{
    return (p_list->next == NULL);
}

status_t find(sll_t *p_list, data_t f_data)
{
    node_t *run = NULL;
    if (p_list->next == NULL)
        return LIST_EMPTY;

    run = p_list->next;

    while (run != NULL)
    {
        if (run->data == f_data)
        {
            return TRUE;
        }
        run = run->next;
    }

    return FALSE;
}

len_t get_list_length(sll_t *p_list)
{
    node_t *run = NULL;
    len_t len = 0;

    for (run = p_list->next; run != NULL; run = run->next)
        len++;

    return len;
}

status_t clear_list(sll_t *p_list)
{
    node_t *r_node = NULL;
    node_t *next_node = NULL;
    r_node = p_list->next;

    while (r_node != NULL)
    {
        next_node = r_node->next;
        free(r_node);
        r_node = next_node;
    }
    p_list->next = NULL;

    return SUCCESS;
}

// inter list routines
sll_t *get_concatenated_list(sll_t *p_list_1, sll_t *p_list_2)
{
    sll_t *p_concat_list = NULL;
    node_t *run = NULL;
    p_concat_list = create_list();

    for (run = p_list_1->next; run != NULL; run = run->next)
        assert(insert_end(p_concat_list, run->data) == SUCCESS);

    for (run = p_list_2->next; run != NULL; run = run->next)
        assert(insert_end(p_concat_list, run->data) == SUCCESS);

    return p_concat_list;
}

status_t concat_lists(sll_t *p_list_1, sll_t *p_list_2)
{
    node_t *run1 = NULL;

    if (is_empty(p_list_2) == TRUE)
    {
        free(p_list_2);
        p_list_2 = NULL;
        return SUCCESS;
    }

    run1 = p_list_1;
    while (run1->next != NULL)
        run1 = run1->next;

    run1->next = p_list_2->next;

    free(p_list_2);
    p_list_2 = NULL;

    return SUCCESS;
}

sll_t *merge_lists(sll_t *p_list_1, sll_t *p_list_2)
{
    sll_t *p_merge_list = NULL;
    p_merge_list = create_list();

    node_t *run1 = NULL;
    node_t *run2 = NULL;

    run1 = p_list_1->next;
    run2 = p_list_2->next;

    while (1)
    {
        if (run1 == NULL)
        {
            while (run2 != NULL)
            {
                assert(insert_end(p_merge_list, run2->data) == SUCCESS);
                run2 = run2->next;
            }
            break;
        }
        if (run2 == NULL)
        {
            while (run1 != NULL)
            {
                assert(insert_end(p_merge_list, run1->data) == SUCCESS);
                run1 = run1->next;
            }
            break;
        }

        if (run1->data < run2->data)
        {
            assert(insert_end(p_merge_list, run1->data) == SUCCESS);
            run1 = run1->next;
        }
        else
        {
            assert(insert_end(p_merge_list, run2->data) == SUCCESS);
            run2 = run2->next;
        }
    }

    return p_merge_list;
}

data_t *to_array(sll_t *p_list, int *pN)
{
    len_t N = 0;
    data_t *p_arr = NULL;
    node_t *run = NULL;
    N = get_list_length(p_list);

    if (N == 0)
        return p_arr;
    p_arr = (data_t *)xMalloc(sizeof(data_t) * N);

    int i = 0;
    run = p_list->next;

    while (run != NULL)
    {
        p_arr[i] = run->data;
        i++;
        run = run->next;
    }

    *pN = N;
    return p_arr;
}

sll_t *to_list(data_t *p_arr, int N)
{
    sll_t *p_list = NULL;
    p_list = create_list();

    for (int i = 0; i < N; i++)
    {
        assert(insert_end(p_list, p_arr[i]) == SUCCESS);
    }

    return p_list;
}

sll_t *get_reversed_list(sll_t *p_list)
{
    sll_t *p_revs_list = NULL;
    node_t *run = NULL;

    p_revs_list = create_list();

    run = p_list->next;

    while (run != NULL)
    {
        assert(insert_start(p_revs_list, run->data) == SUCCESS);
        run = run->next;
    }

    return p_revs_list;
}

status_t reverse_list(sll_t *p_list)
{
    node_t *run = NULL;
    node_t *prev = NULL;
    node_t *next = NULL;

    prev = p_list->next;
    if (prev == NULL)
        return SUCCESS;
    run = prev->next;

    prev->next = NULL;
    while (run != NULL)
    {
        next = run->next;
        run->next = prev;
        prev = run;
        run = next;
    }

    p_list->next = prev;
    
    return SUCCESS;
}

// Auxillary Function
static sll_t *get_node(data_t new_data)
{
    sll_t *new_node = NULL;
    new_node = (sll_t *)xMalloc(sizeof(sll_t));

    new_node->data = new_data;
    new_node->next = NULL;

    return new_node;
}

static void *xMalloc(size_t size_in_bytes)
{
    void *p = NULL;
    p = malloc(size_in_bytes);
    if (p == NULL)
    {
        puts("Error Allocating Memory");
        exit(EXIT_FAILURE);
    }

    return p;
}
