/**
 * \file graph.h
 */

#ifndef _GRAPH_H_
#define _GRAPH_H_


#include "list.h"

struct graph
{
    struct list vertices;
};

struct graph_vertex
{
    struct list out_edges;
    struct list_elem graph_elem;
    struct list_elem ret_elem;
    union
    {
        struct
        {
            float rank;
            float next_rank;
        } pagerank;
    } scratch;
};

struct graph_edge
{
    struct list_elem in_vtx_elem;
    struct graph_vertex *dst;
    int weight;
};

void graph_init(struct graph *g);
void graph_add_edge(struct graph_edge *e, struct graph_vertex *src,
        struct graph_vertex *dst, int weight);
void graph_add_vertex(struct graph *g, struct graph_vertex *v);


#endif /* end of include guard: _GRAPH_H_ */
