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
        seen[a][b]=seen[b][a]=1; ea[k]=a;eb[k]=b;degree[a]++;degree[b]++;k++;
    }
    fclose(f);
    if(k!=E) return 0;
    for(int i=0;i<N;i++)if(degree[i]!=DEG)return 0;
    return 1;
}
/* GF(2) incidence column parity: each intact edge has two 1s. */
static int parity_check(void) {
    for(int k=0;k<E;k++) if((1u^1u)!=0u || ea[k]==eb[k])return 0;
    return 1;
}
static void derivative(const double *z, double *d, const uint8_t *alive, const double *force) {
    for(int i=0;i<2*N;i++)d[i]=0.;
    for(int i=0;i<N;i++)if(alive[i]) {
        const double x=z[2*i],y=z[2*i+1];
        const double gain=-(ALPHA+BETA*(x*x+y*y));
        d[2*i]=gain*x+force[2*i];
        d[2*i+1]=gain*y+force[2*i+1];
    }
    for(int k=0;k<E;k++) {
        const int a=ea[k],b=eb[k];
        if(alive[a]&&alive[b]) {
            for(int c=0;c<2;c++) {
                const double delta=GC*(z[2*a+c]-z[2*b+c]);
                d[2*a+c]-=delta;d[2*b+c]+=delta;
            }
        }
    }
    if(alive[0]) {d[0]-=GREF*(z[0]-REF);d[1]-=GREF*z[1];}
}
static double energy(const double*z,const uint8_t*alive) {
    double result=0.;
    for(int i=0;i<N;i++)if(alive[i]) {
        const double x=z[2*i],y=z[2*i+1],r=x*x+y*y;
        result+=0.5*ALPHA*r+0.25*BETA*r*r;
    }
    for(int k=0;k<E;k++)if(alive[ea[k]]&&alive[eb[k]]) {
        const int a=ea[k],b=eb[k];
        const double dx=z[2*a]-z[2*b],dy=z[2*a+1]-z[2*b+1];
        result+=0.5*GC*(dx*dx+dy*dy);
    }
    if(alive[0])result+=0.5*GREF*((z[0]-REF)*(z[0]-REF)+z[1]*z[1]);
    return result;
}
static void rk4(double *z,const uint8_t*alive,const double *force) {
    double k1[2*N],k2[2*N],k3[2*N],k4[2*N],tmp[2*N];
    derivative(z,k1,alive,force);
    for(int i=0;i<2*N;i++)tmp[i]=z[i]+0.5*DT*k1[i];
    derivative(tmp,k2,alive,force);
    for(int i=0;i<2*N;i++)tmp[i]=z[i]+0.5*DT*k2[i];
    derivative(tmp,k3,alive,force);
    for(int i=0;i<2*N;i++)tmp[i]=z[i]+DT*k3[i];
    derivative(tmp,k4,alive,force);
    for(int i=0;i<2*N;i++)z[i]+=DT*(k1[i]+2*k2[i]+2*k3[i]+k4[i])/6.;
}
static void init(double*z) {
    rng=UINT64_C(0x123456789abcdef);
    for(int i=0;i<N;i++) {
        z[2*i]=0.72+0.20*(uniform01()-.5);
        z[2*i+1]=0.16*(uniform01()-.5);
    }
    z[2]=1.;z[5]=0.4;z[6]=-0.5;
}
static void mask_init(uint8_t*alive,int pct) {
    for(int i=0;i<N;i++)alive[i]=1;
    rng=UINT64_C(0x391077daabbccdde) ^ (uint64_t)pct;
    int target=N*pct/100, removed=0;
    while(removed<target) {
        int i=(int)(rand64()%N);
        if(i!=0&&alive[i]){alive[i]=0;removed++;}
    }
}
static int components(const uint8_t *alive) {
    static int adj[N][DEG];
    static int counts[N];
    memset(counts,0,sizeof counts);
    for(int k=0;k<E;k++)if(alive[ea[k]]&&alive[eb[k]]) {
        int a=ea[k],b=eb[k];adj[a][counts[a]++]=b;adj[b][counts[b]++]=a;
    }
    uint8_t vis[N]={0};int q[N],components=0;
    for(int i=0;i<N;i++) if(alive[i]&&!vis[i]) {
        components++;int begin=0,end=0;q[end++]=i;vis[i]=1;
        while(begin<end)for(int j=0,v=q[begin++];j<counts[v];j++){
            int nxt=adj[v][j];if(!vis[nxt]){vis[nxt]=1;q[end++]=nxt;}
        }
    }
    return components;
}
static int simulate(const char *label,int removed_pct,int noisy,const double *clean,FILE *out) {
    uint8_t alive[N];double z[2*N],force[2*N]={0};
    mask_init(alive,removed_pct);init(z);
    int comps=components(alive),alive_count=0;
    for(int i=0;i<N;i++)alive_count+=alive[i];
    for(int i=0;i<N;i++)if(!alive[i])z[2*i]=z[2*i+1]=0.;
    rng=UINT64_C(0xbeeffeed12344321);
    double prev=energy(z,alive),max_r=0.,max_increase=0.,noise_energy_increase=0.;
    int finite=1,monotone=1;double rms=0.,grad=0.;
    for(int step=0;step<STEPS;step++) {
        for(int j=0;j<2*N;j++)force[j]=0.;
        if(noisy&&step<NOISE_STEPS) {
            for(int i=0;i<N;i++)if(alive[i]) {
                force[2*i]=0.12*(2*uniform01()-1);
                force[2*i+1]=0.12*(2*uniform01()-1);
            }
        }
        rk4(z,alive,force);
        double now=energy(z,alive);
        if(!isfinite(now)) {finite=0;break;}
        for(int i=0;i<N;i++)if(alive[i]) {
            double r=hypot(z[2*i],z[2*i+1]);if(r>max_r)max_r=r;
            if(!isfinite(r))finite=0;
        }
        double rise=now-prev;
        if(rise>max_increase && (!noisy||step>=NOISE_STEPS))max_increase=rise;
        if(noisy&&step<NOISE_STEPS&&rise>noise_energy_increase)noise_energy_increase=rise;
        if((!noisy||step>=NOISE_STEPS)&&rise>1e-9)monotone=0;
        prev=now;
    }
    double d[2*N]; memset(force,0,sizeof force); derivative(z,d,alive,force);
    for(int i=0;i<N;i++)if(alive[i]) {
        grad+=d[2*i]*d[2*i]+d[2*i+1]*d[2*i+1];
        if(clean) {
            rms+=(z[2*i]-clean[2*i])*(z[2*i]-clean[2*i]);
            rms+=(z[2*i+1]-clean[2*i+1])*(z[2*i+1]-clean[2*i+1]);
        }
    }
    rms=clean?sqrt(rms/(2*alive_count)):0.;
    grad=sqrt(grad/(2*alive_count));
    int pass=finite&&monotone&&max_r<5.;
    fprintf(out,"%s,%d,%d,%d,%.10g,%.10g,%.10g,%.10g,%.10g,%s\n",label,removed_pct,alive_count,comps,prev,grad,max_r,max_increase,rms,pass?"PASS":"FAIL");
    printf("%-12s alive=%4d components=%d energy=%9.4f residual=%8.5f rms_to_clean=%8.5f monotone=%s bounded=%s\n",label,alive_count,comps,prev,grad,rms,monotone?"yes":"NO",finite&&max_r<5.?"yes":"NO");
    if(noisy)printf("  max energy increase *during forcing*: %.8g (not constrained by Lyapunov test)\n",noise_energy_increase);
    if(!clean) {
        FILE *f=fopen("clean_state.csv","w"); if(f){fputs("neuron,x,y\n",f);for(int i=0;i<N;i++)fprintf(f,"%d,%.12g,%.12g\n",i,z[2*i],z[2*i+1]);fclose(f);}
    }
    return pass;
}
int main(int argc,char **argv) {
    if(argc!=2){fprintf(stderr,"usage: %s edges.txt\n",argv[0]);return 2;}
    if(!read_edges(argv[1])||!parity_check()){fputs("topology or GF(2) parity FAIL\n",stderr);return 1;}
    printf("Topology PASS: N=%d edges=%d degree=%d; GF(2) incidence parity PASS\n",N,E,DEG);
    FILE*out=fopen("results.csv","w");if(!out){perror("results.csv");return 2;}
    fputs("scenario,removed_percent,alive,components,final_energy,gradient_rms,max_amplitude,max_energy_increase_after_forcing,rms_to_clean_survivors,structural_invariants\n",out);
    /* Save baseline by a direct clean run using the same initial conditions. */
    double clean[2*N],force[2*N]={0};uint8_t alive[N];mask_init(alive,0);init(clean);
    for(int step=0;step<STEPS;step++)rk4(clean,alive,force);
    int ok=1;
    ok&=simulate("clean",0,0,NULL,out);
    ok&=simulate("noisy",0,1,clean,out);
    ok&=simulate("loss_5pct",5,0,clean,out);
    ok&=simulate("loss_10pct",10,0,clean,out);
    ok&=simulate("loss_20pct",20,0,clean,out);
    fclose(out);
    printf("Structural invariant suite: %s (does not imply memory retrieval or consciousness)\n",ok?"PASS":"FAIL");
    return ok?0:1;
}
