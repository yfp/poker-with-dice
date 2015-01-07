#include <stdio.h>
#include <stdlib.h>

#define DEALS_5 252
#define ALL_DEALS 462
#define SLOTS 4096
#define MAX_POINTS 292
#define MILLION 1000000


#define INIT_VALUE (-1)

int     deal_offsets[6];
int     deal_counts[6] = {1, 6, 21, 56, 126, 252};
int     deal_values[ALL_DEALS];
double  deal_probs[ALL_DEALS];

int prices[12][DEALS_5];

int subdeals_counts[DEALS_5];
int subdeals[DEALS_5][32];

int sorted_slots[SLOTS];

void assert_deal(int i, int deal){
  if(deal == deal_values[i])
    return;
  printf("Wrong deal: deal[%d] != %d", i, deal);
  exit(5);
}

int get_deal_index(int deal, int start, int end){
  int i = (start+end)/2;
  if(deal == deal_values[i])
    return i;
  if(i == start){
    printf("Deal search error: %d\n", deal);
    exit(-1);
  }
  if(deal < deal_values[i])
    return get_deal_index(deal, i+1, end);
  else return get_deal_index(deal, start, i);

}

void load_deals()
{
  FILE *f = fopen("init/probabilities.dat", "r");
  int out=0, i, i_other=0;

  for(i=5; i>=0; i--){
    deal_offsets[i] = out;
    out+= deal_counts[i];
  }

  int deal, prob;
  for(i=0; i<ALL_DEALS; i++){
    out = fscanf(f, "%d %d", &deal, &prob);
    deal_probs [i] = 1.0*prob/7776;
    deal_values[i] = deal;
  }
  fclose(f);
}

void load_prices()
{
  int i, s, deal, di;
  FILE *f = fopen("init/prices.dat", "r");
  for (i = 0; i < DEALS_5; i++){
    fscanf(f, "%d", &deal);
    assert_deal(i, deal);
    for(s=0; s<12; s++)
      fscanf(f, "%d", &prices[11-s][i]);
  }
  fclose(f);
}

void load_subdeals()
{
  FILE *f = fopen("init/subdeals.dat", "r");
  int i, j, deal, count;
  for(i=0; i<DEALS_5; i++){
    fscanf(f, "%d %d", &deal, &subdeals_counts[i]);
    assert_deal(i, deal);
    for(j=0; j<subdeals_counts[i]; j++)
      fscanf(f, "%d", &subdeals[i][j]);
  }
  fclose(f);
}

int slots_len(int slots)
{
  int len=0;
  while(slots > 0){
    len += slots%2;
    slots = slots >> 1;
  }
  return len;
}

void sort_slots()
{
  int len, i, n=0;
  for(len=0; len<=12; len++)
    for(i=0; i<SLOTS; i++)
      if(slots_len(i) == len){
        sorted_slots[n] = i;
        n++;
      }
}

void load_comb_data(){
  load_deals();
  load_prices();
  load_subdeals();
  sort_slots();
}