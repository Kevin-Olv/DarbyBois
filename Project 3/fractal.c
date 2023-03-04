/*
fractal.c - Mandelbrot fractal generation
Starting code for CSE 30341 Project 3 - Spring 2023
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "bitmap.h"
#include "fractal.h"

/*
Compute the number of iterations at point x, y
in the complex space, up to a maximum of maxiter.
Return the number of iterations at that point.

This example computes the Mandelbrot fractal:
z = z^2 + alpha

Where z is initially zero, and alpha is the location x + iy
in the complex plane.  Note that we are using the "complex"
numeric type in C, which has the special functions cabs()
and cpow() to compute the absolute values and powers of
complex values.
*/

static int compute_point( double x, double y, int max )
{
	double complex z = 0;
	double complex alpha = x + I*y;

	int iter = 0;

	while( cabs(z)<4 && iter < max ) {
		z = cpow(z,2) + alpha;
		iter++;
	}

	return iter;
}

/*
Compute an entire image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax).

HINT: Generally, you will want to leave this code alone and write your threaded code separately

*/

void compute_image_singlethread ( struct FractalSettings * pSettings, struct bitmap * pBitmap)
{
	int i,j;

	// For every pixel i,j, in the image...

	for(j=0; j<pSettings->nPixelHeight; j++) {
		for(i=0; i<pSettings->nPixelWidth; i++) {

			// Scale from pixels i,j to coordinates x,y
			double x = pSettings->fMinX + i*(pSettings->fMaxX - pSettings->fMinX) / pSettings->nPixelWidth;
			double y = pSettings->fMinY + j*(pSettings->fMaxY - pSettings->fMinY) / pSettings->nPixelHeight;

			// Compute the iterations at x,y
			int iter = compute_point(x,y,pSettings->nMaxIter);

			// Convert a iteration number to an RGB color.
			// (Change this bit to get more interesting colors.)
			int gray = 255 * iter / pSettings->nMaxIter;

            // Set the particular pixel to the specific value
			// Set the pixel in the bitmap.
			bitmap_set(pBitmap,i,j,gray);
		}
	}
}

struct ThreadInfo {
    struct FractalSettings *pSettings;
    struct bitmap *pBitmap;
    int startRow;
    int endRow;
};

void* compute_image_multithreaded(void* arg) {
	struct ThreadInfo* pInfo = (struct ThreadInfo*) arg;
	struct FractalSettings* pSettings = pInfo->pSettings;
	struct bitmap* pBitmap = pInfo->pBitmap;
	int startRow = pInfo->startRow;
	int endRow = pInfo->endRow;

	// For every pixel i,j, in the image...
	for (int j = startRow; j < endRow; j++) {
		for (int i = 0; i < pSettings->nPixelWidth; i++) {

			// Scale from pixels i,j to coordinates x,y
			double x = pSettings->fMinX + i*(pSettings->fMaxX - pSettings->fMinX) / pSettings->nPixelWidth;
			double y = pSettings->fMinY + j*(pSettings->fMaxY - pSettings->fMinY) / pSettings->nPixelHeight;

			// Compute the iterations at x,y
			int iter = compute_point(x, y, pSettings->nMaxIter);

			// Convert a iteration number to an RGB color.
			// (Change this bit to get more interesting colors.)
			int gray = 255 * iter / pSettings->nMaxIter;

			// Set the pixel in the bitmap.
			bitmap_set(pBitmap, i, j, gray);
		}
	}

	pthread_exit(NULL);
}



/* Process all of the arguments as provided as an input and appropriately modify the
   settings for the project 
   @returns 1 if successful, 0 if unsuccessful (bad arguments) */
char processArguments (int argc, char * argv[], struct FractalSettings * pSettings)
{
    /* If we don't process anything, it must be successful, right? */
    return 1;
}



void displayHelp()
{
    printf("Usage: fractal [options]\n");
    printf("Options:\n");
    printf("  -help              Display this information\n");
    printf("  -xmin X            New value for x min\n");
    printf("  -xmax X            New value for x max\n");
    printf("  -ymin Y            New value for y min\n");
    printf("  -ymax Y            New value for y max\n");
    printf("  -maxiter N         New value for the maximum number of iterations (must be an integer)\n");
    printf("  -width W           New width for the output image\n");
    printf("  -height H          New height for the output image\n");
    printf("  -output F          New name for the output file\n");
    printf("  -threads N         Number of threads to use for processing (default is 1)\n");
}

int main( int argc, char *argv[] )
{
    struct FractalSettings  theSettings;

	// The initial boundaries of the fractal image in x,y space.
    theSettings.fMinX = DEFAULT_MIN_X;
    theSettings.fMaxX = DEFAULT_MAX_X;
    theSettings.fMinY = DEFAULT_MIN_Y;
    theSettings.fMaxY = DEFAULT_MAX_Y;
    theSettings.nMaxIter = DEFAULT_MAX_ITER;

    theSettings.nPixelWidth = DEFAULT_PIXEL_WIDTH;
    theSettings.nPixelHeight = DEFAULT_PIXEL_HEIGHT;

    theSettings.nThreads = DEFAULT_THREADS;
    theSettings.theMode  = MODE_THREAD_SINGLE;
    
    strncpy(theSettings.szOutfile, DEFAULT_OUTPUT_FILE, MAX_OUTFILE_NAME_LEN);

    /* 
        TODO:
        Support for setting the number of threads is optional

        You may also appropriately apply reasonable minimum / maximum values (e.g. minimum image width, etc.)
    */


   /* Are there any locks to set up? */

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-help") == 0) { //Display help
            displayHelp();
            return 0;
        } else if (strcmp(argv[i], "-xmin") == 0) { //New value for x min
            theSettings.fMinX = atof(argv[++i]);
        } else if (strcmp(argv[i], "-xmax") == 0) { //New value for x max
            theSettings.fMaxX = atof(argv[++i]);
        } else if (strcmp(argv[i], "-ymin") == 0) { //New value for y min
            theSettings.fMinY = atof(argv[++i]);
        } else if (strcmp(argv[i], "-ymax") == 0) { //New value for y max
            theSettings.fMaxY = atof(argv[++i]);
        } else if (strcmp(argv[i], "-maxiter") == 0) { // New value for the maximum number of iterations (must be an integer) 
            theSettings.nMaxIter = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-width") == 0) { //New width for the output image
            theSettings.nPixelWidth = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-height") == 0) { //New height for the output image
            theSettings.nPixelHeight = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-output") == 0) { //New name for the output file
            strncpy(theSettings.szOutfile, argv[++i], MAX_OUTFILE_NAME_LEN);
        } else if (strcmp(argv[i], "-threads") == 0) { //Number of threads to use for processing (default is 1)
            theSettings.nThreads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-row") == 0) { //Run using a row-based approach
            theSettings.theMode = MODE_THREAD_ROW;
        } else if (strcmp(argv[i], "-task") == 0) { //Run using a thread-based approach
            theSettings.theMode = MODE_THREAD_TASK;
        } else {
            printf("Unknown argument: %s, use -help for more information\n", argv[i]);
        }
    }
   if(processArguments(argc, argv, &theSettings))
   {
        /* Dispatch here based on what mode we might be in */
        if(theSettings.theMode == MODE_THREAD_SINGLE)
        {
            /* Create a bitmap of the appropriate size */
            struct bitmap * pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

            /* Fill the bitmap with dark blue */
            bitmap_reset(pBitmap,MAKE_RGBA(0,0,255,0));

            /* Compute the image */
            compute_image_singlethread(&theSettings, pBitmap);

            // Save the image in the stated file.
            if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
                fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
                return 1;
            }            
        }
        else if(theSettings.theMode == MODE_THREAD_ROW)
        {
            /* A row-based approach will not require any concurrency protection */

            /* Could you send an argument and write a different version of compute_image that works off of a
               certain parameter setting for the rows to iterate upon? */
               /* Create a bitmap of the appropriate size */
            struct bitmap * pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

            /* Fill the bitmap with dark blue */
            bitmap_reset(pBitmap,MAKE_RGBA(0,0,255,0));

            /* Compute the image */
            int numThreads = atoi(argv[2]);
            int rowsPerThread = (int) ceil((double) theSettings.nPixelHeight / numThreads);

            pthread_t threads[numThreads];
            struct ThreadInfo threadInfo[numThreads];

            for (int i = 0; i < numThreads; i++) {
                threadInfo[i].pSettings = &theSettings;
                threadInfo[i].pBitmap = pBitmap;
                threadInfo[i].startRow = i * rowsPerThread;
                threadInfo[i].endRow = (i + 1) * rowsPerThread;
                if (i == numThreads - 1) {
                    threadInfo[i].endRow = theSettings.nPixelHeight;
                }

                pthread_create(&threads[i], NULL, compute_image_multithreaded, (void*) &threadInfo[i]);
            }

            for (int i = 0; i < numThreads; i++) {
                pthread_join(threads[i], NULL);
            }

            // Save the image in the stated file.
            if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
                fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
                return 1;
            } 
        }
        else if(theSettings.theMode == MODE_THREAD_TASK)
        {
            /* For the task-based model, you will want to create some sort of a way that captures the instructions
               or task (perhaps say a startX, startY and stopX, stopY in a struct).  You can have a global array 
               of the particular tasks with each thread attempting to pop off the next task.  Feel free to tinker 
               on what the right size of the work unit is but 20x20 is a good starting point.  You are also welcome
               to modify the settings struct to help you out as well.  
               
               Generally, it will be good to create all of the tasks into that array and then to start your threads
               with them in turn attempting to pull off a task one at a time.  
               
               While we could do condition variables, there is not really an ongoing producer if we create all of
               the tasks at the outset. Hence, it is OK whenever a thread needs something to do to try to access
               that shared data structure with all of the respective tasks.  
               */
        }
        else 
        {
            /* Uh oh - how did we get here? */
        }
   }
   else
   {
        /* Probably a great place to dump the help */

        /* Probably a good place to bail out */
        exit(-1);
   }

    /* TODO: Do any cleanup as required */

	return 0;
}
