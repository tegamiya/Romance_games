#include "select.h"

//選択肢ボックス関係(選択肢は2つとする)
//白
int g_whiteColor;
//選択ボックスの読み込み
int g_selectBoxGraphicHandle;
//選択ボックスに表示するメッセージ
char g_selectBoxMessage[2][SELECT_BOX_MESSAGE_MAX_LENGTH];
//選択ボックスを表示するかどうか 0:非表示 1:表示
int g_selectBoxVisibleFlag;


//選択ボックスの初期化
void initSelectBox()
{
	//白
	g_whiteColor = GetColor(255, 255, 255);
	//選択ボックスの読み込み
	g_selectBoxGraphicHandle = LoadGraph( GRAPHIC_SELECTBOX_FILENAME ); 
	//選択ボックスのメッセージ初期化
	memset( g_selectBoxMessage[0], 0, SELECT_BOX_MESSAGE_MAX_LENGTH);
	memset( g_selectBoxMessage[1], 0, SELECT_BOX_MESSAGE_MAX_LENGTH);
	//選択ボックスを表示しない
	g_selectBoxVisibleFlag = SELECT_BOX_HIDE;
}


//指定したボックス内にマウスが存在するかどうか
//戻り値 1:存在する 0:存在しない
int isContainMousePointer(int x, int y, int width, int height)
{
	int mouseX, mouseY;

	//マウスの座標を取得
	GetMousePoint( &mouseX, &mouseY );

	//ボックス内にマウス座標が存在するか
	if( (mouseX >= x && mouseX <= x + width) &&
		(mouseY >= y && mouseY <= y + height) ) {
			return 1;
	}
	return 0;
}

//選択ボックスにメッセージをセットする
void setSelectBoxMessage(const char* message1, const char* message2)
{
	//メッセージセット
	strncpy(g_selectBoxMessage[0], message1, SELECT_BOX_MESSAGE_MAX_LENGTH );
	strncpy(g_selectBoxMessage[1], message2, SELECT_BOX_MESSAGE_MAX_LENGTH );
	//選択ボックスを表示
	g_selectBoxVisibleFlag = SELECT_BOX_SHOW;
}

//選択肢ボックスを描画する
void drawSelectBox()
{
	//選択ボックスを表示するかどうか
	if( g_selectBoxVisibleFlag ) {
		//選択肢ボックス1がマウスポインタを含んでいた場合
		if( isContainMousePointer( SELECT_BOX_X, SELECT_BOX_Y,
			SELECT_BOX_WIDTH, SELECT_BOX_HEIGHT ) ) {
			//透明度0％とする
			SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );	
		}else {
			//透明度50%とする
			SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128);
		}
		//選択ボックスの描画
		DrawGraph( SELECT_BOX_X, SELECT_BOX_Y, g_selectBoxGraphicHandle, TRUE );

		//選択肢ボックス2がマウスポインタを含んでいた場合
		if( isContainMousePointer( SELECT_BOX_X, SELECT_BOX_Y + SELECT_BOX_HEIGHT,
			SELECT_BOX_WIDTH, SELECT_BOX_HEIGHT ) ) {
			//透明度0％とする
			SetDrawBlendMode( DX_BLENDMODE_ALPHA, 255 );	
		}else {
			//透明度50%とする
			SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128);
		}
		//選択ボックス2つ目の描画
		DrawGraph( SELECT_BOX_X, SELECT_BOX_Y + SELECT_BOX_HEIGHT,
			g_selectBoxGraphicHandle, TRUE);

		//アルファブレンドを元に戻す
		SetDrawBlendMode( DX_BLENDMODE_NOBLEND , 0 );

		//メッセージ表示
		DrawString(SELECT_BOX_X + 20, SELECT_BOX_Y + SELECT_BOX_MESSAGE_Y,
			g_selectBoxMessage[0], g_whiteColor);
		DrawString(SELECT_BOX_X + 20, SELECT_BOX_Y + SELECT_BOX_HEIGHT + SELECT_BOX_MESSAGE_Y,
			g_selectBoxMessage[1], g_whiteColor);
	}
}

//選択肢ボックスがクリックされたかどうか
//0: クリックされていない 1: 選択肢1がクリックされた 2: 選択肢2がクリックされた
int isClickedSelectBox()
{
	//選択肢ボックスが表示されているとき
	if( g_selectBoxVisibleFlag ) {
		//マウスの状態を調べる
		//左クリックされたとき
		if( (GetMouseInput() & MOUSE_INPUT_LEFT) != 0 ) {
			
			if( isContainMousePointer( SELECT_BOX_X, SELECT_BOX_Y,
				SELECT_BOX_WIDTH, SELECT_BOX_HEIGHT ) ) {
					//選択肢ボックス1がマウスポインタを含んでいた場合
					//選択ボックスのメッセージ初期化
					memset( g_selectBoxMessage[0], 0, SELECT_BOX_MESSAGE_MAX_LENGTH);
					memset( g_selectBoxMessage[1], 0, SELECT_BOX_MESSAGE_MAX_LENGTH);
					//選択ボックスを表示しない
					g_selectBoxVisibleFlag = SELECT_BOX_HIDE;
					//選択肢1がクリックされていることを通知
					return 1;
			}else if( isContainMousePointer( SELECT_BOX_X, 
				SELECT_BOX_Y + SELECT_BOX_HEIGHT,
				SELECT_BOX_WIDTH, SELECT_BOX_HEIGHT ) ) {
					//選択肢ボックス2がマウスポインタを含んでいた場合
					//選択ボックスのメッセージ初期化
					memset( g_selectBoxMessage[0], 0, SELECT_BOX_MESSAGE_MAX_LENGTH);
					memset( g_selectBoxMessage[1], 0, SELECT_BOX_MESSAGE_MAX_LENGTH);
					//選択ボックスを表示しない
					g_selectBoxVisibleFlag = SELECT_BOX_HIDE;
					//選択肢2がクリックされていることを通知
					return 2;
			}
		}
	}
	return 0;
}	
