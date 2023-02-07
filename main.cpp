#include <iostream>
#include <easyx.h>
#include <time.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
using namespace std;
#define ROW 10
#define COL 10
#define MINE_NUM 10
#define GRID_W 40
int map[ROW][COL];
IMAGE img[12];

void gemeDraw();
int jugeWin(int row,int col)
{
	if (map[row][col] == -1)
	{
		for (int i = 0; i < ROW; i++)
		{
			for (int j = 0; j < COL; j++)
			{
				if (map[i][j] >= 19&&map[i][j]<=28)
				{
					map[i][j] -= 20;
				}
				else if (map[i][j] >= 39 && map[i][j] <= 48)
				{
					map[i][j] -= 40;
				}
			}
		}
		return 0;
	}
	int cnt = 0;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j]>=0&&map[i][j]<=8)
			{
				cnt++;
			}
		}
	}
	if (cnt == ROW * ROW - MINE_NUM)
	{
		return 1;
	}
	
	return -1;
}

void playMusic(const char* s)
{
	char str[50];

	sprintf_s(str, "close ./%s", s);
	mciSendString(str, 0, 0, 0);

	sprintf_s(str,"open ./%s",s);
	mciSendString(str, 0, 0, 0);

	sprintf_s(str, "play ./%s", s);
	mciSendString(str, 0, 0, 0);
}

void gameInit()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			map[i][j] = 0;
		}
	}
	srand((unsigned)time(NULL));
	for (int i = 0; i < MINE_NUM;)
	{
		int r = rand() % ROW;
		int c = rand() % COL;
		if (map[r][c] == 0)
		{
			map[r][c] = -1;
			i++;
		}

	}

	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] == -1)
			{
				for (int r = i - 1; r <= i + 1; r++)
				{
					for (int c = j - 1; c <= j + 1; c++)
					{
						if ((r >= 0 && r < ROW && c >= 0 && c < COL) && map[r][c] != -1)
						{
							map[r][c]++;
						}
					}
				}
			}
		}
	}
	char str[50] = "";
	for (int i = 0; i < 12; i++)
	{
		sprintf_s(str, "./%d.jpg", i);
		loadimage(&img[i], str,GRID_W,GRID_W);
	}

	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			map[i][j] += 20;
		}
	}
}

void openNull(int row, int col)
{
	if (map[row][col] == 0)
	{
		for (int i = row - 1; i <= row + 1; i++)
		{
			for (int j = col - 1; j <= col + 1; j++)
			{
				if ((i >= 0 && i < ROW && j >= 0 && j < COL) && map[i][j] > 19)
				{
					map[i][j] -= 20;
					openNull(i, j);
				}
			}
		}
	}
}


void gemeDraw()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (map[i][j] >= 0 && map[i][j] <= 8)
			{
				putimage(j * GRID_W, i * GRID_W, &img[map[i][j]]);
			}
			else if (map[i][j] == -1)
			{
				putimage(j * GRID_W, i * GRID_W, &img[9]);
			}
			else if (map[i][j] >= 19 && map[i][j] <= 28)
			{
				putimage(j * GRID_W, i * GRID_W, &img[10]);
			}
			else if (map[i][j] > 28)
			{
				putimage(j * GRID_W, i * GRID_W, &img[11]);
			}
			
		}
	}
}

void show()
{	
	system("cls");
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			printf("%2d ", map[i][j]);
		}
		cout << endl;
	}
}

int mouseEvent()
{
	ExMessage msg;
	if (peekmessage(&msg, WH_MOUSE))
	{
		int row = msg.y / GRID_W;
		int col = msg.x / GRID_W;
		if (msg.message == WM_LBUTTONDOWN)
		{

			if (map[row][col] >= 19)
			{
				playMusic("click.wav");
				map[row][col] -= 20;
				if (map[row][col] >= 19)
				{
					map[row][col] -= 20;
				}
				if (map[row][col]==0)
				{
					playMusic("search.wav");
				}
				openNull(row,col);
				int yn = jugeWin(row, col);
				if (yn == 1)
				{
					return 1;
				}
				else if (yn == 0)
				{
					return 0;
				}
			}

			//show();
		}
		else if (msg.message == WM_RBUTTONDOWN)
		{
			playMusic("rightClick.wav");
			if (map[row][col] >= 19 && map[row][col] <= 28)
			{
				map[row][col] += 20;
			}
			else if (map[row][col] > 28)
			{
				map[row][col] -= 20;
			}
		}
	}
	return -1;
}

int main()
{	
	flag:
	playMusic("start.mp3");
	initgraph(GRID_W*COL, GRID_W*ROW);
	SetWindowText(GetHWnd(), "山哥扫雷");
	gameInit();
	//show();
	while (1)
	{
		int n = mouseEvent();
		gemeDraw();
		if (n == 1)
		{
			gemeDraw();
			int isyes = MessageBox(GetHWnd(), "YOU WIN 是否继续", "GOOD", MB_YESNO);
			if (isyes == IDYES)
			{
				goto flag;
			}
			else
			break;
		}
		else if (n == 0)
		{
			gemeDraw();
			int isyes = MessageBox(GetHWnd(), "YOU FAIL 是否继续", "PITY", MB_YESNO);
			if (isyes == IDYES)
			{
				goto flag;
			}
			else
			break;
		}
	}

	while (1);
	return 0;
}
