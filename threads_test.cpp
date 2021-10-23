#include <iostream>
#include <exception>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *thread1_func(void*){
    for (int i = 0; i < 10; i++){
        cout << "Thread1" << endl;
        sleep(1);
    }
    pthread_exit(NULL);
}

void *thread2_func(void*){
    usleep(500*1000);
    for (int i = 0; i < 10; i++){
        cout << "Thread2" << endl;
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread1_func, NULL);
    pthread_create(&thread2, NULL, thread2_func, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    // pthread_exit(NULL);
    sleep(2);
    cout << "main end" << endl;
    return 0;
}