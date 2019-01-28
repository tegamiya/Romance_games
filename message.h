#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "DxLib.h"

//注意点：文字コードはSHIFT_JIS（Windows標準）を前提としている
//		  他の文字コードでは正常に動作しない
//SHIFT_JISの場合，日本語は2バイトで表される
//上位バイトが0x81～0x9F、0xE0～0xFCの範囲に収まる

int isJapaneseCharacter(unsigned char code);
void writeSubstring(char* message, int start, int len, int posX, int posY, int color, int bufferLine);
void drawMessage();
void initMessage();
void setMessage(const char* message);

//メッセージのフォントの大きさ
#define MESSAGE_FONT_SIZE 16
//仮想バッファの最大文字数
#define MESSAGE_MAX_LENGTH 31
//仮想バッファの最大行数
#define MESSAGE_MAX_LINE 5
//メッセージボックスのX座標
#define MESSAGE_BOX_X_POS 100
//メッセージボックスのY座標
#define MESSAGE_BOX_Y_POS 450

//メッセージボックスの画像
#define MESSAGE_BOX_GRAPHIC_FILENAME "gazou/boxd3.jpg"

//表示したいメッセージ
extern char g_message[MESSAGE_MAX_LENGTH * MESSAGE_MAX_LINE];

//画面にメッセージを表示する際にしようする仮想テキストバッファ
extern char g_messageBuffer[MESSAGE_MAX_LINE][MESSAGE_MAX_LENGTH];

#endif
