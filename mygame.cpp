
#include "DxLib.h"
#include <math.h>
#include<time.h>
#include<conio.h>

#include "PrivateFontManager.h"

#include "mygame.h"
#include "script.h"

#define PI  3.14159

int game(void);
int game2(void);


int game(void)
{
	//データ定義
	int Hx, Hy ; //砲台の座標
	int Hm ; //砲台の移動方向
	int Hsc ; //砲台のショット間隔カウンタ
	int Px, Py ; //自機の座標
	int Mg ; //ミサイルのグラフィック

	int tg , taiho ; //手錠変数
	int stop = 0 ; //手錠受けたらストップする
	int teki, jibun ;
	int DEATHcnt; //メンヘラカウンタ

	int Mx[ MAX_M ] , My[ MAX_M ] ; //ミサイルの座標
	int Msx[ MAX_M ], Msy[ MAX_M ] ; //ミサイルの速度
	int Mv[ MAX_M ] ; //ミサイルの使用状態
	double Ma[ MAX_M ] ; //ミサイルの角度
	int Mc[ MAX_M ] ; //ミサイルの追尾カウンタ

	//-------------

	int Key , Graph , night;
	LONGLONG Time ;
	int i, k=0, Frame=0;

	SetGraphMode( 1280, 960, 16 );

	if(DxLib_Init() == -1 )return -1;

	SetDrawScreen( DX_SCREEN_BACK );

	srand((unsigned)time(NULL));
	
	//初期化
	{
		Mg = LoadGraph("gazou/loveletter2.png");
		teki= LoadGraph("gazou/loveletter_girl_1.png");
		jibun= LoadGraph("gazou/man1.png");
		tg = LoadGraph("gazou/tejyou.png");
		taiho= LoadGraph("gazou/taiho.png");

		DEATHcnt = 1000;

		//自機座標
		Px = 320 ; Py = 200 ;
		//砲台座標
		Hx = 320 ; Hy = 30 ;
		//砲台の移動方向
		Hm = 3 ;
		//砲台の移動間隔カウンタ
		Hsc = 30 ;
		//ミサイルデータ初期化
		for(i=0; i<MAX_M; i++) Mv[i] = 0 ;

	}

	Graph = LoadGraph( "gazou/school.jpg" ) ;
	night = LoadGraph( "gazou/night_school.png" ) ;

	//フォント
	PrivateFontManager lfm;
	auto ttfh = lfm.InstallPrivateFont( _T("cinecaption227.TTF") ) ;
	ChangeFont( lfm.GetFontNameFromList( ttfh ) , DX_CHARSET_DEFAULT ) ;


	//文字

	SetFontSize(50);
	int  kaiwa = LoadGraph( "gazou/kaiwa.png" ) ; 


	//ゲームループ
	Time = GetNowHiPerformanceCount() + 1000000/60 ;
	while( ProcessMessage() == 0 && CheckHitKey( KEY_INPUT_ESCAPE ) == 0 )
	{

		DEATHcnt--;

		// 終了処理
		Frame++;


		//入力
		Key = GetJoypadInputState( DX_INPUT_KEY_PAD1 ) ;

		if(stop <= 0){
			if( Key & PAD_INPUT_RIGHT ) Px += 10 ;
			if( Key & PAD_INPUT_LEFT ) Px -= 10 ;
			if( Key & PAD_INPUT_UP ) Py -= 10 ;
			if( Key & PAD_INPUT_DOWN ) Py += 10 ;
		}

		if( Px > 1280 -16 )Px = 1280 -16 ;
		if( Px < 0 )Px = 0 ;
		if( Py > 960 -16 )Py =960 -16 ;
		if( Py < 0 )Py = 0 ;

		//フレームごとに減らす
		stop--;
		
		//------ミサイルの移動処理 ↓↓↓
		for(i=0; i<MAX_M ; i++)
		{
			//ミサイルデータが無効だったらスキップ
			if(Mv[i] == 0)continue;

			//自機に当たっていたらミサイルデータ無効
			if(((Mx[i] > Px && Mx[i] < Px + 80) || (Px > Mx[i] && Px < Mx[i] + 100)) &&
				((My[i] > Py && My[i] < Py + 80) || (Py > My[i] && Py < My[i] + 100 )))
			{
				Mv[i] = 0;
				if(i==0)stop = 50;
				continue ;
			}


			//追尾カウンタが規定値に来ていなければ追尾処理　100フレーム追いかける
		   if(Mc[i] < 100 )
		   {
			   double ax, ay, bx, by ;

			   bx = cos(Ma[i]) ;
			   by = sin(Ma[i]) ;
			   ax = (Px + 16) - Mx[i] ;
			   ay = (Py + 16) - My[i] ;

			   Ma[i] += ( ax * by - ay * bx <0.0 ) ? + PI /180 * 15: - PI /180 *15 ;
		   }


		   //追尾カウンタ加算
		   Mc[i]+=3 ;



		   if(i != 0){
				 //速度変更
				 Msx[i] += (int)(cos(Ma[i]) * 10.0 ) ;
				 Msy[i] += (int)(sin(Ma[i]) * 10.0 ) ;

				//移動
				Mx[i] = ( Mx[i] * 100 + Msx[i] )/ 100 ;
				My[i] = ( My[i] * 100 + Msy[i] )/ 100 ;
		   }

		   	if(i == 0){ //i=0の手錠だけ速さを変更
				 //速度変更
				 Msx[i] += (int)(cos(Ma[i]) * 35.0 ) ;
				 Msy[i] += (int)(sin(Ma[i]) * 35.0 ) ;

				 //移動
				 Mx[i] = ( Mx[i] * 100 + Msx[i] )/ 100 ;
				 My[i] = ( My[i] * 100 + Msy[i] )/ 100 ;
		   }


		   if(Mx[i] < -100 || Mx[i] >1280 ||
			   My[i]< -100 || My[i] >960 )Mv[i] = 0 ;

		}   //------ミサイルの移動処理　↑↑↑


		//砲台移動
			Hx += Hm ;

			//画面端まで来てたら反転
			if(Hx > 1280 -16 || Hx <0 ) Hm *= -1 ;

			Hsc--;

			if(Hsc == 0)
			{
				for(i=0; i<MAX_M ; i++)
				{
					if(Mv[i] == 0)break;
				}

				//使われてないミサイルデータあったらしょっと
				if( i != MAX_M)
				{
					Mx[i] = Hx +16 ;
					My[i] = Hy +16;

					Msx[i] = 0;
					Msy[i] = 0;

					Ma[i] = PI/2 ;

					Mc[i] = 0;

					Mv[i] = 1;
				}
				Hsc = 30 ;
			}
		
			//描画
			ClearDrawScreen();
			

			if(DEATHcnt>0){
				DrawGraph( 0 , 0 , Graph , FALSE ) ;
				if(stop <= 0)DrawGraph( Px, Py-50, jibun, TRUE);
				if(stop > 0)DrawGraph( Px, Py-50, taiho, TRUE);
				DrawGraph(Hx-8, Hy -8, teki, TRUE);
			}
			else{

				Mg = LoadGraph("gazou/houcho.png");
				teki= LoadGraph("gazou/woman.png");

				DrawGraph( 0 , 0 , night , FALSE ) ;
				DrawGraph( Px, Py-50, jibun, TRUE);
				if(stop > 0)DrawGraph( Px, Py-50, taiho, TRUE);
				DrawGraph(Hx-8, Hy -8, teki, TRUE);
				if(DEATHcnt == -50)break;
			}


			// k = 回転に使う
			k++;
			k = k%360;

			//レター包丁表示
			for(i=0; i<MAX_M; i++)
			{
				if(Mv[i] == 0)continue ;
				if(i != 0)DrawRotaGraph(Mx[i], My[i], 1.0, Ma[i], Mg , TRUE) ;
				//手錠
				if(i == 0){
						 DrawRotaGraph2(Mx[i], My[i], 50,50,1.0, PI/90*k*2, tg, TRUE); // 回転描画
						 DrawCircle(Mx[i]-100, My[i]+100, GetColor(255, 0, 0), TRUE); // 回転の中心点
					}
				}


			     if( DxLib_Init() == -1 )    // ＤＸライブラリ初期化処理
					 {
						 return -1;      // エラーが起きたら直ちに終了
					 }




			//文字
			if(Frame<2){
					DrawGraph( 200 , 550, kaiwa, TRUE);
					DrawString( 210 , 570 , "てすとてすとてすと"  , GetColor( 120 , 200 , 25 ) ) ;
			}
			if(Frame==2)WaitKey();

			ScreenFlip();

			while(GetNowHiPerformanceCount() < Time) {}
			Time += 1000000/ 60;
		
		}

		return  1;

		}



	int map[16][21] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,2,0,2,2,0,3,13,14,6,2,2,4,2,1,8,9,0,0,0,1},
  {1,0,0,0,0,0,3,2,2,0,2,2,5,2,1,0,0,13,14,0,1},
  {1,0,15,0,1,1,3,0,1,1,1,0,0,0,1,1,1,0,2,0,7},
  {1,2,0,2,0,1,3,4,2,0,1,2,0,2,0,0,1,0,0,2,1},
  {1,2,2,0,0,1,3,5,0,2,1,0,0,10,0,0,6,0,2,2,1},
  {1,3,3,3,3,3,3,0,0,2,0,2,0,2,0,0,1,0,15,15,1},
  {1,1,0,2,0,0,3,1,1,1,1,1,1,1,1,0,1,0,15,15,1},
  {1,15,0,2,0,1,3,2,11,12,0,2,2,2,0,1,0,0,15,15,1},
  {1,0,0,4,1,0,3,2,0,2,1,0,2,4,0,1,0,0,0,2,1},
  {1,0,2,5,0,0,3,3,0,2,1,2,0,5,0,1,0,0,2,0,1},
  {1,2,13,14,0,1,1,3,0,0,1,1,1,0,0,1,1,3,2,10,1},
  {1,0,0,0,6,8,9,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
  {1,0,6,2,0,0,1,2,2,2,0,2,0,2,0,11,12,3,0,3,1},
  {1,0,0,2,0,0,1,0,1,0,0,0,0,0,0,0,0,1,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};



int game2(void) {

	ChangeWindowMode(TRUE);
    DxLib_Init();
	SetDrawScreen(DX_SCREEN_BACK);

	int player[12], enemy[12], chip[16];
	int px = 1, py = 1;
	int pd = 0, ei = 0; //自分、敵の移動方向
	int ex = 18, ey = 13;
	int esa_num = 0;
	enum {PLAY, OVER, CLEAR, die} status = PLAY; //ステータスで変更できる範囲
	int eat;
	int stop = 0, R=5;

	SetGraphMode( 672, 512, 8 );

  LoadDivGraph("gazou/p.png", 12, 3, 4, 32, 32, player);
  LoadDivGraph("gazou/mobu.png",  12, 3, 4, 32, 32, enemy);
  LoadDivGraph("gazou/1212_4.png", 16, 16, 1, 32, 32, chip);

  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 21; x++) {
      if (map[y][x] == 7) esa_num++;
    }
  }

  eat = LoadSoundMem("eat.mp3");
  PlaySoundFile("bgm.mp3", DX_PLAYTYPE_LOOP);

  int  kaiwa = LoadGraph( "gazou/boxd2.jpg" ) ; 

  while (!ProcessMessage() && status == PLAY) {
    ClearDrawScreen();

	//stopカウント減らす
	stop--;



	//自分の移動方向
    int kx = px, ky = py;
	if(stop < 0){
    if (CheckHitKey(KEY_INPUT_LEFT)) {kx--; pd = 3;}
    if (CheckHitKey(KEY_INPUT_RIGHT)) {kx++; pd = 6;}
    if (CheckHitKey(KEY_INPUT_UP)) {ky--; pd = 9;}
    if (CheckHitKey(KEY_INPUT_DOWN)) {ky++; pd = 0;}
	}

    if (map[ky][kx] != 1 && map[ky][kx] != 4 && map[ky][kx] != 5 && map[ky][kx] != 13 && map[ky][kx] != 14 && map[ky][kx] != 8 && map[ky][kx] != 9 && map[ky][kx] != 15) {
      px = kx; py = ky;
    }
	//pd += rand()%3;

	//7　階段ならクリア
    if (map[py][px] == 7) {
      map[py][px] = 0;
      esa_num--;
      PlaySoundMem(eat, DX_PLAYTYPE_BACK, TRUE);
      if (esa_num <= 0) status = CLEAR;
    }

	//6　キノコなら少しの間足が止まる
    if (map[py][px] == 6) {
      map[py][px] = 0;
      stop = 80;
      PlaySoundMem(eat, DX_PLAYTYPE_BACK, TRUE);
    }

	//11,12　落とし穴なら死ぬ
    if (map[py][px] == 11 || map[py][px] == 12) {
      map[py][px] = 0;
	  status = die;
      PlaySoundMem(eat, DX_PLAYTYPE_BACK, TRUE);
    }
	//10 骸骨なら敵の足が速くなる
    if (map[py][px] == 10) {
      map[py][px] = 0;
	  R = 0;
	 DrawGraph( 70, 220, kaiwa  , FALSE );
    }


	//敵の移動方向
    kx = ex; ky = ey;
    if (rand() % (1+R) == 0) {
		if (kx > px){ kx--; ei = 3;}
		if (kx < px){ kx++; ei = 6;}
		if (ky > py){ ky--; ei = 9;}
		if (ky < py){ ky++; ei = 0;}
    }  

	if (rand() % (1+R) == 0) {
      kx += rand() % 3 - 1;
      ky += rand() % 3 - 1;
    }

    if (map[ky][kx] != 1 && map[ky][kx] != 4 && map[ky][kx] != 5 && map[ky][kx] != 13 && map[ky][kx] != 14 && map[ky][kx] != 8 && map[ky][kx] != 9 && map[ky][kx] != 15 ) {
      ex = kx; ey = ky;
    }

    if (ex == px && ey == py) status = OVER;

    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 21; x++) {
        DrawGraph(x * 32, y * 32, chip[map[y][x]], FALSE);
      }
    }
	

    DrawGraph(px * 32, py * 32, player[pd], TRUE);
    DrawGraph(ex * 32, ey * 32, enemy[ei], TRUE);

	
	if(stop > 0){
	 DrawGraph( 70, 220, kaiwa  , FALSE );
	 DrawString(80, 230, "キノコおいしい！もぐもぐ！　あれっ体が動かない・・・", GetColor(0, 0, 0));
	}

	if(R == 0){
	  DrawGraph( 70, 220, kaiwa  , FALSE );
	  DrawString(80, 230, "骨「痛っ」", GetColor(0, 0, 0));
	  WaitTimer(1300);
	  R = 1;
	}

	if (status == die) {
		DrawGraph( 70, 220, kaiwa  , FALSE );
		DrawString(80, 230, "おお勇者よ、死んでしまうとは情けない  死因：落とし穴", GetColor(0, 0, 0));
		WaitTimer(1300);
	}
    if (status == OVER) {
		DrawGraph( 70, 220, kaiwa  , FALSE );
		DrawString(80, 230, "捕まえた　ふふっ　死ぬまで一緒に暮らそうね", GetColor(0, 0, 0));
		WaitTimer(1300);
	}
    if (status == CLEAR){
		DrawGraph( 70, 220, kaiwa  , FALSE );
		DrawString(80, 230, "ハｱ…　ﾊｧ…　なんとか逃げ切ったか", GetColor(0, 0, 0));
		WaitTimer(1300);
	}


    ScreenFlip();
    WaitTimer(80);
    if (status != PLAY) {
		WaitTimer(1000);
		break;
	}
  }

  return 1;

}
