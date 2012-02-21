#ifndef _TERRAIN_GENERATOR_H_
#define _TERRAIN_GENERATOR_H_

#include "vecmath.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

class TerrainGenerator
{
public:
	TerrainGenerator(float h);
	~TerrainGenerator();

	//generateTerrain();
	//void diamondSquare(float * heights, int * length, float h);
	//void diamondSquare(float * heights, int * length, float h, int n);

	int determineLength(int n);

	//int diamondStep(int n, float * heights, int rows, int columns, float h);
	//int squareStep(int n, float * heights, int rows, int columns, float h);

	template <class T>
	int diamondStep(int n, T * heights, int rows, int columns, T h) {
		int inc;
		if (n == 1) {
			inc = 2;
		} else {
			inc = pow(2.0f, n);
		}

		int colInc;
		int rowInc = (rows - 1)/inc;

		for (int r = 0; rowInc > 0 && r < rows - rowInc; r += rowInc) {
			int rOff = 0;
			colInc = (columns - 1)/inc;
			int midR = (r + (r+rowInc))/2;
			for (int c = 0; colInc > 0 && c < columns - colInc; c += colInc) {
				int midC = (c + (c+colInc))/2;
				if (heights[midR * columns + midC] == 0) {
					T avg = 0;

					avg += heights[r * columns + c];
					avg += heights[r * columns + c + colInc];
					avg += heights[(r + rowInc) * columns + c];
					avg += heights[(r + rowInc) * columns + (c+colInc)];
					avg /= 4;
					
					avg += (T)((float)rand()/(float)RAND_MAX * h - (float)rand()/(float)RAND_MAX * h);

					if (avg == 0) {
						avg = .001;
					}
					heights[midR * columns + midC] = avg;
				}
			}
		}

		return max(rowInc, colInc);
	}


	template <class T>
	int squareStep(int n, T * heights, int rows, int columns, T h) {
		int inc;
		n += 1;
		if (n == 1) {
			inc = 2;
		} else {
			inc = pow(2.0f, n);
		}

		int colInc;
		int rowInc = (rows - 1)/inc;

		int rCount = 0;
		for (int r = 0; r < rows; r += rowInc) {
			colInc = (columns - 1)/inc;
			int startC = 0;
			if (rCount % 2 == 0) {
				startC = colInc;
			}
			for (int c = startC; c < columns; c += colInc) {
				int val = r * columns + c;

				if (heights[(int)r * columns + (int)c] == 0) {
					int roi[4] = {r-rowInc, r, r, r+rowInc};
					int coi[4] = {c, c-colInc, c+colInc, c};


					if (roi[0] < 0) {
						roi[0] += rows - 1;
					} 
					if (roi[3] > rows - 1) {
						roi[3] %= rows - 1;
					}


					if (coi[1] < 0) {
						coi[1] += columns - 1;
					}
					if (coi[2] > columns - 1) {
						coi[2] %= columns - 1;
					}

					T avg = 0;
					for (int i = 0; i < 4; i++) {
						int ind = roi[i] * columns + coi[i];
						avg += heights[ind];
					}

					avg /= 4;
					avg += (T)((float)rand()/(float)RAND_MAX * h - (float)rand()/(float)RAND_MAX * h);
					if (avg == 0) {
						avg = .001;
					}
					heights[(int)r * columns + (int)c] = avg;
				}
			}
			rCount++;
		}

		return max(rowInc, colInc);
	}

	template <class T>
	void diamondSquare(T * heights, int * length, T h) {
		int rows = sqrt((float)*length);
		int n = 0;
		int gap;

		do {
			gap = diamondStep<T>(n, heights, rows, rows, h);
			gap = min(squareStep<T>(n, heights, rows, rows, h), gap);
			h *= pow(2, -roughness);
			n++;
		} while (gap > 1);
	}

	template <class T>
	void diamondSquare(T * heights, int * length, T h, int n) {
		if (heights == NULL || length == NULL) {
			*length = determineLength(n);
			heights = new T[*length];
			for (int i = 0; i < *length; i++) {
				heights[i] = 0;
			}
		}

		diamondSquare<T>(heights, length, h);
	}

private:
	float roughness;
};

#endif