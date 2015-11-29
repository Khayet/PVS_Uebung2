#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM 32767                                             // Elementanzahl

// ---------------------------------------------------------------------------
// Vertausche zwei Zahlen im Feld v an der Position i und j

void swap(float *v, int i, int j)
{
    float t = v[i]; 
    v[i] = v[j];
    v[j] = t;
}

bool is_sorted(float *v) 
{
  for (int i = 1; i < NUM; i++) {
    if (v[i-1] > v[i]) {
      printf("v[i-1] = %f > %f = v[i] \n", v[i-1], v[i]);
      printf("Not sorted at iterators i-1 = %i and i = %i \n", i-1, i);
      return false;
    }
  }

  return true;
}

//
// ---------------------------------------------------------------------------
// Serielle Version von Quicksort (Wirth) 

void quicksort_s(float *v, int start, int end) 
{
    int i = start, j = end;
    float pivot;

    pivot = v[(start + end) / 2];                         // mittleres Element
    do {
        while (v[i] < pivot)
            i++;
        while (pivot < v[j])
            j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j);
            i++;
            j--;
        }
   } while (i <= j);
   if (start < j)                                        // Teile und herrsche
       quicksort_s(v, start, j);                      // Linkes Segment zerlegen
   if (i < end)
       quicksort_s(v, i, end);                       // Rechtes Segment zerlegen
}

void sort_serial(int number)
{
  float *v;
  v = (float *) calloc(NUM, sizeof(float));      

  int iter = number;
  double t_start, t_end;

  t_start = omp_get_wtime();
  for (int i = 0; i < iter; i++) {
      for (int j = 0; j < NUM; j++)
          v[j] = (float)rand();

      quicksort_s(v, 0, NUM-1);
  }
  t_end = omp_get_wtime();

  if (is_sorted(v)) {
    printf("Time for serial version: %f \n", t_end-t_start);
  } else {
    printf("Error: not sorted. \n");
  }
}

// ---------------------------------------------------------------------------
// Parallele Version 1 (mit tasks)

void quicksort_p1(float *v, int start, int end) 
{
  int i = start, j = end;
  float pivot;

  pivot = v[(start + end) / 2];
  do {
      while (v[i] < pivot)
          i++;
      while (pivot < v[j])
          j--;
      if (i <= j) {
          swap(v, i, j);
          i++;
          j--;
      }
 } while (i <= j);

  #pragma omp task
  if (start < j)
     quicksort_p1(v, start, j);

  #pragma omp task
  if (i < end)
     quicksort_p1(v, i, end);
}

void sort_parallel_v1(int number)
{
  float *v;
  v = (float *) calloc(NUM, sizeof(float));      

  int iter = number;
  double t_start, t_end;

  #pragma omp parallel
  {
    #pragma omp single
    {
      t_start = omp_get_wtime();
      for (int i = 0; i < iter; i++) {
          for (int j = 0; j < NUM; j++)
              v[j] = (float)rand();

          quicksort_p1(v, 0, NUM-1);
      }
      t_end = omp_get_wtime();
    }
  }

  if (is_sorted(v)) {
    printf("Time for parallel version 1: %f \n", t_end-t_start);
  } else {
    printf("Error: not sorted. \n");
  }

}

// ---------------------------------------------------------------------------
// Parallele Version 2 (mit sections)

void quicksort_p2(float *v, int start, int end, int depth) 
{
  int i = start, j = end;
  float pivot;

  pivot = v[(start + end) / 2];
  do {
      while (v[i] < pivot)
          i++;
      while (pivot < v[j])
          j--;
      if (i <= j) {
          swap(v, i, j);
          i++;
          j--;
      }
  } while (i <= j);

  #pragma omp parallel sections if (depth < 1)
  {
    #pragma omp section
    {
      if (start < j)
         quicksort_p2(v, start, j, depth+1);
    }

    #pragma omp section
    {
    if (i < end)
       quicksort_p2(v, i, end, depth+1);
    }
  }
}

void sort_parallel_v2(int number)
{
  float *v;
  v = (float *) calloc(NUM, sizeof(float));      

  int iter = number;
  double t_start, t_end;

/*  #pragma omp parallel
  {
    #pragma omp single
    {*/
      t_start = omp_get_wtime();
      for (int i = 0; i < iter; i++) {
          for (int j = 0; j < NUM; j++)
              v[j] = (float)rand();

          quicksort_p2(v, 0, NUM-1, 0);
      }
      t_end = omp_get_wtime();
/*    }
  }*/

  if (is_sorted(v)) {
    printf("Time for parallel version 2: %f \n", t_end-t_start);
  } else {
    printf("Error: not sorted. \n");
  }
}

// ---------------------------------------------------------------------------
// Hauptprogramm

int main(int argc, char *argv[])
{
    if (argc != 2) {                                      // Benutzungshinweis
        printf ("Vector sorting\nUsage: %s <NumIter>\n", argv[0]); 
        return 0;
    }

    int iter;                                                // Wiederholungen             
    iter = atoi(argv[1]);                               

    printf("Perform vector sorting %d times...\n", iter);
    sort_serial(iter);
    sort_parallel_v1(iter);
    sort_parallel_v2(iter);

    printf ("\nDone.\n");
    return 0;
}