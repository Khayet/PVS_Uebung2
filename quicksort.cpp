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


//serielle Variante
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

// Variante 1: Parallelisierung mit tasks
void quicksort_v1(float *v, int start, int end)
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

     #pragma omp task
     if (start < j)                                        // Teile und herrsche
         quicksort_v1(v, start, j);                      // Linkes Segment zerlegen

     #pragma omp task
     if (i < end)
         quicksort_v1(v, i, end);                       // Rechtes Segment zerlegen
}

//Variante 2: Parallelisierung mit sections
void quicksort_v2(float *v, int start, int end)
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

   #pragma omp sections
   {
     #pragma omp section
     if (start < j) {                                        // Teile und herrsche
         quicksort_v2(v, start, j);
     }                      // Linkes Segment zerlegen

     #pragma omp section
     if (i < end) {
         quicksort_v2(v, i, end);                       // Rechtes Segment zerlegen
     }
   }
}

void quicksort_v3(float *v, int start, int end)
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
       quicksort_v3(v, start, j);                      // Linkes Segment zerlegen
   if (i < end)
       quicksort_v3(v, i, end);                       // Rechtes Segment zerlegen
}

void sort_serial(int iter) {
    float *v;                                                         // Feld

    v = (float *) calloc(NUM, sizeof(float));        // Speicher reservieren

    float start, end;

    start = omp_get_wtime();
    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++)      // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();

        quicksort_s(v, 0, NUM-1);                                  // Sortierung
    }
    end = omp_get_wtime();
    printf("Time for serial version: %f \n", end-start);
}

// ---------------------------------------------------------------------------
// Hauptprogramm

int main(int argc, char *argv[])
{
    float *v;                                                         // Feld
    int iter;                                                // Wiederholungen

    if (argc != 2) {                                      // Benutzungshinweis
        printf ("Vector sorting\nUsage: %s <NumIter>\n", argv[0]);
        return 0;
    }
    iter = atoi(argv[1]);
    v = (float *) calloc(NUM, sizeof(float));        // Speicher reservieren

    float start, end;

    printf("Perform vector sorting %d times...\n", iter);

    sort_serial(iter);
/*
    start = omp_get_wtime();
    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++)      // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();

        quicksort_s(v, 0, NUM-1);                                  // Sortierung
    }
    end = omp_get_wtime();
    printf("Time for serial version: %f \n", end-start);
*/

    start = omp_get_wtime();
    #pragma omp parallel
    {
      #pragma omp single
      {
        for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
            for (int j = 0; j < NUM; j++)      // Mit Zufallszahlen initialisieren
                v[j] = (float)rand();

            quicksort_v1(v, 0, NUM-1);                                  // Sortierung
        }
      }
    }
    end = omp_get_wtime();
    printf("Time for parallel version 1: %f \n", end-start);


    start = omp_get_wtime();
    #pragma omp parallel
    {
      #pragma omp single
      {
        for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
            for (int j = 0; j < NUM; j++)      // Mit Zufallszahlen initialisieren
                v[j] = (float)rand();

            quicksort_v2(v, 0, NUM-1);                                  // Sortierung
        }
      }
    }
    end = omp_get_wtime();
    printf("Time for parallel version 2: %f \n", end-start);


    start = omp_get_wtime();
    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++)      // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();

        quicksort_v3(v, 0, NUM-1);                                  // Sortierung
    }
    end = omp_get_wtime();
    printf("Time for parallel version 3: %f \n", end-start);



    printf ("\nDone.\n");
    return 0;
}
