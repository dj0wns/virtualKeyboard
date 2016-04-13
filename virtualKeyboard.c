#include <stdio.h>

#define COORDINATES_ARRAY_SIZE 256


#define KEYBOARD_WIDTH 10
#define KEYBOARD_HEIGHT 4

const char keyboard[KEYBOARD_HEIGHT][KEYBOARD_WIDTH] = {
	{'1','2','3','4','5','6','7','8','9','0'},
	{'q','w','e','r','t','y','u','i','o','p'},
	{'a','s','d','f','g','h','j','k','l','-'},
	{'z','x','c','v','b','n','m','_','@','.'}
};

//iirc it is an omptimization to use int over byte or short despite only using values <10
typedef struct {
	int x;
	int y;
} coordinates;

void generateCoords(coordinates*);

int main(int argc, char **argv){
	coordinates coords[COORDINATES_ARRAY_SIZE];
	if(argc != 2){
		printf("Invalid Arguments. Usage: virtualKeyboard <string>\n");

	}else{
		printf("%s\n", argv[1]);
	}
	generateCoords(coords);


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

