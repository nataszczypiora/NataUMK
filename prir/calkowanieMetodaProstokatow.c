/*
    Kompilacja: gcc -fopenmp calkowanieMetodaProstokatow.c -lm
*/

#include<stdio.h>
#include<omp.h>
#include<math.h>
#include<stdlib.h>

/*
    Funkcja którą będziemy całkować, PAMIĘTAĆ O PUNKTACH WYKLUCZONYCH Z DZIEDZINY
*/
double f(double x)
{
    return x;
}


double policzOdleglosc(double p, double k, int numThreads)
{
    if(k > p) return (k-p)/(double)numThreads;
    else return (p-k)/(double)numThreads;
}

int main(int argc, char* argv[])
{
    double precision = 0.0001;
    double poczatek, koniec;
    double odleglosc = 0;  
    double result = 0;
    // /^\ Bedzie tu policzony rozmiar kawałka wykresu 
    //  |  który każdy wątek będzie przerabiał
    int numThreads = 4; // <- Domyslnie beda 4 watki, jesli program bedzie uruchomiony bez argumentow
    if(argc > 1)
    {
        // Jesli w argumencie ./a.out poda sie liczbe, stanie sie ona liczba watkow
        // W przypadku podania argv[1] stringa będą dalej domyślnie 4 wątki
        numThreads = atoi(argv[1]);
    }

    // Pamietać o obsłudze błędów, jeżeli jakiś punkt z wprowadzonego zakresu nie należy do dziedziny
    scanf("%lf %lf", &poczatek, &koniec); 
    
    odleglosc = policzOdleglosc(poczatek, koniec, numThreads);
    printf("%lf\n\n", odleglosc);
    
    #pragma omp parallel num_threads(numThreads) shared(precision, result)
    {
        int thId = omp_get_thread_num();
        int thNum = omp_get_num_threads();
        // Dzielimy przedział na lokalne przedziały dla każdego watku
        double localPoczatek = poczatek + (((double)thId)*odleglosc);
        double localKoniec = koniec - (((double)(thNum-thId-1))*odleglosc); 
        double localPrec = precision;
        double localResult = 0;

        for(double i = localPoczatek;i<localKoniec;i+=localPrec)
        {
            localResult += f(i) * localPrec;
            printf("%lf\n", localResult);
        }
        

        

        #pragma omp critical
        result+=localResult;

        #pragma omp barrier
        if(thId==0)printf("Calka na podanym przedziale wnosi okolo %lf", result);
    }
    return 0;
}
