#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2\SDL.h>

#include "maths.h"

//#include "winconwrapper.h"

#define HEIGHT 28
#define WIDTH 28
#define SCALE 10

#define TRAINING_DATA "train-images.idx3-ubyte"
#define TRAINING_NUMBER 0x00000803

#define LABEL_DATA "train-labels.idx1-ubyte"
#define LABEL_NUMBER 0x00000801

bool init_SDL();

bool quit_SDL();

bool loadTrainingData();
bool loadLabelData();

int getImage(int index, float* image);

void drawImage(int index);

SDL_Window* window;
SDL_Renderer* renderer;

SDL_Texture* texture;

Uint8* trainingData;
int trainingCount;

Uint8* labelData;
int labelCount;

float image[WIDTH * HEIGHT]; //TODO: replace with neural init


int SDL_main(int argc, char** argv)
{
	setbuf(stdout, NULL);
	//Init, checks for fail
	if(!init_SDL())
	{
		printf("Failed to initialize; exiting\n");
		return 1;
	}

	if(!loadTrainingData())
	{
		printf("Failed to load data; exiting\n");
		quit_SDL();
		return 2;
	}
	if(!loadLabelData())
	{
		printf("Failed to load label; exiting\n");
		free(trainingData);
		quit_SDL();
		return 3;
	}

	int n = 28 * 28;
	Noise noise = {true, 0.f};
	for(int i = 0; i < 30; ++i)
	{
		printf("%f\n", gaussianNoisef(0, sqrt(2.f/(float)n), &noise));
		drawImage(i);
		SDL_Delay(200);
	}

	/*
	for(int i = 0; i < 20; ++i)
	{
		SDL_PumpEvents();
		int label = getImage(i, image);

		printf("%d\n", label);

		drawImage(i);
		SDL_Delay(1000);
	}*/


	printf("Bleh: %s\n", SDL_GetError());
	SDL_Delay(2000);
	free(labelData);
	free(trainingData);
	quit_SDL();
	return 0;
}


//Init function starts everything
bool init_SDL()
{
	//init SDL
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		return false;
	}

	//Create window
	window = SDL_CreateWindow
	(
		"SDL2 window",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIDTH  * SCALE,
		HEIGHT * SCALE,
		0
	);
	if(!window)
	{
		SDL_Quit();
		return false;
	}

	//Create 2d renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(!renderer)
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	texture = SDL_CreateTexture
	(
		renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );
	if(!texture)
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	return true;
}

//Quit function stops everything
bool quit_SDL()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool loadTrainingData()
{
	SDL_RWops* file = SDL_RWFromFile(TRAINING_DATA, "rb");
	if(!file)
	{
		return false;
	}

	int magicNumber;
	int read = SDL_RWread(file, &magicNumber, sizeof(int), 1);
	magicNumber = SDL_Swap32(magicNumber);
	if(magicNumber != TRAINING_NUMBER || !read)
	{
		SDL_RWclose(file);
		return false;
	}

	int dimensions[3];
	read = SDL_RWread(file, &dimensions, sizeof(int), 3);
	if(read != 3)
	{
		SDL_RWclose(file);
		return false;
	}

	dimensions[0] = SDL_Swap32(dimensions[0]);
	dimensions[1] = SDL_Swap32(dimensions[1]);
	dimensions[2] = SDL_Swap32(dimensions[2]);
	trainingCount = dimensions[0];
	if(dimensions[1] != WIDTH || dimensions[2] != HEIGHT)
	{
		SDL_RWclose(file);
		return false;
	}

	trainingData = malloc(sizeof(Uint8) * dimensions[0] * dimensions[1] * dimensions[2]);
	if(!trainingData)
	{
		SDL_RWclose(file);
		return false;
	}

	read = SDL_RWread(file, trainingData, sizeof(Uint8), dimensions[0] * dimensions[1] * dimensions[2]);
	if(read != dimensions[0] * dimensions[1] * dimensions[2])
	{
		free(trainingData);
		SDL_RWclose(file);
		return false;
	}
	
	SDL_RWclose(file);
	return true;
}

bool loadLabelData()
{
	SDL_RWops* file = SDL_RWFromFile(LABEL_DATA, "rb");
	if(!file)
	{
		return false;
	}

	int magicNumber;
	int read = SDL_RWread(file, &magicNumber, sizeof(int), 1);
	magicNumber = SDL_Swap32(magicNumber);
	if(magicNumber != LABEL_NUMBER || !read)
	{
		SDL_RWclose(file);
		return false;
	}

	int count;
	read = SDL_RWread(file, &count, sizeof(int), 1);
	if(!read)
	{
		SDL_RWclose(file);
		return false;
	}
	count = SDL_Swap32(count);
	labelCount = count;

	labelData = malloc(sizeof(Uint8) * count);
	if(!labelData)
	{
		SDL_RWclose(file);
		return false;
	}

	read = SDL_RWread(file, labelData, sizeof(Uint8), count);
	if(read != count)
	{
		free(labelData);
		SDL_RWclose(file);
		return false;
	}

	SDL_RWclose(file);
	return true;
}

void drawImage(int index)
{
	SDL_RenderClear(renderer);

	Uint8* pixels = NULL;
	int pitch;

	SDL_LockTexture(texture, NULL, (void*)&pixels, &pitch);

	int bytes = pitch / WIDTH;
	int width = bytes * WIDTH;

	int i, j, k = index;
	for(j = 0; j < HEIGHT; ++j)
	{
		for(i = 0; i < WIDTH; i++)
		{
			pixels[i * bytes + 1 + j * width] = trainingData[i + j * WIDTH + k * WIDTH * HEIGHT];
			pixels[i * bytes + 2 + j * width] = trainingData[i + j * WIDTH + k * WIDTH * HEIGHT];
			pixels[i * bytes + 3 + j * width] = trainingData[i + j * WIDTH + k * WIDTH * HEIGHT];
		}
	}
		

	SDL_UnlockTexture(texture);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int getImage(int index, float* image)
{
	int i, j, k = index;
	for(j = 0; j < HEIGHT; ++j)
	{
		for(i = 0; i < WIDTH; i++)
		{
			image[i + j * WIDTH] = 1.f / 255.f * (float)trainingData[i + j * WIDTH + k * WIDTH * HEIGHT];
		}
	}

	return labelData[k];
}