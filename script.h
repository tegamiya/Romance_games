#ifndef SCRIPT_H_
#define SCRIPT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//スクリプトは最大1000行まで読み込む
#define SCRIPT_MAX_LINE 1000
//スクリプト最大文字数
#define SCRIPT_MAX_STRING_LENGTH 300

//キー入力待ち
#define WAIT_UNTIL_KEY_INPUT 1
#define NO_WAIT 0

typedef struct ScriptInformation_tag {
	int maxLineNumber;			//スクリプト行数
	int currentLine;			//現在何行目を実行しているか
	const char* filename;		//ファイル名
	char script[SCRIPT_MAX_LINE][SCRIPT_MAX_STRING_LENGTH];
} ScriptInformation;

//キー入力待ちかどうか 1:入力待ち 0:入力待ちではない
extern int g_waitKeyInput;
//選択ボックス選択待ち
extern int g_waitSelectBoxInput;
//条件分岐に使う
extern char* g_select[10][2];

//★別ゲームに飛ぶ
extern int gameflag;

//音楽再生
extern int OPmusic;

//プロトタイプ宣言
int loadScript(const char* filename, ScriptInformation* scriptInfo);
void splitString(const char* src, char* dest[], const char* delim, int splitNum);
void printElements(char* elem[]);
int decodeScript(const char* scriptMessage, ScriptInformation* scriptInfo);
int searchScriptLabel(const char* label, ScriptInformation* scriptInfo);
void setWaitKeyInput();
void unsetWaitKeyInput();
int getWaitKeyInput();
void setWaitSelectBoxInput();
int getWaitSelectBoxInput();
void checkSelectBoxClicked(ScriptInformation* scriptInfo);

#endif
