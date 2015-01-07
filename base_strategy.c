#include "init/combinations.h"

#define EPS 0.0000001
#define COMBINATIONS (2*ALL_DEALS+DEALS_5)
#define START_DEAL_INDEX (ALL_DEALS-1)


int    choices[SLOTS][3*DEALS_5];
double weights[SLOTS][COMBINATIONS];
double slot_weights[SLOTS];


double get_choice_weight(int slots, int ci, int rolls)
{
  int chosen = -1, i, choice = deal_values[ci];
  double max = -1, cur;

  // all dice chosen
  if(ci < DEALS_5){
    if(rolls == 0){ // no rolls left

      int sl=slots, slot=0;
      // Maximize slot choice
      while(sl>0){
        if(sl % 2){
          cur = slot_weights[slots - (1<<slot)] + prices[slot][ci];
          if(cur > max+EPS){
            max = cur;
            chosen = slot;
          } 
        }
        slot++;
        sl = sl >> 1;
      }// while
      choices[slots][ci] = chosen;
      return max;
    }else{
      int new_choice;
      chosen = choice;
      max = weights[slots][(rolls-1)*ALL_DEALS+ci];
      // Maximize subdeal choice
      for(i=subdeals_counts[ci]-2; i>=0; i--){
        new_choice = subdeals[ci][i];
        cur = weights[slots][(rolls-1)*ALL_DEALS +
          get_deal_index(new_choice, DEALS_5, ALL_DEALS) ];
        if(cur > max+EPS){
          max = cur;
          chosen = new_choice;
        }
      } // for in subsets
      choices[slots][rolls*DEALS_5+ci] = chosen;
      return max;
    }// if rolls = 0
  }

  // some dice chosen
  int len = 5 - choice/MILLION;
  max = 0;
  for(i = deal_offsets[len]+deal_counts[len]-1; i >= deal_offsets[len]; i--){
    max += deal_probs[i] * weights[slots][rolls*ALL_DEALS +
      get_deal_index(choice + deal_values[i], 0, DEALS_5) ];
  }
  return max;
}

void fprint_data(int slots)
{
  FILE *f;
  char str[50];
  snprintf(str, 50, "data/bs_choices/c%03X.dat", slots);
  f = fopen(str, "w");

  int choice, rolls, i;
  for(i=DEALS_5-1; i>=0; i--){
    fprintf(f, "%6d\t%6d\t%6d\t%6d", deal_values[i] % MILLION,
      choices[slots][0*DEALS_5+i] % MILLION,
      choices[slots][1*DEALS_5+i] % MILLION,
      choices[slots][2*DEALS_5+i] % MILLION );
    fprintf(f, "\n");
  }
  fclose(f);
}

void calculate_strategy()
{
  int i, ir, slots, j;
  double w;

  slot_weights[0] = 0.0;

  for(j = 1; j < SLOTS; j++){
    slots = sorted_slots[j];
    for(ir=0; ir<COMBINATIONS; ir++)
      weights[slots][ir] = get_choice_weight(slots, ir % ALL_DEALS, ir / ALL_DEALS);

    slot_weights[slots] = get_choice_weight(slots, START_DEAL_INDEX, 2);
    printf("%4d %03X %9.5f  \n", j, slots, slot_weights[slots]);
    fprint_data(slots);
  }
}

int make_valid(int uv){
  int len=0, c = uv;
  while(c>0){
    len+=c%10;
    c/=10;
  }
  return len*MILLION + uv;
}

int main()
{
  load_comb_data();
  calculate_strategy();  
  return 0;
}