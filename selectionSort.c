#include "selectionSort.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


   

void* sortThread(void* args)
{
   TArg* targ = args;
   Segment* me = targ->seg;
   Share*  shr = targ->shr;
   me->won  = 1;
   int best = MAXINT;
   int myGen=0;
   //while start less than end 
   while(me->from < me->to) {
       //set myGen to wave identifier 
       myGen = shr->gen;
       //if we won 
       if(me->won == 1) {
           //set best to be maxInt 
           best = MAXINT;
           //find which index has min value 
           for(int i = me->from; i<me->to; i++) {
               if(me->array[i] < best) {
                   best = me->array[i];
                   me->minAt = i;
               }
           }
           //no longer winner so set to 0
           me->won = 0;
       }
       //lock! (dont forget unlcok later)
       pthread_mutex_lock(&shr->mtx);
       
       //once at end, every num has been tested 
       //if current best < best in Share 
       if(best < shr->best) {
           shr->best = best;
           shr->winner = me;
       }
       //one more reported 
       shr->reporting++;
       
       //more reporting than needed 
       if(shr->reporting >= shr->needed){
           shr->res[shr->nbIn] = shr->best;
           shr->winner->won = 1; //tell thread it won 
           int t = shr->winner->array[shr->winner->from];
           shr->winner->array[shr->winner->from] = shr->winner->array[shr->winner->minAt];
           shr->winner->array[shr->winner->minAt] = t;
           
           //update vars
           shr->winner->from++;
           shr->nbIn++;
           shr->gen++;
           shr->best = MAXINT;
           shr->reporting = 0;
           pthread_cond_broadcast(&shr->done);
          
       }
       else{
           //wait if reported to early
           pthread_cond_wait(&shr->done, &shr->mtx);
           
       }
       //unlock the lock from above
       pthread_mutex_unlock(&shr->mtx);
       
     /*
       TODO: Implement this. 
      */
   }
   pthread_mutex_lock(&shr->mtx);
   shr->needed--;
   shr->reporting = 0;
   pthread_cond_broadcast(&shr->done);
   
   pthread_mutex_unlock(&shr->mtx);
   return NULL;
}

MTArray* initArray(int* numbers, int n)
{
  MTArray* array = malloc(sizeof(MTArray));
  array->numbers = numbers;
  array->size = n;
  return array;
}

void printArray(MTArray* array)
{
  for(int i = 0; i < array->size; i++)
    printf("%d ", array->numbers[i]);    
  printf("\n");
}
void checkArray(MTArray* a)
{
   int ok =  1;
   for(int i=0; ok && i < a->size - 1;i++)
      ok = ok && a->numbers[i] <= a->numbers[i+1];
   if (!ok) {
      printf("Not sorted....\n");
      exit(1);
   }
}
