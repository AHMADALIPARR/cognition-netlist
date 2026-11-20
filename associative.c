/* Associative-memory capacity benchmark, C11, pointer arithmetic over contiguous pattern storage.
   Binary Hopfield associative memory with Hebbian low-rank weights, NO diagonal self-coupling.
   Not claimed equivalent to the continuous analog SPICE model.
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define N 1024
#define TRIALS 16
#define SWEEPS 24
static uint64_t rng;
static uint64_t next64(void){uint64_t x=rng; x^=x>>12;x^=x<<25;x^=x>>27; rng=x;return x*UINT64_C(2685821657736338717);}
static uint32_t uniform(uint32_t n){return (uint32_t)(next64()%n);}
static void patterns_fill(int8_t *restrict mem,int p){for(int8_t *q=mem;q<mem+(size_t)p*N;q++)*q=(next64()&1)?1:-1;}
/* Pointer arithmetic accesses the stored patterns at base + pattern*N + neuron. */
static void overlaps_init(const int8_t *mem,int p,const int8_t *state,const uint8_t *alive,int32_t *overlap){
  for(int m=0;m<p;m++){
    const int8_t *ptr=mem+(size_t)m*N;
    int32_t s=0;
    for(int i=0;i<N;i++)if(*(alive+i))s+=(*(ptr+i))*(*(state+i));
    *(overlap+m)=s;
  }
}
/* Maintain exact overlap sums; neuron update calculated with self-term removed. */
static int retrieve(const int8_t *mem,int p,int8_t *state,const uint8_t *alive,int32_t *overlap,int *energy_ok){
  overlaps_init(mem,p,state,alive,overlap);
  int sweeps=0;
  double prev_objective=0.; for(int m=0;m<p;m++)prev_objective+=(double)overlap[m]*overlap[m];
  for(int step=0;step<SWEEPS;step++){
    int changes=0;
    /* deterministic asynchronous permutation, one vertex each visit */
    int start=(int)uniform(N),stride=1+2*(int)uniform(N/2);
    for(int j=0;j<N;j++){
      int i=(start+j*stride)&(N-1);
      if(!alive[i])continue;
      const int old=state[i];
      int64_t field=0;
      for(int m=0;m<p;m++){
        int v=*(mem+(size_t)m*N+i);
        field+=(int64_t)v*(*(overlap+m)-v*old);
      }
      int newstate=field>0?1:field<0?-1:old;
      if(newstate!=old){
        *(state+i)=(int8_t)newstate;
        for(int m=0;m<p;m++)*(overlap+m)+=(*(mem+(size_t)m*N+i))*(newstate-old);
        changes++;
      }
    }
    double objective=0.;for(int m=0;m<p;m++)objective+=(double)overlap[m]*overlap[m];
    if(objective+1e-6<prev_objective)*energy_ok=0;
    prev_objective=objective;
    sweeps=step+1;
    if(!changes)break;
  }
  return sweeps;
}
static void mask(uint8_t *alive,int loss){memset(alive,1,N);int removed=loss*N/100;while(removed){int i=(int)uniform(N);if(alive[i]){alive[i]=0;removed--;}}}
static void corrupt(int8_t *state,const int8_t *target,const uint8_t *alive,int pct){
  memcpy(state,target,N);int live=0;for(int i=0;i<N;i++)live+=alive[i];
  int flips=pct*live/100;
  while(flips){int i=(int)uniform(N);if(alive[i] && state[i]==target[i]){state[i]=-state[i];flips--;}}
}
static double energy(const int32_t *overlap,int p,int live){
  double sum=0.;for(int m=0;m<p;m++)sum+=(double)overlap[m]*overlap[m];
  /* Exact Hopfield energy on surviving nodes, up to N normalization. */
  return -(sum-(double)p*live)/(2*N);
}
int main(void){
  static const int pvals[]={1,2,4,8,16,32,64};
  static const int noise[]={5,10,20,30,40};
  static const int loss[]={0,10,20};
  FILE *f=fopen("capacity.csv","w");if(!f){perror("capacity.csv");return 2;}
  fputs("patterns,flip_percent,neuron_loss_percent,trials,perfect_recall_rate,mean_bit_accuracy,mean_sweeps,mean_energy,baseline_cue_accuracy\n",f);
  int8_t *mem=malloc(64*N),*state=malloc(N);
  int32_t *ov=malloc(64*sizeof *ov);
  if(!mem||!state||!ov){fputs("allocation error\n",stderr);return 2;}
  uint8_t alive[N];int ok=1;
  for(size_t a=0;a<sizeof pvals/sizeof *pvals;a++){
    int p=pvals[a]; rng=UINT64_C(0x6250badaaaa5246)^((uint64_t)p<<22);patterns_fill(mem,p);
    for(size_t b=0;b<sizeof noise/sizeof *noise;b++) for(size_t c=0;c<sizeof loss/sizeof *loss;c++){
      int perfect=0;double correct_total=0,sweeps_total=0,energy_total=0,cue_total=0;
      for(int trial=0;trial<TRIALS;trial++){
        rng=UINT64_C(0xe83beef75a73) ^ ((uint64_t)p<<34) ^ ((uint64_t)noise[b]<<20) ^ ((uint64_t)loss[c]<<12) ^ (uint64_t)trial;
        const int8_t *target=mem+(size_t)uniform((uint32_t)p)*N;
        mask(alive,loss[c]);corrupt(state,target,alive,noise[b]);
        int live=0,cue_correct=0;for(int i=0;i<N;i++)if(alive[i]){live++;cue_correct+=(state[i]==target[i]);}
        cue_total+=(double)cue_correct/live;
        int energy_ok=1;int steps=retrieve(mem,p,state,alive,ov,&energy_ok);
        if(!energy_ok)ok=0;
        int correct=0;for(int i=0;i<N;i++)if(alive[i])correct+=(state[i]==target[i]);
        perfect+=(correct==live);correct_total+=(double)correct/live;
        sweeps_total+=steps;energy_total+=energy(ov,p,live);
        /* A candidate fixed-point must have non-increasing Hopfield energy. */
        if(!isfinite(energy_total)||steps<1||steps>SWEEPS)ok=0;
      }
      fprintf(f,"%d,%d,%d,%d,%.6f,%.6f,%.3f,%.7f,%.6f\n",p,noise[b],loss[c],TRIALS,
        (double)perfect/TRIALS,correct_total/TRIALS,sweeps_total/TRIALS,energy_total/TRIALS,cue_total/TRIALS);
    }
  }
  free(mem);free(state);free(ov);fclose(f);
  printf("Associative benchmark: %zu capacities x %zu corruption levels x %zu loss levels x %d trials = %zu runs\n",sizeof pvals/sizeof *pvals,sizeof noise/sizeof *noise,sizeof loss/sizeof *loss,TRIALS,(sizeof pvals/sizeof *pvals)*(sizeof noise/sizeof *noise)*(sizeof loss/sizeof *loss)*TRIALS);
  printf("Hopfield per-sweep nonincreasing-energy check: %s\n",ok?"PASS":"FAIL");
  puts("Outputs: capacity.csv; metric counts only surviving neurons. This is a binary Hopfield reference, not SPICE.");
  return ok?0:1;
}
