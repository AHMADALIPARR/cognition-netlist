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
  fprintf(out,"patterns,flip_percent,neuron_loss_percent,trials,perfect_recall_rate,mean_bit_accuracy,mean_sweeps,mean_energy,baseline_cue_accuracy\n");
  int8_t *mem=malloc((size_t)128*N);
  int8_t *state=malloc(N);
  uint8_t *alive=malloc(N);
  int32_t *overlap=malloc(128*sizeof(int32_t));
  if(!mem||!state||!alive||!overlap){fputs("oom\n",stderr);return 2;}
  int ok=1;
  for(int loss=0;loss<=20;loss+=10){
    for(int flip=5;flip<=10;flip+=5){
      for(int p=1;p<=32;p++){
        double sum_rate=0, sum_sweeps=0, sum_energy=0, sum_base=0;
        int perfect=0;
        for(int t=0;t<TRIALS;t++){
          memset(alive,1,N);
          int nrem=(loss*N)/100;
          for(int k=0;k<nrem;k++){
            int i; do{i=uniform(N);}while(!alive[i]); alive[i]=0;
          }
          patterns_fill(mem,p);
          memcpy(state,mem,N);
          int nflips=(flip*N)/100;
          for(int k=0;k<nflips;k++){ int i=uniform(N); if(alive[i]) state[i]=-state[i]; }
          int base_match=0; for(int i=0;i<N;i++) if(alive[i]&&state[i]==mem[i]) base_match++;
          int nalive=0; for(int i=0;i<N;i++) if(alive[i]) nalive++;
          double base_acc = nalive? (double)base_match/nalive : 0;
          int energy_ok=1;
          int sweeps=retrieve(mem,p,state,alive,overlap,&energy_ok);
          if(!energy_ok) ok=0;
          int matches=0; for(int i=0;i<N;i++) if(alive[i]&&state[i]==mem[i]) matches++;
          double rate = nalive?(double)matches/nalive:0;
          if(rate>0.999) perfect++;
          sum_rate+=rate; sum_sweeps+=sweeps; sum_base+=base_acc;
          double eng=0; for(int m=0;m<p;m++) eng -= 0.5*(double)overlap[m]*overlap[m]/(N);
          sum_energy+=eng;
        }
        fprintf(out,"%d,%d,%d,%d,%.6f,%.6f,%.3f,%.7f,%.6f\n",
          p,flip,loss,TRIALS,(double)perfect/TRIALS,sum_rate/TRIALS,sum_sweeps/TRIALS,sum_energy/TRIALS,sum_base/TRIALS);
      }
    }
  }
  fclose(out);
  printf("Energy-sweep nonincreasing-energy check: %s\n",ok?"PASS":"FAIL");
  puts("Outputs: capacity.csv; metric counts only surviving neurons. This is a binary Hopfield reference, not SPICE.");
  free(mem);free(state);free(alive);free(overlap);
  return ok?0:1;
}
