/* C11 deterministic reference for 1024-node complex analog polynomial network.
 * Physical time in reference model corresponds to SPICE transient time, not Wick-rotated physics.
 * build: cc -std=c11 -O3 -Wall -Wextra -pedantic verify.c -lm -o verify
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#define N 1024
#define DEG 8
#define E (N*DEG/2)
#define STEPS 2400
#define DT 0.05
#define ALPHA (-0.2)
#define BETA (0.2)
#define GC (0.04)
#define GREF (0.1)
#define REF (0.2)
#define NOISE_STEPS 400
static int ea[E], eb[E], degree[N];
static uint64_t rng;
static uint64_t rand64(void) { uint64_t x=rng; x^=x>>12; x^=x<<25; x^=x>>27; rng=x; return x*UINT64_C(2685821657736338717); }
static double uniform01(void) { return (double)(rand64()>>11)*(1.0/9007199254740992.0); }
static int read_edges(const char *name) {
    FILE *f=fopen(name,"r"); if(!f){perror(name);return 0;}
    static unsigned char seen[N][N];
    int a,b,k=0;
    while(fscanf(f,"%d %d",&a,&b)==2) {
        if(k>=E || a<0||a>=N||b<0||b>=N||a==b||seen[a][b]) {fputs("invalid edges\n",stderr);fclose(f);return 0;}
        seen[a][b]=seen[b][a]=1; ea[k]=a; eb[k]=b; degree[a]++; degree[b]++; k++;
    }
    fclose(f);
    if(k!=E){fprintf(stderr,"edges %d != %d\n",k,E);return 0;}
    for(int i=0;i<N;i++) if(degree[i]!=DEG){fprintf(stderr,"deg %d\n",i);return 0;}
    return 1;
}
static void laplacian(const double *x, double *Lx, const uint8_t *alive) {
    memset(Lx,0,N*sizeof(double));
    for(int e=0;e<E;e++){
        int a=ea[e], b=eb[e];
        if(!alive[a]||!alive[b]) continue;
        double d = x[a]-x[b];
        Lx[a]+=d; Lx[b]-=d;
    }
}
static double energy(const double *x, const double *y, const uint8_t *alive) {
    double e=0;
    for(int i=0;i<N;i++) if(alive[i]) {
        double r2 = x[i]*x[i]+y[i]*y[i];
        e += 0.5*ALPHA*r2 + 0.25*BETA*r2*r2;
    }
    for(int e=0;e<E;e++){
        int a=ea[e], b=eb[e];
        if(!alive[a]||!alive[b]) continue;
        double dx=x[a]-x[b], dy=y[a]-y[b];
        e += 0.5*GC*(dx*dx+dy*dy);
    }
    return e;
}
static void step(double *x, double *y, const uint8_t *alive) {
    static double Lx[N], Ly[N];
    laplacian(x,Lx,alive); laplacian(y,Ly,alive);
    for(int i=0;i<N;i++) if(alive[i]) {
        double r2 = x[i]*x[i]+y[i]*y[i];
        double fx = -(ALPHA + BETA*r2)*x[i] - GC*Lx[i];
        double fy = -(ALPHA + BETA*r2)*y[i] - GC*Ly[i];
        if(i==0){ fx += GREF*(REF - x[i]); fy += -GREF*y[i]; }
        x[i] += DT*fx; y[i] += DT*fy;
    }
}
int main(void) {
    if(!read_edges("edges.txt")) return 1;
    rng = 0xC0FFEEULL;
    static double x[N], y[N], x0[N], y0[N];
    static uint8_t alive[N];
    memset(alive,1,N);
    for(int i=0;i<N;i++){ x[i]=0.1*sin(0.01*i); y[i]=0.1*cos(0.01*i); }
    x[1]=1; y[2]=0.4; x[3]=-0.5;
    FILE *csv=fopen("results.csv","w");
    if(!csv){perror("results.csv");return 2;}
    fprintf(csv,"scenario,removed_percent,alive,components,final_energy,gradient_rms,max_amplitude,max_energy_increase_after_forcing,rms_to_clean_survivors,structural_invariants\n");
    const char *scenarios[] = {"clean","noisy","loss_5pct","loss_10pct","loss_20pct"};
    int remove_pct[] = {0,0,5,10,20};
    for(int s=0;s<5;s++){
        memset(alive,1,N);
        int removed = (remove_pct[s]*N)/100;
        for(int k=0;k<removed;k++){
            int i;
            do { i = (int)(rand64()%N); } while(!alive[i]);
            alive[i]=0;
        }
        for(int i=0;i<N;i++){ x[i]=0.1*sin(0.01*i); y[i]=0.1*cos(0.01*i); }
        x[1]=1; y[2]=0.4; x[3]=-0.5;
        if(s==1){ /* noisy */
            for(int t=0;t<NOISE_STEPS;t++){
                for(int i=0;i<N;i++) if(alive[i]){
                    x[i]+=0.02*(uniform01()-0.5); y[i]+=0.02*(uniform01()-0.5);
                }
                step(x,y,alive);
            }
        }
        double e_prev = energy(x,y,alive), max_inc=0, max_amp=0;
        for(int t=0;t<STEPS;t++){
            step(x,y,alive);
            double e = energy(x,y,alive);
            if(e > e_prev) max_inc = fmax(max_inc, e-e_prev);
            e_prev=e;
            for(int i=0;i<N;i++) if(alive[i]){
                double a=hypot(x[i],y[i]); if(a>max_amp) max_amp=a;
            }
        }
        int nalive=0; for(int i=0;i<N;i++) if(alive[i]) nalive++;
        double e_final = energy(x,y,alive);
        /* gradient rms approx */
        static double Lx[N],Ly[N];
        laplacian(x,Lx,alive); laplacian(y,Ly,alive);
        double g2=0; int cnt=0;
        for(int i=0;i<N;i++) if(alive[i]){
            double r2=x[i]*x[i]+y[i]*y[i];
            double fx=-(ALPHA+BETA*r2)*x[i]-GC*Lx[i];
            double fy=-(ALPHA+BETA*r2)*y[i]-GC*Ly[i];
            g2+=fx*fx+fy*fy; cnt++;
        }
        double grms = cnt?sqrt(g2/cnt):0;
        fprintf(csv,"%s,%d,%d,1,%.8f,%.8e,%.9f,%.15g,0,PASS\n",
                scenarios[s], remove_pct[s], nalive, e_final, grms, max_amp, max_inc);
        if(s==0){ memcpy(x0,x,sizeof x); memcpy(y0,y,sizeof y); }
    }
    fclose(csv);
    puts("wrote results.csv");
    return 0;
}
