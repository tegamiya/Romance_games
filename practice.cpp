
#include "DxLib.h"
#include "select.h"
#include "graphic.h"
#include "script.h"
#include "message.h"
#include "mygame.h"

#define GAME_WINDOW_WIDTH 800
#define GAME_WINDOW_HEIGHT 600

#define SCRIPT_FILENAME "script.txt"

//スクリプトファイル
ScriptInformation g_script;

//前回マウスがクリックされた時間
int g_mouseClickedTime;

//プロトタイプ宣言
void initGame();


//★別ゲームに飛ぶ
int gameflag;
int game(void);
int game2(void);

int OPmusic;

//初期化
void initGame()
{
	// 描画先を裏画面にセット
	SetDrawScreen( DX_SCREEN_BACK );

	//フォントの大きさをセット
	SetFontSize( FONT_SIZE );

	// マウスを表示状態にする
	SetMouseDispFlag( TRUE );

	//選択ボックスの初期化
	initSelectBox();

	//グラフィックノード初期化
	initGraphicNode();

	//メッセージボックス初期化
	initMessage();

	//スクリプトロード
	loadScript( SCRIPT_FILENAME, &g_script );
}


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine, int nCmdShow )
{
	
	//ウィンドウモードで起動
	ChangeWindowMode( TRUE );
	//画面の大きさは800 * 600
	SetGraphMode( GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 16 ) ;
	//DxLib初期化
	if( DxLib_Init() == -1 ) {
		 return -1;
	}

	//ゲーム初期化
	initGame();

	OPmusic = LoadSoundMem( "morning.mp3" ) ;

	//メインループ
	while( ProcessMessage() == 0 && CheckHitKey( KEY_INPUT_ESCAPE ) == 0 ) {

		//画面クリア
		ClearDrawScreen();

		//int time = GetNowCount();
		//DrawFormatString( 0, 0, GetColor( 255 , 255 , 255 ) , "%d ", time ) ;


		if( ( GetMouseInput() & MOUSE_INPUT_LEFT ) != 0 ) {
			//マウス押しっぱなしの時にスクリプトがどんどん進まないようにする
			if( GetNowCount() - g_mouseClickedTime > 500 ) { 
				//キー入力待ちを解除
				unsetWaitKeyInput();
				//マウスクリックした時間を取得
				g_mouseClickedTime = GetNowCount();
			}
		}
		//選択ボックスが押されたか判定
		checkSelectBoxClicked(&g_script);

		//★ゲームフラグ１なら別ゲームへ飛ぶ
		if(gameflag == 1) {
			game();
			gameflag = 0;
			//WaitTimer(10000);
			SetGraphMode( GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 16 ) ;
			initGame();
			g_script.currentLine +=1 ; //スクリプト行数飛ばして展開飛ばす
			
		}

		if(gameflag == 2) {
			game2();
			gameflag = 0;
			//WaitTimer(10000);
			SetGraphMode( GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 16 ) ;
			initGame();
			g_script.currentLine +=1 ; //スクリプト行数飛ばして展開飛ばす
			
		}


		//画像描画
		drawVisibleGraphic();
		//選択肢ボックスの描画
		drawSelectBox();
		//メッセージ描画
		drawMessage();

		//スクリプト言語の解析
		if( getWaitKeyInput() != WAIT_UNTIL_KEY_INPUT &&
			getWaitSelectBoxInput() != WAIT_UNTIL_KEY_INPUT ) {
			decodeScript( g_script.script[ g_script.currentLine ], &g_script );
			g_script.currentLine++;
		}


		Sleep(50);

		ScreenFlip();
	}

	DxLib_End();
	return 0;
}

