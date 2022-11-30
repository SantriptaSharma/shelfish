#include <stdio.h>
#include <pthread.h>

void* PrintInt(void *args)
{
    int *a = (int *)args;

    for (int i = a[0]; i <= a[1]; i++)
        printf("%d, ", i);

    printf("\n");

    
    return NULL;
}

int main()
{
    pthread_t thread1, thread2;
    int args1[] = {1, 100}, args2[] = {101, 200};
    
    if (pthread_create(&thread1, NULL, PrintInt, (void *)args1) || pthread_create(&thread2, NULL, PrintInt, (void *)args2))
    {
        printf("Threads Died \n");
        pthread_cancel(thread1);
        pthread_cancel(thread2);
        return 1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}