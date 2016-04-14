#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define COORDINATES_ARRAY_SIZE 256
#define MOVEMENT_ARRAY_LEN 1024

#define KEYBOARD_WIDTH 10
#define KEYBOARD_HEIGHT 4

#define DIRECTION_BIT 0x40

#define INITIAL_CHAR '1'

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
unsigned int nearestNeighbor(coordinates*, char*, char*, char*, char*, int, int, int, int, int, int);
int calcDistance(coordinates*, char, char, movedata*);

int main(int argc, char **argv){
	coordinates coords[COORDINATES_ARRAY_SIZE];
	char moves[MOVEMENT_ARRAY_LEN];
	unsigned int temp, min = 0;
	min--; //largest possible int

	if(argc != 2){
		printf("Invalid Arguments. Usage: virtualKeyboard <string>\n");

	}else{
		int len = strlen(argv[1]);
		int moveCursor, capsLock;
		char *workingString =  malloc(len * sizeof(char));
		generateCoords(coords);
		movedata data;
		for(int weight = 1; weight < 20; weight ++){
			for(int i = 0; i<len; i++){
				//zero out arrays
				moveCursor = 0;
				capsLock = 0;
				memset(moves, 0, MOVEMENT_ARRAY_LEN);
				memset(workingString, 0, len);
				workingString[0] = argv[1][i];
				//add initial moves
				temp = calcDistance(coords, INITIAL_CHAR, argv[1][i], &data);
				
				for(int j = 0; j < data.xmove; j++){
					moves[moveCursor] = data.xdir == right ? 'R' : 'L';
					moveCursor++;
				}
				for(int j = 0; j < data.ymove; j++){
					moves[moveCursor] = data.ydir == up ? 'U' : 'D';
					moveCursor++;
				}
				if((capsLock==0) != (isupper(argv[1][i])==0)){
					moves[moveCursor] = 'C';
					capsLock = capsLock == 0 ? 1 : 0;
					moveCursor++;
					temp++;
				}
				moves[moveCursor] = 'A';
				temp++;
				temp += nearestNeighbor(coords, moves, argv[1], argv[1], workingString, i ,len, len, 1, weight, capsLock);
				if(temp < min){
					printf("%s\t%s\t%i\n", moves, workingString, temp);
					min = temp;
				}
			
			}
		}
		free(workingString);
	}
	return 0;
}

//optimization so it only searches through the matrix once
void generateCoords(coordinates* coords){
	for(int i = 0; i < KEYBOARD_HEIGHT; i++){
		for(int j = 0; j < KEYBOARD_WIDTH; j++){
			coords[(int)keyboard[i][j]].y = i;
			coords[(int)keyboard[i][j]].x = j;
		}
	}
}

int calcCursorMoves(char* og, char* workingString, char added, int cursor, int len){
	char* inUseWS = calloc(len, sizeof(char));
	char* inUseOG = calloc(len, sizeof(char));
	int indexToAdd = 0, newCursor = cursor;
	for(int i = 0; i < len; i++){
		for(int j = 0; j < len; j++){
			if(workingString[j] != -1 && og[i] == workingString[j] 
					&& inUseOG[i] == 0 && inUseWS[j] == 0){
				inUseOG[i] = 1;
				inUseWS[j] = i+1; // set in use to corresponding working string
				break;
			}
			if(workingString[j] == 0){
				inUseWS[j] = -1; //append -1 to end
				break; //nothing more to see here	
			}
		}
	}
	for(int i = 0; i < len; i++){
		if(og[i] == added && inUseOG[i] == 0){
			indexToAdd = i;
			break;
		}
	}
	for(int i = 0; i < len; i++){
		if(i==0 && indexToAdd < inUseWS[i]-1){
			newCursor = i;
			break;

		}
		if(inUseWS[i]-1 < indexToAdd && (indexToAdd < inUseWS[i+1]-1 || inUseWS[i+1] == -1)){
			newCursor = i+1;
			break;
		}
		if(workingString[i+1] == 0){
			newCursor = i+1;
			break;
		}

	}
	free(inUseWS);
	free(inUseOG);
	return newCursor - cursor;  
}

int calcDistance(coordinates* coords, char from, char to, movedata* data){
	if(to == ' ')
		return 0;
	from = tolower(from);
	to = tolower(to);
	data->xmove = coords[(int)from].x - coords[(int)to].x;
	data->ymove = coords[(int)from].y - coords[(int)to].y;	
	data->xdir = data->xmove >= 0 ? left : right;
	data->ydir = data->ymove >= 0 ? up : down;
	data->xmove = abs(data->xmove);
	data->ymove = abs(data->ymove);
	//wraparound
	if(data->xmove > 5){
		data->xmove = 10 - data->xmove;
		data->xdir = data->xdir == right ? left : right;
	}	
	if(data->ymove > 2){
		data->ymove = 4 - data->ymove;
		data->ydir = data->ydir == up ? down : up;
	}
	return data->xmove + data->ymove;
}

//assumes array is longer than current string
void insertAt(char* str, char toIns, int index){
	char* temp = malloc(strlen(str) + 1);
	strncpy(temp, str, index);
	temp[index] = toIns;
	strcpy(temp+index+1, str+index);
  	strcpy(str, temp);	
	free(temp);
}

unsigned int nearestNeighbor(coordinates* coords, char* moves, char* og, 
		char* input, char* workingString, int index, int lenOG, int len,  
		int cursor, int weight, int capsLock){
	if(len  == 1){
		return 0;
	}
	movedata data, bestData;
	char bestChar = '\0';
	unsigned int bestCursor = -1, bestIndex = -1, bestDist = -1;
	unsigned int dist = -1, cursorDist = -1, best = -1;
	char newString[len-1] ;
	memset(newString, 0, len-1);
	strncpy(newString, input, index);
	strcpy(newString+index, input+index+1);
//	printf("%s\t", newString);
	for(int i = 0; i < len-1; i++){
		dist = calcDistance(coords, input[index], newString[i], &data);
		cursorDist = calcCursorMoves(og, workingString, newString[i], cursor, lenOG);
		if(dist + weight * abs(cursorDist) + ((capsLock==0) != (isupper(newString[i])==0)) < best){
	//		printf("%c %c %i %i\n", workingString[cursor], newString[i], cursorDist, cursor);
			bestDist = cursorDist;
			bestCursor = cursor + cursorDist;
			best = dist + weight * abs(cursorDist) + ((capsLock==0) != (isupper(newString[i])==0));
			bestChar = newString[i];
			bestIndex = i;
			memcpy(&bestData, &data, sizeof(data));
		}
	}
	//append moves
	int i;
	for(i = 0; i < MOVEMENT_ARRAY_LEN && moves[i]!=0; i++);
	if(bestChar != ' '){
		for(int j = 0; j < bestData.xmove; j++){
			moves[i] = bestData.xdir == right ? 'R' : 'L';
			i++;
		}
		for(int j = 0; j < bestData.ymove; j++){
			moves[i] = bestData.ydir == up ? 'U' : 'D';
			i++;
		}
		for(int j = 0; j < abs(bestDist); j++){
			moves[i] = bestDist > 0 ? 'B' : 'F'; 
			i++;
		}
		if((capsLock==0) != (isupper(bestChar)==0)){
			moves[i] = 'C';
			capsLock = capsLock == 0 ? 1 : 0;
			i++;
		}
		moves[i] = 'A';
	} else {
		moves[i] = 'Y';
	}
	//remove weight
	best -= weight * abs(bestDist);

	insertAt(workingString, bestChar, bestCursor);
	int max = nearestNeighbor(coords, moves, og, newString, workingString, 
			 bestIndex, lenOG, len-1, bestCursor+1, weight, capsLock);
	//the 1 is for the actual click
	return max + best + 1 + abs(bestDist);
}



