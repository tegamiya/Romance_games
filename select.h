#ifndef SELECT_H_
#define SELECT_H_

#include "DxLib.h"

#define GRAPHIC_SELECTBOX_FILENAME  "gazou/select.png"

#define SELECT_BOX_X 50
#define SELECT_BOX_Y 200

#define SELECT_BOX_WIDTH 700
#define SELECT_BOX_HEIGHT 50

#define SELECT_BOX_MESSAGE_Y 20

//フォントサイズを16とする
#define FONT_SIZE 16

#define SELECT_BOX_MESSAGE_MAX_LENGTH 100
#define SELECT_BOX_HIDE 0
#define SELECT_BOX_SHOW 1

//選択肢ボックス関係(選択肢は2つとする)
//白
extern int g_whiteColor;
//選択ボックスの読み込み
extern int g_selectBoxGraphicHandle;
//選択ボックスに表示するメッセージ
extern char g_selectBoxMessage[2][SELECT_BOX_MESSAGE_MAX_LENGTH];
//選択ボックスを表示するかどうか 0:非表示 1:表示
extern int g_selectBoxVisibleFlag;

//プロトタイプ宣言
void initSelectBox();
int isContainMousePointer(int x, int y, int width, int height);
void drawSelectBox();
void setSelectBoxMessage(const char* message1, const char* message2);
int isClickedSelectBox();


#endif
