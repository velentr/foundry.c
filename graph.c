/**
 * \file graph.c
 */

#include "graph.h"
#include "list.h"

void graph_init(struct graph *g)
{
    list_init(&g->vertices);
}

void graph_add_edge(struct graph_edge *e, struct graph_vertex *src,
        struct graph_vertex *dst, int weight)
{
    e->dst = dst;
    e->weight = weight;
    list_pushback(&src->out_edges, &e->in_vtx_elem);
}

void graph_add_vertex(struct graph *g, struct graph_vertex *v)
{
    list_init(&v->out_edges);
    list_pushback(&g->vertices, &v->graph_elem);
}

