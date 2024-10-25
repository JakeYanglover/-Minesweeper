#include <iostream>
#include <easyx.h>
#include <time.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
using namespace std;

// 定义游戏参数
#define ROW 10       // 行数
#define COL 10       // 列数
#define MINE_NUM 10  // 地雷数量
#define GRID_W 40    // 格子宽度

int map[ROW][COL];   // 存储游戏地图状态
IMAGE img[12];       // 用于存储图片资源

// 游戏绘制函数声明
void gemeDraw();

// 判断是否胜利函数
int jugeWin(int row, int col) {
    // 如果点到地雷，显示地雷并返回失败
	if (map[row][col] == -1) {
		for (int i = 0; i < ROW; i++) {
			for (int j = 0; j < COL; j++) {
				// 显示未打开的地雷
				if (map[i][j] >= 19 && map[i][j] <= 28) {
					map[i][j] -= 20;
				} else if (map[i][j] >= 39 && map[i][j] <= 48) {
					map[i][j] -= 40;
				}
			}
		}
		return 0;  // 游戏失败
	}

    // 检查是否已翻开所有非地雷格子
	int cnt = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (map[i][j] >= 0 && map[i][j] <= 8) {
				cnt++;
			}
		}
	}

    // 如果所有非地雷格子都被翻开，则胜利
	if (cnt == ROW * ROW - MINE_NUM) {
		return 1;  // 游戏胜利
	}

	return -1;  // 游戏继续
}

// 播放音效函数
void playMusic(const char* s) {
	char str[50];

	sprintf_s(str, "close ./%s", s);
	mciSendString(str, 0, 0, 0);

	sprintf_s(str, "open ./%s", s);
	mciSendString(str, 0, 0, 0);

	sprintf_s(str, "play ./%s", s);
	mciSendString(str, 0, 0, 0);
}

// 游戏初始化函数
void gameInit() {
    // 初始化地图，所有格子设置为0
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			map[i][j] = 0;
		}
	}

    // 随机生成地雷
	srand((unsigned)time(NULL));
	for (int i = 0; i < MINE_NUM;) {
		int r = rand() % ROW;
		int c = rand() % COL;
		if (map[r][c] == 0) {
			map[r][c] = -1;  // 地雷设置为-1
			i++;
		}
	}

    // 计算每个格子周围的地雷数量
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (map[i][j] == -1) {
				for (int r = i - 1; r <= i + 1; r++) {
					for (int c = j - 1; c <= j + 1; c++) {
						if ((r >= 0 && r < ROW && c >= 0 && c < COL) && map[r][c] != -1) {
							map[r][c]++;  // 增加周围地雷计数
						}
					}
				}
			}
		}
	}

    // 加载图片资源
	char str[50] = "";
	for (int i = 0; i < 12; i++) {
		sprintf_s(str, "./%d.jpg", i);
		loadimage(&img[i], str, GRID_W, GRID_W);
	}

    // 初始化地图状态，所有格子为未打开状态（加20）
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			map[i][j] += 20;
		}
	}
}

// 打开空格子的递归函数
void openNull(int row, int col) {
	if (map[row][col] == 0) {
		for (int i = row - 1; i <= row + 1; i++) {
			for (int j = col - 1; j <= col + 1; j++) {
				if ((i >= 0 && i < ROW && j >= 0 && j < COL) && map[i][j] > 19) {
					map[i][j] -= 20;  // 标记为已打开
					openNull(i, j);  // 递归打开相邻空白格子
				}
			}
		}
	}
}

// 游戏界面绘制函数
void gemeDraw() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (map[i][j] >= 0 && map[i][j] <= 8) {
				putimage(j * GRID_W, i * GRID_W, &img[map[i][j]]);  // 显示数字
			} else if (map[i][j] == -1) {
				putimage(j * GRID_W, i * GRID_W, &img[9]);  // 显示地雷
			} else if (map[i][j] >= 19 && map[i][j] <= 28) {
				putimage(j * GRID_W, i * GRID_W, &img[10]);  // 显示未打开的格子
			} else if (map[i][j] > 28) {
				putimage(j * GRID_W, i * GRID_W, &img[11]);  // 显示标记的格子
			}
		}
	}
}

// 鼠标事件处理函数
int mouseEvent() {
	ExMessage msg;
	if (peekmessage(&msg, WH_MOUSE)) {
		int row = msg.y / GRID_W;
		int col = msg.x / GRID_W;

		// 左键点击
		if (msg.message == WM_LBUTTONDOWN) {
			if (map[row][col] >= 19) {
				playMusic("click.wav");
				map[row][col] -= 20;  // 打开格子
				if (map[row][col] >= 19) {
					map[row][col] -= 20;
				}
				if (map[row][col] == 0) {
					playMusic("search.wav");
				}
				openNull(row, col);  // 递归打开空白格子
				int yn = jugeWin(row, col);  // 判断游戏是否结束
				if (yn == 1) {
					return 1;  // 胜利
				} else if (yn == 0) {
					return 0;  // 失败
				}
			}
		}
		// 右键点击
		else if (msg.message == WM_RBUTTONDOWN) {
			playMusic("rightClick.wav");
			if (map[row][col] >= 19 && map[row][col] <= 28) {
				map[row][col] += 20;  // 标记地雷
			} else if (map[row][col] > 28) {
				map[row][col] -= 20;  // 取消标记
			}
		}
	}
	return -1;  // 游戏继续
}

int main() {
flag:
	playMusic("start.mp3");  // 播放开始音乐
	initgraph(GRID_W * COL, GRID_W * ROW);  // 初始化窗口
	SetWindowText(GetHWnd(), "山哥扫雷");  // 设置窗口标题
	gameInit();  // 初始化游戏
	while (1) {
		int n = mouseEvent();  // 处理鼠标事件
		gemeDraw();  // 绘制游戏界面
		if (n == 1) {
			gemeDraw();
			int isyes = MessageBox(GetHWnd(), "YOU WIN 是否继续", "GOOD", MB_YESNO);  // 胜利消息框
			if (isyes == IDYES) {
				goto flag;  // 重新开始游戏
			} else break;
		} else if (n == 0) {
			gemeDraw();
			int isyes = MessageBox(GetHWnd(), "YOU FAIL 是否继续", "PITY", MB_YESNO);  // 失败消息框
			if (isyes == IDYES) {
				goto flag;  // 重新开始游戏
			} else break;
		}
	}

	while (1);
	return 0;
}
