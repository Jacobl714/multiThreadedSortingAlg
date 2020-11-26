#include "selectionSort.h"
#include <assert.h>
#include <stdio.h>




int* parSelSort(int* numbers,int n,int nt)
// creates a sorted version of numbers in a newly allocated array
{
  Segment segs[nt];
  int from = 0,sz = n / nt, rem = n % nt;
  for(int i=0;i < nt;i++) {
    Segment si = {numbers,from,from + sz + (i < rem)};
    segs[i] = si;
    from += sz + (i < rem);
  }
  Share share;
  share.best = MAXINT;
  share.winner = NULL;
  share.reporting = 0;
  share.needed    = nt;
  share.gen       = 0;
  share.res       = malloc(sizeof(int)*n);
  share.nbIn      = 0;
  pthread_mutex_init(&share.mtx,NULL);
  pthread_cond_init(&share.done,NULL);

  TArg ta[nt];
  for(int i=0;i<nt;i++) {
    ta[i].seg = segs + i;
    ta[i].shr = &share;
    pthread_create(&ta[i].name, NULL, sortThread,ta+i);
  }
  for(int i = 0; i < nt; i++)
    pthread_join(ta[i].name, NULL);
  return share.res;
}

int main(int argc,char* argv[])
{
  if (argc < 3) {
    printf("usage: selectionSort <arraySize> <NBThreads>\n");
    exit(-1);
  }
  int n = atoi(argv[1]);
  int nt = atoi(argv[2]);
  int ub = 1000000;
  int* numbers = malloc(sizeof(int)*n);
  int* h0 = calloc(ub,sizeof(int));
  for(int i = 0; i < n; i++) {
    numbers[i] = random() % ub;
    h0[numbers[i]] += 1;
  }

  int* resPtr = parSelSort(numbers,n,nt);
  MTArray* res = initArray(resPtr,n);
  // Print and check output correctness.
  printArray(res);
  checkArray(res);
  int* h1 = calloc(ub,sizeof(int));
  for(int i = 0; i < n; i++) {
    h1[res->numbers[i]] += 1;
  }
  for(int i=0;i < ub;i++) {
    if (h0[i] != h1[i]) {
      printf("Bad!\n");
      exit(2);
    }
  }
  free(h0);
  free(h1);
  free(numbers);
  free(resPtr);
  free(res);
  return 0;
}
