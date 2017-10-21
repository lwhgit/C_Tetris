#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

const int KEY_ESC = 27;				// 탈출
const int KEY_UP = 72;				// 회전
const int KEY_DOWN = 80;			// 한칸 아래로
const int KEY_LEFT = 75;			// 왼쪽
const int KEY_RIGHT = 77;			// 오른쪽
const int KEY_SPACE = 32;			// 맨 아래로
const int KEY_ENTER = 13;			// 엔터

const int MAP_WIDTH = 10;			// 맵의 가로사이즈
const int MAP_HEIGHT = 20;			// 맵의 세로사이즈

const int BLOCK_ID_AIR = 0;			// 빈 블럭 ID
const int BLOCK_ID_FIXED = 1;		// 고정된 블럭 ID
const int BLOCK_ID_FLOATING = 2;	// 움직일 수 있는 블럭 ID
const int BLOCK_ID_WALL = 3;		// 벽 ID

const int ROTATE_0 = 0;				// 0도 회전
const int ROTATE_1 = 1;				// 90도 회전
const int ROTATE_2 = 2;				// 180도 회전
const int ROTATE_3 = 3;				// 270도 회전

const char BLOCKS[4][4] = { "  \0", "■\0", "□\0", "▦\0" };		// 블럭들

const int BLOCK_SHAPE[][16] = { { 0, 1, 0, 0,			//   ■
								  0, 1, 0, 0,			//   ■
								  0, 1, 1, 0,			//   ■■
								  0, 0, 0, 0 },			//
								

								{ 0, 0, 1, 0,			//     ■
								  0, 0, 1, 0,			//     ■
								  0, 1, 1, 0,			//   ■■
								  0, 0, 0, 0},			//

								{ 0, 1, 0, 0,			//   ■
								  0, 1, 0, 0,			//   ■
								  0, 1, 0, 0,			//   ■
								  0, 1, 0, 0},			//   ■

								{ 0, 0, 0, 0,			//
								  0, 1, 1, 0,			//   ■■
								  0, 1, 1, 0,			//   ■■
								  0, 0, 0, 0},			//

								{ 0, 0, 0, 0,			//
								  0, 1, 1, 0,			//   ■■
								  1, 1, 0, 0,			// ■■
								  0, 0, 0, 0},			//

								{ 0, 0, 0, 0,			//
								  1, 1, 0, 0,			// ■■
								  0, 1, 1, 0,			//   ■■
								  0, 0, 0, 0},			//

								{ 0, 1, 0, 0,			//   ■
								  0, 1, 1, 0,			//   ■■
								  0, 1, 0, 0,			//   ■
								  0, 0, 0, 0} };		//						블럭 모양들

int map[200];			// 현재 그려져 있는 맵
int cpyMap[200];		// 새로고침 된 맵				map와 cpyMap를 비교해 가며 달라진 부분만 그린다. 안그러면 깜빡거림.

int repeatTime = 0;						// 이게 1000가 되면 블럭이 한 칸 내려간다.
int currentBlockId = 0;					// 현재 블럭 ID
int currentBlock[16] = { 0, 0, 0, 0,
						 0, 0, 0, 0,
						 0, 0, 0, 0,
						 0 ,0, 0, 0 };	// 현재 블럭 모양
int currentX = 0;						// 현재 블럭 X좌표
int currentY = 0;						// 현재 블럭 Y좌표
int currentRotate = 0;					// 현재 블럭 회전정도

void menu();							// 시작 시 호출될 함수
void hideCursor();						// 커서를 숨기는 함수
void initWindow();						// 윈도우의 타이틀과 크기를 조절하는 함수

void gameStart();						// 게임이 시작되면 이게 호출된다.
void createNewBlock();					// 새 블럭을 만드는 함수.
void deleteFloatingBlock();				// 현재 움직이는 블럭을 콘솔상에서 삭제한다.
int downBlock();						// 블럭을 아래로 한 칸
void moveLeft();						// 블럭을 왼쪽으로 한 칸
void moveRight();						// 블럭을 오른쪽으로 한 칸
void dropBlock();						// 블럭을 맨 아래로
int checkArea(int x, int y);			// x, y에 현재 블럭이 위치할 수 있는지
void handleKeyEvent();					// 키 입력을 처리한다.
void escape();							// ESC 누르면 발동
void drawAllMap();						// 맵을 모두 다시그림 (Resume 되거나 Restart 또는 Start 될 시 호출)
void drawChangedMap();					// 변경 된 맵만 그림
void copyMap();							// cpyMap의 값을 map로 옮기는 작업
int getBlock(int x, int y);				// x, y에 있는 블럭 ID 반환
void setBlock(int x, int y, int id);	// x, y에 블럭 설정
void gotoBlock(int x, int y);			// 블럭을 x, y로 움직임.
void fixBlock(int x, int y);			// 블럭을 x, y에 고정시킴
void checkClearFloor();					// 다 채워진 층이 있는지 확인한다. 있으면 attachFloor을 호출해서 층을 붙임.
void rotateBlock();						// 블럭을 회전시킴
void attachFloor(int floor);			// floor층을 붙임

void endGame();							// 게임 끝나면 호출 (기능 없음)

void gotoxy(int x, int y);				// 콘솔상에서 커서를 x, y로 움직인다,
int getKey();							// 현재 눌린 키를 반환 (getch로 쓰면 방향키 같은거 눌릴시 처리해야 할 게 있어서 따로 분리)

int main() {
	srand(time(NULL));
	menu();

	printf("\n계속하려면 아무키나 누르세요...");
	getKey();
	return 0;
}

void menu() {
	hideCursor();
	initWindow();
	gotoxy(3, 2);
	printf("■■■ ■■■ ■■■ ■■■   ■    ■■■");
	gotoxy(3, 3);
	printf("  ■   ■       ■   ■   ■  ■  ■      ");
	gotoxy(3, 4);
	printf("  ■   ■■■   ■   ■ ■    ■    ■■  ");
	gotoxy(3, 5);
	printf("  ■   ■       ■   ■   ■  ■        ■");
	gotoxy(3, 6);
	printf("  ■   ■■■   ■   ■   ■  ■  ■■■  ");
	gotoxy(21, 12);
	printf("> START <");
	gotoxy(21, 14);
	printf("  EXIT  ");

	int currentMenu = 0;		// 0: Start, 1: Exit

	while (1) {
		int key = getKey();

		if (key == KEY_UP) {
			if (currentMenu == 1) {
				currentMenu = 0;
				gotoxy(21, 12);
				printf("> START <");
				gotoxy(21, 14);
				printf("  EXIT  ");
			}
		} else if (key == KEY_DOWN) {
			if (currentMenu == 0) {
				currentMenu = 1;
				gotoxy(21, 12);
				printf("  START  ");
				gotoxy(21, 14);
				printf("> EXIT <");
			}
		} else if (key == KEY_ENTER) {
			if (currentMenu == 0) {
				system("cls");
				gameStart();
				break;
			} else if (currentMenu == 1) {
				exit(0);
			}
		} else if (key == KEY_ESC) {
			exit(0);
			break;
		}
	}
}

void initWindow() {
	system("mode con:cols=49 lines=21");
	system("title Tetris");
}

void hideCursor() {
	CONSOLE_CURSOR_INFO consoleCursor;
	consoleCursor.bVisible = 0;
	consoleCursor.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursor);
}

void gameStart() {
	repeatTime = 0;

	int i = 0;
	for (i = 0;i < MAP_WIDTH * MAP_HEIGHT;i ++) {
		map[i] = 0;
		cpyMap[i] = 0;
	}
	
	drawAllMap();
	createNewBlock();
	drawChangedMap();

	while (1) {
		handleKeyEvent();
		Sleep(10);

		repeatTime += 10;

		if (repeatTime >= 1000) {
			deleteFloatingBlock();
			if (!downBlock()) {
				break;
			}
			drawChangedMap();

			repeatTime = 0;
		}
	}
}

void createNewBlock() {
	currentBlockId = rand() % 7;
	currentX = 0;
	currentY = 0;
	currentRotate = 0;

	int i = 0;

	for (i = 0;i < 16;i ++) {
		currentBlock[i] = BLOCK_SHAPE[currentBlockId][i];
	}
	for (i = 0;i < 16;i ++) {
		if (currentBlock[i] == 1) {
			setBlock((i % 4), (i / 4), BLOCK_ID_FLOATING);
		}
	}
}

void deleteFloatingBlock() {
	int i = 0;
	for (i = 0;i < MAP_WIDTH * MAP_HEIGHT;i ++) {
		if (getBlock(i % MAP_WIDTH, i / MAP_WIDTH) == BLOCK_ID_FLOATING) {
			setBlock(i % MAP_WIDTH, i / MAP_WIDTH, BLOCK_ID_AIR);
		}
	}
}

void deleteAllBlock() {
	int i = 0;
	for (i = 0;i < MAP_WIDTH * MAP_HEIGHT;i ++) {
		setBlock(i % MAP_WIDTH, i / MAP_WIDTH, BLOCK_ID_AIR);
	}
}

int downBlock() {
	if (checkArea(currentX, currentY + 1)) {
		currentY ++;
		gotoBlock(currentX, currentY);
		repeatTime = 0;
	} else {
		fixBlock(currentX, currentY);
		if (currentY == 0) {
			endGame();
			return 0;
		}
		createNewBlock();
	}

	return 1;
}

void moveLeft() {
	if (checkArea(currentX - 1, currentY)) {
		currentX --;
		gotoBlock(currentX, currentY);
	}
}

void moveRight() {
	if (checkArea(currentX + 1, currentY)) {
		currentX ++;
		gotoBlock(currentX, currentY);
	}
}

void dropBlock() {
	int i = 0;
	for (i = currentY;i < 20;i ++) {
		if (!checkArea(currentX, i)) {
			fixBlock(currentX, i - 1);
			deleteFloatingBlock();
			createNewBlock();
			drawChangedMap();
			break;
		}
	}
}

int checkArea(int x, int y) {
	int i = 0;
	for (i = 0;i < 16;i ++) {
		int tmp = currentBlock[i];
		if (tmp == 1) {
			if (x + (i % 4)< 0 || x + (i % 4) >= MAP_WIDTH || y + (i / 4) < 0 || y + (i / 4) >= MAP_HEIGHT) {
				return 0;
			}
			if (getBlock(x + (i % 4), y + (i / 4)) != BLOCK_ID_AIR && getBlock(x + (i % 4), y + (i / 4)) != BLOCK_ID_FLOATING) {
				return 0;
			}
		}
	}
	return 1;
}

void handleKeyEvent() {
	if (kbhit()) {
		int key = getKey();

		if (key == KEY_ESC) {
			escape();
		} else if (key == KEY_UP) {
			rotateBlock();
		} else if (key == KEY_DOWN) {
			downBlock();
		} else if (key == KEY_LEFT) {
			moveLeft();
		} else if (key == KEY_RIGHT) {
			moveRight();
		} else if (key == KEY_SPACE) {
			dropBlock();
		}
	}
}

void drawAllMap() {
	system("cls");

	int y = 0;
	int x = 0;
	for ( y = 0; y < MAP_HEIGHT;y ++) {
		printf("%s", BLOCKS[BLOCK_ID_WALL]);

		for (x = 0;x < MAP_WIDTH;x ++) {
			int id = getBlock(x, y);
			printf("%s", BLOCKS[id]);
		}
		printf("%s\n", BLOCKS[BLOCK_ID_WALL]);
	}
	int i = 0;
	for (i = 0;i < 12;i ++) {
		printf("%s", BLOCKS[BLOCK_ID_WALL]);
	}

	gotoxy(32, 1);
	printf("TETRIS");
	gotoxy(28, 3);
	printf("  ESC  : Escape");
	gotoxy(28, 5);
	printf("  ↑   : Rotation");
	gotoxy(28, 6);
	printf("←  → : Move");
	gotoxy(28, 7);
	printf("  ↓   : Down");
	gotoxy(28, 9);
	printf(" SPACE : Drop");
}

void drawChangedMap() {
	int i = 0;
	for (i = 0;i < MAP_WIDTH * MAP_HEIGHT;i ++) {
		int id = cpyMap[i];
		if (map[i] != id) {
			gotoxy((1 + i % MAP_WIDTH) * 2, i / MAP_WIDTH);
			printf("%s", BLOCKS[id]);
		}
	}
	copyMap();
}

void copyMap() {
	int i = 0;
	for (i = 0;i < MAP_WIDTH * MAP_HEIGHT;i ++) {
		map[i] = cpyMap[i];
	}
}

int getBlock(int x, int y) {
	return cpyMap[y * MAP_WIDTH + x];
}

void setBlock(int x, int y, int id) {
	cpyMap[y * MAP_WIDTH + x] = id;
}

void gotoBlock(int x, int y) {
	deleteFloatingBlock();

	int i = 0;
	for (i = 0;i < 16;i ++) {
		if (currentBlock[i] == 1) {
			setBlock(x + (i % 4), y + (i / 4), BLOCK_ID_FLOATING);
		}
	}

	drawChangedMap();
}

void fixBlock(int x, int y) {
	int i = 0;
	for (i = 0;i < 16;i ++) {
		if (currentBlock[i] == 1) {
			setBlock(x + (i % 4), y + (i / 4), BLOCK_ID_FIXED);
		}
	}

	checkClearFloor();
}

void checkClearFloor() {
	int y = 0;
	int x = 0;
	for (y = 19;y >= 0;y --) {
		int tmp = 0;

		for (x = 0;x < 10;x ++) {
			if (getBlock(x, y) == BLOCK_ID_FIXED) {
				tmp ++;
			}
		}

		if (tmp == 10) {
			for (x = 0;x < 10;x ++) {
				setBlock(x, y, BLOCK_ID_AIR);
			}

			drawChangedMap();											// 클리어된 칸을 삭제하는 부분.
			Sleep(100);													//
			attachFloor(y);												//
			break;														// 단순히 타격감을 높이기 위해 이렇게 코딩함.
		}																//
	}																	//
}																		//
																		//
void attachFloor(int floor) {											//
								
	int x = 0;										//
	for (;floor > 0;floor --) {											//
		for (x = 0;x < 10;x ++) {									//
			setBlock(x, floor, getBlock(x, floor - 1));					//
		}																//
	}																	//
	drawChangedMap();													//
	Sleep(100);															//
	checkClearFloor();													//
}																		//

void rotateBlock() {
	int tmp[16];
	int i = 0;

	for (i = 0;i < 16;i ++) {
		tmp[i] = currentBlock[i];
	}

	currentRotate ++;

	if (currentRotate == 4) {
		currentRotate = 0;
	}
	int y = 0;
	int x = 0;

	if (currentRotate == 0) {
		for (y = 0;y < 4;y ++) {
			for (x = 0;x < 4;x ++) {
				currentBlock[y * 4 + x] = BLOCK_SHAPE[currentBlockId][y * 4 + x];
			}
		}
	} else if (currentRotate == 1) {
		for (y = 0;y < 4;y ++) {
			for (x = 0;x < 4;x ++) {
				currentBlock[y * 4 + x] = BLOCK_SHAPE[currentBlockId][12 - (x * 4 - y)];
			}
		}
	} else if (currentRotate == 2) {
		for (y = 0;y < 4;y ++) {
			for (x = 0;x < 4;x ++) {
				currentBlock[y * 4 + x] = BLOCK_SHAPE[currentBlockId][15 - (y * 4 + x)];
			}
		}
	} else if (currentRotate == 3) {
		for (y = 0;y < 4;y ++) {
			for (x = 0;x < 4;x ++) {
				currentBlock[y * 4 + x] = BLOCK_SHAPE[currentBlockId][x * 4 - y + 3];
			}
		}
	}

	if (checkArea(currentX, currentY)) {
		deleteFloatingBlock();

	int i = 0;
		for (i = 0;i < 16;i ++) {
			if (currentBlock[i] == 1) {
				setBlock(currentX + (i % 4), currentY + (i / 4), BLOCK_ID_FLOATING);
			}
		}

		drawChangedMap();
	} else {
		currentRotate --;
	int i = 0;
		for (i = 0;i < 16;i ++) {
			currentBlock[i] = tmp[i];
		}
	}
}

void escape() {
	gotoxy(15, 10);
	printf("▒▒▒▒▒▒▒▒▒▒▒");
	gotoxy(15, 11);
	printf("▒                  ▒");
	gotoxy(15, 12);
	printf("▒                  ▒");
	gotoxy(15, 13);
	printf("▒                  ▒");
	gotoxy(15, 14);
	printf("▒                  ▒");
	gotoxy(15, 15);
	printf("▒                  ▒");
	gotoxy(15, 16);
	printf("▒                  ▒");
	gotoxy(15, 17);
	printf("▒                  ▒");
	gotoxy(15, 18);
	printf("▒▒▒▒▒▒▒▒▒▒▒");
	gotoxy(21, 12);
	printf("> RESUME <");
	gotoxy(21, 14);
	printf("  RESTART  ");
	gotoxy(21, 16);
	printf("  EXIT   ");

	int currentMenu = 0;		// 0: Start, 1: Exit

	while (1) {
		int key = getKey();

		if (key == KEY_UP) {
			if (currentMenu > 0) {
				currentMenu --;
			}
		} else if (key == KEY_DOWN) {
			if (currentMenu < 2) {
				currentMenu ++;
			}
		} else if (key == KEY_ENTER) {
			if (currentMenu == 0) {
				drawAllMap();
				break;
			} else if (currentMenu == 1) {
				gameStart();
				break;
			} else if (currentMenu == 2) {
				exit(0);
			}
		}

		if (currentMenu == 0) {
			gotoxy(21, 12);
			printf("> RESUME <");
			gotoxy(21, 14);
			printf("  RESTART  ");
			gotoxy(21, 16);
			printf("  EXIT  ");
		} else if (currentMenu == 1) {
			gotoxy(21, 12);
			printf("  RESUME  ");
			gotoxy(21, 14);
			printf("> RESTART <");
			gotoxy(21, 16);
			printf("  EXIT  ");
		} else if (currentMenu == 2) {
			gotoxy(21, 12);
			printf("  RESUME  ");
			gotoxy(21, 14);
			printf("  RESTART  ");
			gotoxy(21, 16);
			printf("> EXIT <");
		}
	}
}

void endGame() {
	//system("cls");
}

void gotoxy(int x, int y) {
	COORD pos = { (short)x, (short)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int getKey() {
	int key = getch();

	if (key == 0xE0) {
		key = getch();
		return key;
	} else {
		return key;
	}
}
