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
    int flips=0;
    for(int i=0;i<N;i++){
      if(!*(alive+i))continue;
      int32_t field=0;
      for(int m=0;m<p;m++){
        const int8_t *ptr=mem+(size_t)m*N;
        int32_t o=*(overlap+m) - (*(ptr+i))*(*(state+i)); /* remove self */
        field += (*(ptr+i))*o;
      }
      int8_t newv = (field>0)?1:((field<0)?-1:*(state+i));
      if(newv!=*(state+i)){
        for(int m=0;m<p;m++){
          const int8_t *ptr=mem+(size_t)m*N;
          *(overlap+m) += (*(ptr+i))*(newv - *(state+i));
        }
        *(state+i)=newv;
        flips++;
      }
    }
    sweeps++;
    double obj=0.; for(int m=0;m<p;m++)obj+=(double)overlap[m]*overlap[m];
    if(obj+1e-9<prev_objective)*energy_ok=0;
    prev_objective=obj;
    if(!flips)break;
  }
  return sweeps;
}
int main(void){
  rng=0xC0FFEEULL;
  FILE *out=fopen("capacity.csv","w");
  if(!out){perror("capacity.csv");return 1;}
  fprintf(out,"patterns,trial,success_rate,mean_sweeps,energy_monotonic\n");
  int8_t *mem=malloc((size_t)256*N);
  int8_t *state=malloc(N);
  uint8_t *alive=malloc(N);
  int32_t *overlap=malloc(256*sizeof(int32_t));
  if(!mem||!state||!alive||!overlap){fputs("oom\n",stderr);return 2;}
  memset(alive,1,N);
  for(int p=1;p<=64;p+=1){
    for(int t=0;t<TRIALS;t++){
      patterns_fill(mem,p);
      /* corrupt first pattern slightly */
      memcpy(state,mem,N);
      int flips=(int)(0.1*N);
      for(int k=0;k<flips;k++){
        int i=uniform(N);
        state[i]=-state[i];
      }
      int energy_ok=1;
      int sweeps=retrieve(mem,p,state,alive,overlap,&energy_ok);
      int matches=0;
      for(int i=0;i<N;i++)if(state[i]==mem[i])matches++;
      double rate=(double)matches/N;
      fprintf(out,"%d,%d,%.6f,%d,%d\n",p,t,rate,sweeps,energy_ok);
    }
  }
  fclose(out);
  free(mem);free(state);free(alive);free(overlap);
  puts("wrote capacity.csv");
  return 0;
}
