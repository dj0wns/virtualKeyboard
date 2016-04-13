#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define COORDINATES_ARRAY_SIZE 256
#define MOVEMENT_ARRAY_LEN 1024

#define KEYBOARD_WIDTH 10
#define KEYBOARD_HEIGHT 4

const char keyboard[KEYBOARD_HEIGHT][KEYBOARD_WIDTH] = {
	{'1','2','3','4','5','6','7','8','9','0'},
	{'q','w','e','r','t','y','u','i','o','p'},
	{'a','s','d','f','g','h','j','k','l','-'},
	{'z','x','c','v','b','n','m','_','@','.'}
};

typedef enum {
	right,
	left,
	up,
	down
} dir;

//iirc it is an omptimization to use int over byte or short despite only using values <10
typedef struct {
	int x;
	int y;
} coordinates;

typedef struct {
	int xmove;
	int ymove;
	dir xdir;
	dir ydir;
} movedata;

void generateCoords(coordinates*);
unsigned int nearestNeighbor(coordinates*, char*, char*, char*, int, int);
int calcDistance(coordinates*, char, char, movedata*);

int main(int argc, char **argv){
	coordinates coords[COORDINATES_ARRAY_SIZE];
	char moves[MOVEMENT_ARRAY_LEN];
	
	unsigned int temp, min = 0;
	min--; //largest possible int

	if(argc != 2){
		printf("Invalid Arguments. Usage: virtualKeyboard <string>\n");

	}else{
		printf("%i\n", strlen(argv[1]));
		int len = strlen(argv[1]);
		generateCoords(coords);
		for(int i = 5; i<6; i++){
			temp = nearestNeighbor(coords, moves, argv[1], argv[1], i, len);
			if(temp < min){
				printf("%i\n",temp);
				min = temp;
			}
		}
	
	}
	return 0;
}

//optimization so it only searches through the matrix once
void generateCoords(coordinates* coords){
	for(int i = 0; i < KEYBOARD_HEIGHT; i++){
		for(int j = 0; j < KEYBOARD_WIDTH; j++){
			coords[keyboard[i][j]].y = i;
			coords[keyboard[i][j]].x = j;
		}
	}
}

int calcDistance(coordinates* coords, char from, char to, movedata* data){
	if(to == ' ')
		return 0;
	from = tolower(from);
	to = tolower(to);
	printf("Comparing %c to %c: ",from, to);
	data->xmove = coords[(int)from].x - coords[(int)to].x;
	data->ymove = coords[(int)from].y - coords[(int)to].y;	
	data->xdir = data->xmove >= 0 ? left : right;
	data->ydir = data->ymove >= 0 ? up : down;
	data->xmove = abs(data->xmove);
	data->ymove = abs(data->ymove);
	//wraparound
	if(data->xmove > 5){
		data->xmove = 10 - data->xmove;
	}	
	if(data->ymove > 2){
		data->ymove = 4 - data->ymove;
	}

	printf("%i\n", data->xmove + data->ymove);
	return data->xmove + data->ymove;
}

unsigned int nearestNeighbor(coordinates* coords, char* moves, char* og, char* input, int index, int len){
	movedata data;
	char* newString = malloc((len-1) * sizeof(char));
	strncpy(newString, input, index);
	strcpy(newString+index, input+index+1);
	for(int i = 0; i < len-1; i++){
		calcDistance(coords, input[index], newString[i], &data);
	}
	free(newString);	
	return 0;
}
