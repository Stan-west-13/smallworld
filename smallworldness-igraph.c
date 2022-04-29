#include <stdio.h>
#include <igraph.h>

/*
int igraph_matrix_from_file(
    igraph_matrix_t* m,
    long int nodes,
    FILE* f
);
*/
igraph_real_t smallworld_q(
    igraph_real_t aspl,
    igraph_real_t clust,
    igraph_real_t aspl_R,
    igraph_real_t clust_R
);
igraph_real_t smallworld_omega(
    igraph_real_t aspl,
    igraph_real_t clust,
    igraph_real_t aspl_R,
    igraph_real_t clust_L
);
igraph_real_t smallworld_SWI(
    igraph_real_t aspl,
    igraph_real_t clust,
    igraph_real_t aspl_R,
    igraph_real_t clust_R,
    igraph_real_t aspl_L,
    igraph_real_t clust_L
);

void usage(void);

int main(int argc, char* argv[]) {
  if (argc < 4) {
    usage();
    exit(0);
  }
  long int epochs = atoi(argv[1]);
  long int n = atoi(argv[2]);
  char* filename = argv[3];
  igraph_real_t aspl, aspl_R, aspl_L;
  igraph_real_t clust, clust_R, clust_L;
  igraph_real_t unconn_pairs, unconn_pairs_R, unconn_pairs_L;
  igraph_bool_t directed = 1;
  igraph_bool_t unconn = 1;
  igraph_t g, gperm;
  igraph_vector_t perm_ix;
  //igraph_matrix_t m;

  FILE* f = fopen(filename, "r");
  if (f) {
    igraph_read_graph_edgelist(&g, f, n, directed);
    //igraph_matrix_from_file(&m, n, f);
    fclose(f);
  } else {
    fprintf(stderr, "File does not exist: %s\n", filename);
    exit(1);
  }

  //igraph_adjacency(&g, &m, IGRAPH_ADJ_DIRECTED);

  igraph_rng_seed(igraph_rng_default(), time(NULL));
  igraph_vector_init_seq(&perm_ix, (igraph_real_t) 0, (igraph_real_t) (n - 1));
  igraph_vector_shuffle(&perm_ix);
  igraph_permute_vertices(&g, &gperm, &perm_ix);

  igraph_average_path_length(&g, &aspl, &unconn_pairs, directed, unconn);
  igraph_transitivity_undirected(&g, &clust, IGRAPH_TRANSITIVITY_ZERO);

  igraph_rewire_towards_lattice(&g, igraph_ecount(&g) * epochs, IGRAPH_REWIRING_SIMPLE);
  igraph_average_path_length(&g, &aspl_L, &unconn_pairs_L, directed, unconn);
  igraph_transitivity_undirected(&g, &clust_L, IGRAPH_TRANSITIVITY_ZERO);

  igraph_rewire(&gperm, igraph_ecount(&gperm) * epochs, IGRAPH_REWIRING_SIMPLE);
  igraph_average_path_length(&gperm, &aspl_R, &unconn_pairs_R, directed, unconn);
  igraph_transitivity_undirected(&gperm, &clust_R, IGRAPH_TRANSITIVITY_ZERO);

  printf("%f\t%g\t%f\t%f\t%f\t%g\t%f\t%f\t%f\t%g\t%f\t%f\t%f\t%f\t%f\n",
      aspl,
      unconn_pairs,
      unconn_pairs / (igraph_real_t) (n * n),
      clust,
      aspl_R,
      unconn_pairs_R,
      unconn_pairs_R / (igraph_real_t) (n * n),
      clust_R,
      aspl_L,
      unconn_pairs_L,
      unconn_pairs_L / (igraph_real_t) (n * n),
      clust_L,
      smallworld_q(aspl, clust, aspl_R, clust_R),
      smallworld_omega(aspl, clust, aspl_R, clust_L),
      smallworld_SWI(aspl, clust, aspl_R, clust_R, aspl_L, clust_L)
  );

  igraph_vector_destroy(&perm_ix);
  //igraph_matrix_destroy(&m);
  igraph_destroy(&g);
  igraph_destroy(&gperm);
  return 0;
}

/*
int igraph_matrix_from_file(igraph_matrix_t* m, long int nodes, FILE* f) {
  double x[nodes * nodes];
  fread(&x, sizeof(double), nodes * nodes, f);
  igraph_matrix_init(m, nodes, nodes);
  for (int i = 0; i < nodes; i++) {
    for (int j = 0; j < nodes; j++) {
      igraph_matrix_set(m, i, j, (igraph_real_t) x[(i * nodes) + j]);
    }
  }
  return(0);
}
*/

igraph_real_t smallworld_q(
    igraph_real_t aspl,
    igraph_real_t clust,
    igraph_real_t aspl_R,
    igraph_real_t clust_R)
{
  return((clust/clust_R) / (aspl/aspl_R));
}

igraph_real_t smallworld_omega(
    igraph_real_t aspl,
    igraph_real_t clust,
    igraph_real_t aspl_R,
    igraph_real_t clust_L)
{
  double omega;
  omega = (aspl_R / aspl) - (clust / clust_L);
  return(1 - fabs(omega));
}

igraph_real_t smallworld_SWI(
    igraph_real_t aspl,
    igraph_real_t clust,
    igraph_real_t aspl_R,
    igraph_real_t clust_R,
    igraph_real_t aspl_L,
    igraph_real_t clust_L)
{
  return(
      ((aspl - aspl_L) / (aspl_R - aspl_L))
      * ((clust - clust_R) / (clust_L - clust_R)));
}

void usage(void) {
  printf("\n");
  printf("%s\n", "usage: smallworldness-igraph epochs vertices filename");
  printf("\n");
  printf("%s\n", "  epochs      number of times to attempt to re-wire each edge of the network when");
  printf("%s\n", "              randomizing and latticizing.");
  printf("%s\n", "  vertices    number of vertices (i.e., nodes) in the network.");
  printf("%s\n", "  filename    path to a binary file representing an adjacency matrix as doubles.");
  printf("%s\n", "              the binary file should contain only edge data. the vertices argument");
  printf("%s\n", "              above will determine the shape of the network. it is assumed the the");
  printf("%s\n", "              adjacency matrix is symmetric.");
  printf("\n");
  return;
}

