#include "script.h"
#include "select.h"
#include "graphic.h"
#include "message.h"

//キー入力待ちかどうか 1:入力待ち 0:入力待ちではない
int g_waitKeyInput;
//選択ボックス選択待ち
int g_waitSelectBoxInput;
//条件分岐に使う
char* g_select[10][2];



//キー入力待ち状態にする
void setWaitKeyInput()
{
	g_waitKeyInput = WAIT_UNTIL_KEY_INPUT;
}

//キー入力待ち状態を解除
void unsetWaitKeyInput()
{
	g_waitKeyInput = NO_WAIT;
}

//キー入力待ち状態かを取得
int getWaitKeyInput()
{
	return g_waitKeyInput;
}

//選択ボックス選択待ち状態にする
void setWaitSelectBoxInput()
{
	g_waitSelectBoxInput = WAIT_UNTIL_KEY_INPUT;
}

//選択ボックスの状態を取得
int getWaitSelectBoxInput()
{
	return g_waitSelectBoxInput;
}

//選択ボックスがクリックされたときの処理
void checkSelectBoxClicked(ScriptInformation* scriptInfo)
{
	int choice, line, k;

	if( getWaitSelectBoxInput() == NO_WAIT ) {
		return;
	}

	//どの選択ボックスがクリックされたか
	choice = isClickedSelectBox();
	if( choice == 0 ) {
		//どこもクリックされていない
		return;
	}

	//ラベルが何行目にあるかを取得
	line = searchScriptLabel( g_select[choice - 1][1] , scriptInfo );
	//指定したラベルが見つからなかった
	if( line == -1 ) {
		for(k=0; k<1000; k++)DrawFormatString( 0, 0, GetColor( 255 , 255 , 255 ) , "スクリプトエラー:条件分岐の指定ラベルが間違っています(%d行目)\n",scriptInfo->currentLine + 1 ) ;
		WaitTimer(1000);
		return;
	}
	//読み取り中の行番号をラベルの行に移動
	scriptInfo->currentLine = line;

	//入力待ち解除
	g_waitSelectBoxInput = NO_WAIT;
}


//選択ボックスを

//スクリプトファイルを読み込む
//戻り値 -1 : 失敗  0 : 成功
int loadScript(const char* filename, ScriptInformation* scriptInfo)
{
	int pos;
	char c;
	//スクリプトファイル
	FILE* fp;

	//スクリプト情報を初期化
	memset( scriptInfo , 0, sizeof(ScriptInformation) );

	//スクリプトファイルを開く
	fp = fopen(filename, "r");
	if( fp == NULL ) {
		//ファイル読み込みに失敗
		DrawFormatString( 0, 0, GetColor( 255 , 255 , 255 ) , "スクリプト %s を読み込めませんでした\n ", filename ) ;
		WaitTimer(1000);
		return -1;
	}

	//script書き込み時に使用
	pos = 0;

	for( ;; ) {
		//一文字読み込み
		c = fgetc( fp );
		//ファイルの終わりかどうか
		if( feof( fp ) ) {
			break;
		}
		//文章先頭の空白部分を読み飛ばす
		while( (c == ' ' || c == '\t') && pos == 0 && !feof( fp ) ) {
			c = fgetc( fp );
		}

		if( pos >= SCRIPT_MAX_STRING_LENGTH - 1 ) {
			//1行の文字数が多すぎる
			DrawFormatString( 0, 0, GetColor( 255 , 255 , 255 ) , "error: 文字数が多すぎます (%d行目) ", scriptInfo->currentLine ) ;
			WaitTimer(1000);
			return -1;
		}

		//改行文字が出てきた場合，次の行へ移動
		if( c == '\n' ) {
			//空行は読み飛ばす
			if( pos == 0 ) {
				continue;
			}
			//\0を文字列の最後に付ける
			scriptInfo->script[ scriptInfo->currentLine ][ pos ] = '\0';
			//次の行に移動
			scriptInfo->currentLine++;
			//書き込み位置を0にする
			pos = 0;
		}else {
			//書き込み
			scriptInfo->script[ scriptInfo->currentLine ][ pos ] = c;
			//文字書き込み位置をずらす
			pos++;
		}
	}
	//最大行数
	scriptInfo->maxLineNumber = scriptInfo->currentLine;
	//読み込み中の行を0にする
	scriptInfo->currentLine = 0;
	//スクリプトファイル名を設定
	scriptInfo->filename = filename;

	return 0;
}

//文字列分割(1行の最大文字数は SCRIPT_MAX_STRING_LENGTH)
//src : 分割したい文字列
//dest: 分割された文字列
//delim: 区切り文字
//splitNum : 最大分割数
void splitString(const char* src, char* dest[], const char* delim, int splitNum)
{
	int i;
	char* cp;
	char* copySrc;

	//元の文章をコピーする
	copySrc = (char*)malloc( sizeof(int) * SCRIPT_MAX_STRING_LENGTH + 1);
	strncpy( copySrc, src, SCRIPT_MAX_STRING_LENGTH );
	cp = copySrc;

	//strtokを使って copySrc をdelim区切りで分割する
	for( i = 0; i < splitNum ; i++ ) {
		//分割対象文字列が無くなるまで分割
		if( (dest[i] = strtok(cp, delim)) == NULL ) {
			break;
		}
		//2回目にstrtokを呼び出す時は，cpをNULLにする
		cp = NULL;
	}
	//分割された文字列の最後の要素はNULLとしておく
	dest[i] = NULL;
}

//デバッグ用
//elemの要素を表示
void printElements(char* elem[])
{
	int i;
	for( i = 0; elem[i] != NULL; i++ ) {
		printf("%d : %s\n", i + 1, elem[i] );
	}
}

//スクリプト文を解読する
//戻り値 1: 成功  0: 失敗
int decodeScript(const char* scriptMessage, ScriptInformation* scriptInfo)
{
	int i, selectNum, line;
	//分割されたスクリプト文
	char* message[100];
	//条件分岐用
	char* selectMessage[10];


	//文字列分割時の区切り文字
	const char* delim = " ";
	const char* selectDelim = "@@";
	
	//スクリプト分割
	splitString( scriptMessage, message, delim, 100 );

	//分割に失敗した場合
	if( message[0] == NULL ) {
		return 0;
	}

	//message[0] が @@message の時は，メッセージ命令が来たと判断
	if( strncmp(message[0], "@@message", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		printf("メッセージ : %s\n", message[1] );
		setMessage( message[1] );
		setWaitKeyInput();
	}else if( strncmp(message[0], "@@addgraph", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		//表示する画像を登録
		addVisibleGraphic( atoi( message[1] ), atoi( message[2] ),
			atoi( message[3] ), atoi( message[4] ) );
	}else if( strncmp(message[0], "@@load", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		//画像を登録
		addGraphicNode( atoi( message[1] ), message[2] );

	}else if( strncmp(message[0], "@@removegraph", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		//画像を削除
		removeVisibleGraphic( atoi(message[1]) );

	}else if( strncmp(message[0], "@@select", SCRIPT_MAX_STRING_LENGTH) == 0 ) {

		for(i = 0; message[i + 1] != NULL; i++ ) {
			//条件を条件文章とジャンプラベルとに分ける
			splitString( message[i + 1], selectMessage, selectDelim, 2 );
			//条件文章
			g_select[i][0] = selectMessage[0];
			//ラベル
			g_select[i][1] = selectMessage[1];
		}
		//分岐数
		selectNum = i;

		//条件分岐の数は2個に固定 それ以外はエラーとする
		if( selectNum != 2 ) {
			DrawFormatString( 0, 0, GetColor( 255 , 255 , 255 ) , "スクリプトエラー:条件分岐の分岐数が間違っています(%d行目) \n ", scriptInfo->currentLine + 1 ) ;
			WaitTimer(1000);
			return 0;
		}
		//条件分岐のメッセージをセット
		setSelectBoxMessage( g_select[0][0], g_select[1][0] );

		//ここで分岐待ち処理をいれる
		setWaitSelectBoxInput();

	}else if( strncmp(message[0], "@@goto", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		//ラベルが何行目にあるかを取得
		line = searchScriptLabel( message[1], scriptInfo );
		//指定したラベルが見つからなかった
		if( line == -1 ) {
			DrawFormatString( 0, 0, GetColor( 255 , 255 , 255 ) , "スクリプトエラー:指定したラベルが見つかりませんでした(%d行目)\n", scriptInfo->currentLine + 1 ) ;
			WaitTimer(1000);
			return 0;
		}
		//読み取り中の行番号をラベルの行に移動
		scriptInfo->currentLine = line;

	}else if( strncmp(message[0], "@@label", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		//ラベルの場合はなにもしない
	}else if( strncmp(message[0], "@@opmusic", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
			//OP音楽再生
			PlaySoundMem( OPmusic , DX_PLAYTYPE_BACK ) ;
	}else if( strncmp(message[0], "@@stop", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		 StopSoundMem(OPmusic) ;
		 //音楽止める
	}
	else if( strncmp(message[0], "@@wait", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		setWaitKeyInput();
		//キー入力待ち	
	}else if( strncmp(message[0], "@@game", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		return gameflag = 1;
		//別ゲームへ飛ぶ
	}else if( strncmp(message[0], "@@game_2", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		return gameflag = 2;
		//別ゲームへ飛ぶ
	}else if( strncmp(message[0], "@@END", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
		WaitTimer(2000);
		DxLib_End();
		return 0;
		//ゲーム終了
	}else {
		DrawFormatString( 0, 0, GetColor( 255 , 255 , 255 ) , "スクリプトエラー(%d行目)\n", scriptInfo->currentLine + 1) ;
		return WaitTimer(2000);
	}
	//WaitTimer(2000);
	return 1;
}

//指定したラベルがある行数を探す
//戻り値 正の数: 指定したラベルの行番号 -1: エラー
int searchScriptLabel(const char* label, ScriptInformation* scriptInfo)
{
	//分割されたスクリプト文
	char* message[100];
	//文字列分割時の区切り文字
	const char* delim = " ";
	int i, line = -1;

	for( i = 0; i < scriptInfo->maxLineNumber; i++ ) { 
		//スクリプト分割
		splitString( scriptInfo->script[i] , message, delim, 100 );

		//分割に失敗した場合
		if( message[0] == NULL ) {
			return -1;
		}

		//指定したラベルを探す
		if( strncmp(message[0], "@@label", SCRIPT_MAX_STRING_LENGTH) == 0 ) {
			if( strncmp(message[1], label, SCRIPT_MAX_STRING_LENGTH) == 0 ) {
				//指定したラベルが見つかった時
				line = i;
				break;
			}
		}
	}

	return line;
}

