/* ISO C11: generate an ngspice analog 1024-node complex-state network.
 * Graph edges are supplied in edges.txt and validated as 8-regular.
 * This program does not assert a Ramanujan proof; use spectral checks.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define N 1024
#define D 8
#define EDGES (N*D/2)
static unsigned char adj[N][N];
static int degree[N];
int main(int argc, char **argv) {
    const char *in = argc > 1 ? argv[1] : "edges.txt";
    const char *out = argc > 2 ? argv[2] : "neurons_1024.cir";
    FILE *fi = fopen(in, "r");
    if (!fi) { perror(in); return 1; }
    int a,b, m=0;
    while (fscanf(fi, "%d %d", &a, &b) == 2) {
        if (a<0 || a>=N || b<0 || b>=N || a==b || adj[a][b]) {
            fprintf(stderr, "invalid or duplicate edge: %d %d\n",a,b);
            fclose(fi); return 2;
        }
        adj[a][b]=adj[b][a]=1; degree[a]++;degree[b]++;m++;
    }
    fclose(fi);
    if (m != EDGES) {fprintf(stderr,"edges %d != %d\n",m,EDGES);return 3;}
    for (int i=0;i<N;i++) if(degree[i]!=D) {
        fprintf(stderr,"vertex %d has degree %d\n",i,degree[i]);return 4;
    }
    FILE *fo=fopen(out,"w");if(!fo){perror(out);return 5;}
    fprintf(fo,"* 1024-node complex analog attractor netlist; X=real, Y=imaginary\n");
    fprintf(fo,"* 8-regular sparse connectivity; numerical spectral check in README\n");
    fprintf(fo,".param cap=1 alpha=-0.2 beta=0.2 gc=0.04\n");
    fprintf(fo,"* Each neuron follows C dx/dt=-alpha*x-beta*x*(x^2+y^2)+coupling\n");
    for(int i=0;i<N;i++) {
        fprintf(fo,"Cx%d x%d 0 {cap}\nCy%d y%d 0 {cap}\n",i,i,i,i);
        fprintf(fo,"Bx%d x%d 0 I={alpha*V(x%d)+beta*V(x%d)*(V(x%d)^2+V(y%d)^2)}\n",i,i,i,i,i,i);
        fprintf(fo,"By%d y%d 0 I={alpha*V(y%d)+beta*V(y%d)*(V(x%d)^2+V(y%d)^2)}\n",i,i,i,i,i,i);
    }
    int e=0;
    for(int i=0;i<N;i++)for(int j=i+1;j<N;j++)if(adj[i][j]) {
        fprintf(fo,"Rx%d x%d x%d {1/gc}\nRy%d y%d y%d {1/gc}\n",e,i,j,e,i,j);e++;
    }
    fprintf(fo,"* Weak reference phase and external input, avoid floating phase\n");
    fprintf(fo,"Vref ref 0 0.2\nRref ref x0 10\nRyref y0 0 10\n");
    fprintf(fo,"* Initial local cue for associative relaxation\n");
    fprintf(fo,".ic V(x1)=1 V(y2)=0.4 V(x3)=-0.5\n");
    fprintf(fo,".tran 0.1 40 uic\n");
    fprintf(fo,".save V(x0) V(y0) V(x1) V(y1) V(x2) V(y2) V(x3) V(y3)\n");
    fprintf(fo,".end\n");
    fclose(fo);
    fprintf(stdout,"Generated %s: %d neurons, %d undirected edges, %d analog couplers\n",out,N,e,2*e);
    return 0;
}
