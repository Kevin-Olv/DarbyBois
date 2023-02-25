#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

char g_bKeepLooping = 1;

#define MAX_THREADS 5

struct ChimeThreadInfo
{
    int        nIndex;
    float      fChimeInterval;
    char       bIsValid;
    pthread_t  ThreadID;
};

struct ChimeThreadInfo  TheThreads[MAX_THREADS];

pthread_mutex_t mutex;

void * ThreadChime (void * pData)
{
    struct ChimeThreadInfo  * pThreadInfo;

    /* Which chime are we? */
    pThreadInfo = (struct ChimeThreadInfo *) pData;

    while(g_bKeepLooping)
    {
        sleep(pThreadInfo->fChimeInterval);
        printf("Ding - Chime %d with an interval of %f s!\n", pThreadInfo->nIndex, pThreadInfo->fChimeInterval);
    }

    return NULL;
}

#define BUFFER_SIZE 1024

int main (int argc, char *argv[])
{
    char szBuffer[BUFFER_SIZE];

    /* Set all of the thread information to be invalid (none allocated) */
    for(int j=0; j<MAX_THREADS; j++)
    {
        TheThreads[j].bIsValid = 0;
    }

    pthread_mutex_init(&mutex, NULL);

    while(1)
    {
        /* Prompt and flush to stdout */
        printf("CHIME>");
        fflush(stdout);

        /* Wait for user input via fgets */
        fgets(szBuffer, BUFFER_SIZE, stdin);

        /* If the command is quit - join any active threads and finish up gracefully */
        if(strcmp(szBuffer, "quit\n") == 0 || strcmp(szBuffer, "exit\n") == 0)
        {
            g_bKeepLooping = 0;

            for(int j=0; j<MAX_THREADS; j++)
            {
                if(TheThreads[j].bIsValid)
                {
                    printf("Joining chime %d (Thread %lu)\n", j, (long)TheThreads[j].ThreadID);
                    pthread_join(TheThreads[j].ThreadID, NULL);
                    printf("Join complete for Chime %d\n", j);
                }
            }
            printf("Exit chime program\n");
            break;
        }
        /* If the command is chime, the second argument is the chime number (integer) and the 
           third number is the new interval (floating point). If necessary, start the thread
           as needed */
        else if(strncmp(szBuffer, "chime", 5) == 0)
        {
            int nIndex;
            float fChimeInterval;

            if(sscanf(szBuffer + 5, "%d %f", &nIndex, &fChimeInterval) == 2)
            {
                if(nIndex < 0 || nIndex >= MAX_THREADS)
                {
                    printf("Cannot adjust chime %d, out of range\n", nIndex); //Out of range error
                    continue;
                }

                pthread_mutex_lock(&mutex);

                if(TheThreads[nIndex].bIsValid)
                {
                    printf("Adjusting chime %d to have an interval of %f s\n", nIndex, fChimeInterval);
                    TheThreads[nIndex].fChimeInterval = fChimeInterval;
                }
                else
                {
                    printf("Starting thread %lu for chime %d, interval of %f s\n", (long)TheThreads[nIndex].ThreadID, nIndex, fChimeInterval);
                    TheThreads[nIndex].nIndex = nIndex;
                    TheThreads[nIndex].fChimeInterval = fChimeInterval;
                    TheThreads[nIndex].bIsValid = 1;
                    pthread_create(&TheThreads[nIndex].ThreadID, NULL, ThreadChime, &TheThreads[nIndex]);
                }

                pthread_mutex_unlock(&mutex);
            }
        }
        else
        {
            printf("Unknown command: %s\n", szBuffer);
        }
        
    }
    return 0;
}
