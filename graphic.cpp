#include "graphic.h"

//グラフィック管理
GraphicNode g_graphicManager[GRAPHIC_MAX_NUM];

//表示するグラフィックの管理
VisibleGraphicNode g_visibleGraphic[VISIBLE_GRAPHIC_MAX_NUM];

//初期化
void initGraphicNode()
{
	//グラフィック管理初期化
	memset( &g_graphicManager, 0, sizeof(GraphicNode) * GRAPHIC_MAX_NUM );
	//表示するグラフィックの管理初期化
	memset( &g_visibleGraphic, 0, sizeof(VisibleGraphicNode) * VISIBLE_GRAPHIC_MAX_NUM );
}

//グラフィックを読み込む
//戻り値 -1: 失敗 0: 成功
int addGraphicNode(int id, const char* graphFilename)
{
	int i;
	//idが重複していないか確認
	for(i = 0; i < GRAPHIC_MAX_NUM ; i++) {
		if( id == g_graphicManager[i].id ) {
			DrawString(0, 0, "idが重複しています", GetColor( 255, 255, 255 ) ) ;
			return -1;
		}
	}

	//利用していないノードを見つける
	for(i = 0; i < GRAPHIC_MAX_NUM; i++) {
		if( g_graphicManager[i].id == 0 ) {
			break;
		}
	}
	//グラフィックノードの空きがない
	if( i == GRAPHIC_MAX_NUM ) {
		DrawString(0, 0, "グラフィックノードの空きがありません", GetColor( 255, 255, 255 ) ) ;
		return -1;
	}

	//画像読み込み
	g_graphicManager[i].graphicHandle = LoadGraph( graphFilename );
	
	//読み込み失敗時
	if( g_graphicManager[i].graphicHandle == -1 ) {
		DrawString(0, 0, "画像読み込みに失敗しました", GetColor( 255, 255, 255 ) ) ;
		g_graphicManager[i].graphicHandle = 0;
		return -1;
	}

	//idをセット
	g_graphicManager[i].id = id;
	return 0;
}

//idからグラフィックハンドルを取得する
int getGraphicHandle(int id)
{
	int i = 0;
	for(i = 0; i < GRAPHIC_MAX_NUM; i++ ) {
		if( id == g_graphicManager[i].id ) {
			return g_graphicManager[i].graphicHandle;
		}
	}
	return -1;
}


//画面に画像を表示する
//idにはGraphicNodeのidを指定， graphicIdはいまから描画する画像にidを付ける
//posX, posYは画像を表示する位置
//戻り値 -1: 失敗 0: 成功
int addVisibleGraphic(int id, int graphicId, int posX, int posY)
{
	int i;
	//idからグラフィックハンドルを取得
	int handle = getGraphicHandle( id );

	//graphicHandleの取得失敗
	if( handle == -1 ) {
		DrawString(0, 0, "画像は登録されていません", GetColor( 255, 255, 255 ) ) ;
		return -1;
	}

	//graphicIdが重複していないか確認
	for(i = 0; i < VISIBLE_GRAPHIC_MAX_NUM ; i++) {
		if( graphicId == g_visibleGraphic[i].graphicId ) {
			DrawString(0, 0, "graph idが重複しています", GetColor( 255, 255, 255 ) ) ;
			return -1;
		}
	}

	//利用していないノードを見つける
	for(i = 0; i < VISIBLE_GRAPHIC_MAX_NUM; i++) {
		if( g_visibleGraphic[i].graphicId == 0 ) {
			break;
		}
	}

	//ノードの空きがない
	if( i == VISIBLE_GRAPHIC_MAX_NUM ) {
		DrawString(0, 0, "画面にこれ以上画像を表示できません", GetColor( 255, 255, 255 ) ) ;
		return -1;
	}

	//情報登録
	//グラフィックハンドル登録
	g_visibleGraphic[i].graphicHandle = handle;
	//graphicIdを登録
	g_visibleGraphic[i].graphicId = graphicId;
	//座標を登録
	g_visibleGraphic[i].x = posX;
	g_visibleGraphic[i].y = posY;
	//輝度情報登録
	g_visibleGraphic[i].bright = 0;
	//モード
	g_visibleGraphic[i].mode = GRAPHIC_MODE_FADEIN;
	
	return 0;
}

//指定したgraphicIdの画像を削除する
//戻り値 -1: 失敗 0:成功
int removeVisibleGraphic(int graphicId)
{
	int i;
	for(i = 0; i < VISIBLE_GRAPHIC_MAX_NUM ; i++) {
		if( graphicId == g_visibleGraphic[i].graphicId ) {
			//指定したグラフィックが見つかった
			//モードをフェードアウトとする
			g_visibleGraphic[i].mode = GRAPHIC_MODE_FADEOUT;
			return 0;
		}
	}
	return -1;
}


//画像描画
void drawVisibleGraphic()
{
	int i;
	for(i = 0; i < VISIBLE_GRAPHIC_MAX_NUM; i++ ) {
		//graphicIdが0でなければ（画像が存在すれば)
		if( g_visibleGraphic[i].graphicId != 0 ) {

			//フェードインを行うとき
			if( g_visibleGraphic[i].mode == GRAPHIC_MODE_FADEIN ) {
				//輝度を上げる
				g_visibleGraphic[i].bright += GRAPHIC_FADEIN_FADEOUT_SPEED;
				if( g_visibleGraphic[i].bright > GRAPHIC_MAX_BRIGHT ) {
					//輝度が最大値に達した時
					g_visibleGraphic[i].bright = GRAPHIC_MAX_BRIGHT;
					//フェードイン解除
					g_visibleGraphic[i].mode = GRAPHIC_MODE_NONE;
				}
			}

			//フェードアウトを行うとき
			if( g_visibleGraphic[i].mode == GRAPHIC_MODE_FADEOUT ) {
				//輝度を下げる
				g_visibleGraphic[i].bright -= GRAPHIC_FADEIN_FADEOUT_SPEED;
				if( g_visibleGraphic[i].bright <= 0 ) {
					//画像を画面から消す
					g_visibleGraphic[i].graphicHandle = 0;
					g_visibleGraphic[i].graphicId = 0;
					g_visibleGraphic[i].x = 0;
					g_visibleGraphic[i].y = 0;
					g_visibleGraphic[i].bright = 0;
					g_visibleGraphic[i].mode = GRAPHIC_MODE_NONE;
					continue;
				}
			}

			//輝度セット
			SetDrawBright(g_visibleGraphic[i].bright,
					g_visibleGraphic[i].bright, g_visibleGraphic[i].bright );
			//アルファブレンドを行う
			SetDrawBlendMode(DX_BLENDMODE_ALPHA , g_visibleGraphic[i].bright );

			//指定した座標に画像描画
			DrawGraph( g_visibleGraphic[i].x, g_visibleGraphic[i].y,
				g_visibleGraphic[i].graphicHandle, TRUE);
		}
	}
	//輝度情報をデフォルトに戻しておく
	SetDrawBright( GRAPHIC_MAX_BRIGHT, GRAPHIC_MAX_BRIGHT, GRAPHIC_MAX_BRIGHT);
	//ブレンドモードを元にもどしておく
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, GRAPHIC_MAX_BRIGHT);
}
