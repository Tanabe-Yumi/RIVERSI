//////////////////////////////////////////////////////////////
//
//	<J2プログラミング 夏休み課題>
//
//		リバーシ
//					j1819 田辺 侑美
//
//		CPUとの対戦、定石を守って
//		・隅をとる	・ヤスリ攻め	・中割り	・開放度理論
//
//				最終更新日:2019/09/27
//
//////////////////////////////////////////////////////////////

#include<DxLib.h>
#include<stdio.h>
#include <string.h>			//memcpy()

#define MARGIN 20			//盤面の外側の余白
#define LAT 80				//１格子の大きさ
#define KURO 3				//黒の石
#define SHIRO 4				//白の石
#define PLAYER 80			//winner,senkou
#define CPU 67				//winner,senkou
#define OK 6				//石を置ける場所
#define C 61
#define X 62

int field[8][8] = {					//盤面
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,4,3,0,0,0},
	{0,0,0,3,4,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0} };
int Myfield[8][8] = {				//置ける場所を記録する
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,4,3,0,0,0},
	{0,0,0,3,4,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0} };
int openness[8][8] = {				//開放度
	{60,60,60,60,60,60,60,60},
	{60,60,60,60,60,60,60,60},
	{60,60,60,60,60,60,60,60},
	{60,60,60,60,60,60,60,60},
	{60,60,60,60,60,60,60,60},
	{60,60,60,60,60,60,60,60},
	{60,60,60,60,60,60,60,60},
	{60,60,60,60,60,60,60,60} };
int board[8][8] = {
	{0,C,0,0,0,0,C,0},
	{C,X,0,0,0,0,X,C},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{C,X,0,0,0,0,X,C},
	{0,C,0,0,0,0,C,0} };

int col_bann = GetColor(0, 169, 95);							//盤の緑
int col_whi = GetColor(255, 255, 255);							//文字の白
int col_bla = GetColor(0, 0, 0);								//黒
int col_ao = GetColor(43, 164, 223);							//青文字
int col_aka = GetColor(252, 57, 41);							//赤文字
int col_kin = GetColor(246, 221, 131);							//金色
int col_pink = GetColor(249, 162, 164);							//ピンク色
int col_gre = GetColor(84, 252, 96);							//タイトル表示のカラー
int col_put = GetColor(228, 208, 4);							//置ける場所の表示色
int blightness = 225;											//表示を明るさの設定
int Handle_kuro, Handle_shiro;									//石の画像ハンドル
int Handle_ukuro, Handle_ushiro;								//白黒決定の画像ハンドル
int Handle_walll, Handle_wallr, Handle_wallc, Hnadle_wallo;		//隠しの画像ハンドル
int Handle_sound;												//効果音のハンドル
int al = -150, ar = -150, ac = -150, ao = -150;					//隠しのアニメーション
int player_col,CPU_col, ue, shita;								//黒か白が入る
int kuro_num, shiro_num;										//黒と白の石の数
int zero_num, O_num = 0;										//石を置いてない場所、置ける場所の数
int turn = 1;													//何手目か
int senkou, koukou;												//PLAYER or CPU
int MyCol, OppCol;												//自分の色と、相手の色
int MX, MY;														//マウス座標
int least = 60, leastX, leastY;									//開放度が最小の筋と、そのx,y座標
int fieldX, fieldY;												//裏返す場所のx,y座標
int winner;														//勝者
int x, y;														//field[y][x]
int v = 1, g = 3;												//速さと加速度
int mark = 0, marka = 0;										//石を置ける場所の表示/非表示
int loop = 1;

void lattice(){
	int sx, sy, fx, fy;
	int i, j;
	sx = MARGIN, sy = MARGIN, fx = sx + LAT, fy = sy + LAT;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			DrawBox(sx, sy, fx, fy, col_whi, FALSE);
			sx += LAT, fx += LAT;
		}
		sx = MARGIN, fx = sx + LAT;
		sy += LAT, fy += LAT;
	}
}

void updowndecision() {										//上、下を決定
	if (GetRand(1) == 0) {
		ue = KURO;
		shita = SHIRO;
	}
	else {
		ue = SHIRO;
		shita = KURO;
	}
}

void kakushiClose() {										//隠すアニメーション
	while (ProcessMessage() == 0) {
		SetDrawScreen(DX_SCREEN_BACK);
		if (al < 240)
			al += 5;
		else if (ar < 240)
			ar += 5;
		else if (ac < 240)
			ac += 5;
		else if (ao < 240)
			ao += 5;
		DrawBox(0, 0, 680, 680, col_bann, TRUE);			//盤の緑
		DrawFormatString(700, 50, col_whi, "先攻後攻の決定\n");
		DrawGraph(240, al, Handle_walll, TRUE);
		DrawGraph(240, ar, Handle_wallr, TRUE);
		DrawGraph(240, ac, Handle_wallc, TRUE);
		DrawGraph(240, ao, Hnadle_wallo, TRUE);
		ScreenFlip();
		if (ao == 240)
			break;
	}
}

void kakushiOpen() {										//隠しを上にあげる
	while (ProcessMessage() == 0) {
		SetDrawScreen(DX_SCREEN_BACK);
		DrawBox(0, 0, 680, 680, col_bann, TRUE);			//盤の緑
		if (ue == KURO)
			DrawGraph(240, 230, Handle_ukuro, TRUE);
		else if (ue == SHIRO)
			DrawGraph(240, 230, Handle_ushiro, TRUE);
		DrawGraph(240, al, Handle_walll, TRUE);
		DrawGraph(240, ar, Handle_wallr, TRUE);
		DrawGraph(240, ac, Handle_wallc, TRUE);
		DrawGraph(240, ao, Hnadle_wallo, TRUE);
		al -= 5, ar -= 5, ac -= 5, ao -= 5;
		ScreenFlip();
		if (al == -240)
			break;
	}
}

void start() {
	int x = 100;
	SetFontSize(250);
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawFormatString(x-8, 156, col_kin, "START");
	DrawFormatString(x, 160, col_aka, "START");
	ScreenFlip();
	WaitTimer(880);
	for (;;) {
		ClearDrawScreen();
		x += v;
		v += g;
		DrawFormatString(x - 8, 155, col_kin, "START");
		DrawFormatString(x, 160, col_aka, "START");
		ScreenFlip();
		if (x >= 1000)
			break;
	}
	SetFontSize(30);
	v = 1;								//初期化
}

int NumberofStone(int col) {			//盤面の石の数を数える
	int x, y;
	int number = 0;
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {
			if (field[y][x] == col)
				number++;
		}
	}
	return number;
}

int OnBoard(int x, int y, int a, int b) {								//渡した場所が盤上にあるかを調べる
	if ((x + a) < 0 || (x + a) > 7 || (y + b) < 0 || (y + b) > 7)		//xからa方向、yからb方向に進んで、外に出た
		return 1;
	return 0;	//中にいる
}

int LineResearch(int q,int p,int b,int a,int Col,int nCol) {			//自分の石までの相手の石の数を返す
	int i;
	for (i = 1; field[q + i * b][p + i * a] == nCol; i++){}
	if (OnBoard(p, q, a*i, b * i) == 0 && field[q + i * b][p + i * a] == Col)
		return i - 1;
	else
		return 0;
}

int PutCanOrNot(int Col,int nCol) {										//石が置ける場所の判定
	int x, y, a, b;
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {										//自分の場所
			if (Myfield[y][x] != KURO && Myfield[y][x] != SHIRO) {		//石がある時は変えない
				for (b = -1; b <= 1; b++) {								//8方向
					for (a = -1; a <= 1; a++) {
						if (a == 0 && b == 0)
							continue;
						if (LineResearch(y, x, b, a, Col, nCol) > 0)
							Myfield[y][x] = OK;							//OKの時置ける
					}
				}
			}
		}
	}
	return 0;
}

int MousePosition(int a) {						//カーソルの、盤面の位置を返す
	int MouseX, MouseY;
	GetMousePoint(&MouseX, &MouseY);
	if (a == 24)								//x座標を返す
		return (MouseX - MARGIN) / LAT;
	else if (a == 25)							//y座標を返す
		return (MouseY - MARGIN) / LAT;
}

void TurnOver(int fieldX, int fieldY, int Col, int nCol) {				//間の石を裏返す
	int number, a, b, i;
	for (b = -1; b <= 1; b++) {
		for (a = -1; a <= 1; a++) {										//8方向と今の場所
			if (a == 0 && b == 0)										//今の場所は処理しない
				continue;
			number = LineResearch(fieldY, fieldX, b, a, Col, nCol);		//間にある相手の石の数
			for (i = 1; i <= number; i++) {								//間の石の数だけ
				field[fieldY + b * i][fieldX + a * i] = Col;			//裏返し
			}
		}
	}
}

int CornerAttack() {									//盤の隅からとる
	int i, j;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			if (Myfield[i * 7][j * 7] == OK) {			//置ける場合
				fieldX = j * 7, fieldY = i * 7;
				return 0;
			}
		}
	}
	return 1;											//置けない場合
}

int Direction(int num) {//ヤスリ攻めの方向を決める
	switch (num) {
	case 0:				//左 or 上にいる
		return 1;		//右 or 下方向を返す
	case 7:				//右 or 下にいる
		return -1;		//左 or 上方向を返す
	default:
		break;
	}
	return 0;
}

int FileAttack_kai(int Col,int nCol) {
	int i, j, k, dir;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			if (Myfield[i * 7][j * 7] == Col) {
				dir = Direction(j * 7);									//横方向
				for (k = 1; k < 7; k++) {								//自分の色の時、相手の色の時はその隣
					if (Myfield[i * 7 + 0][j * 7 + k * dir] == Col|| Myfield[i * 7 + 0][j * 7 + k * dir] == nCol)
						continue;
					if (Myfield[i * 7 + 0][j * 7 + k * dir] == OK) {	//置ける
						fieldX = j * 7 + k * dir, fieldY = i * 7;
						return 0;										//成功
					}
					else
						break;											//隣に行かない
				}
			}
		}
	}
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			if (Myfield[i * 7][j * 7] == Col) {
				dir = Direction(i * 7);									//上下方向
				for (k = 1; k < 7; k++) {
					if (Myfield[i * 7 + k * dir][j * 7 + 0] == Col || Myfield[i * 7 + k * dir][j * 7 + 0] == nCol)
						continue;
					if (Myfield[i * 7 + k * dir][j * 7 + 0] == OK) {
						fieldX = j * 7, fieldY = i * 7 + k * dir;
						return 0;										//成功
					}
					else
						break;
				}
			}
		}
	}
	return 1;															//ヤスリ攻めできない場合
}

int DegreeofOpenness(int fieldX, int fieldY, int Col, int nCol) {					//開放度理論
	int number = 0, a, b, c, d, i, count;
	for (b = -1; b <= 1; b++) {														//どの方向に何個裏返せるか
		for (a = -1; a <= 1; a++) {													//8方向と今の場所
			if (a == 0 && b == 0)													//今の場所は処理しない
				continue;
			count = LineResearch(fieldY, fieldX, b, a, Col, nCol);					//間にある相手の石の数
			for (i = 1; i <= count; i++) {											//裏返される各石について、開放度を数える
				for (d = -1; d <= 1; d++) {
					for (c = -1; c <= 1; c++) {										//8方向で、空いている場所の数
						if (c == 0 && d == 0)
							continue;
						if (field[fieldY + b * i + d][fieldX + a * i + c] == 0)		//石が置いていない時
							number++;
						/* a*i,b*iで進む方向を定義し、c,dで確認する方向を定義している */
					}
				}
			}
		}
	}
	return number;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetMainWindowText("REVERSI");
	SetWindowIconID(333);
	ChangeWindowMode(TRUE);							//非全画面にセット
	SetGraphMode(1000, 680, 32);					//画面サイズ指定
	SetOutApplicationLogValidFlag(FALSE);			//Log.txtを生成しないように設定
	if (DxLib_Init() == 1) { return -1; }			//初期化に失敗時にエラーを吐かせて終了
	SetMouseDispFlag(TRUE);							// マウスを表示状態にする
	//画像の読み込み//
	Handle_kuro = LoadGraph("black.PNG");
	Handle_shiro = LoadGraph("white.PNG");
	Handle_ukuro = LoadGraph("black_up.PNG");
	Handle_ushiro = LoadGraph("white_up.PNG");
	Handle_walll = LoadGraph("wall_left2.PNG");
	Handle_wallr = LoadGraph("wall_right2.PNG");
	Handle_wallc = LoadGraph("wall_center2.PNG");
	Hnadle_wallo = LoadGraph("wall_up2.PNG");
	Handle_sound = LoadSoundMem("sound1.mp3");

	ClearDrawScreen();												//裏画面消す
	SetDrawScreen(DX_SCREEN_BACK);									//描画先を裏画面に
	ChangeFont("Snap ITC");
	SetFontSize(180);
	DrawFormatString(0, 260, col_gre, "REVERSI");					//タイトル表示
	ScreenFlip();
	WaitTimer(3000);
	for (int i = 0; i < 45; i++) {									//暗くなって消えていく
		ClearDrawScreen();
		blightness -= 5;
		SetDrawBright(blightness, blightness, blightness);
		DrawFormatString(0, 260, col_gre, "REVERSI");
		ScreenFlip();
	}
	SetDrawBright(225, 225, 225);									//明るさを戻す
	ChangeFont("Arial-ltalicMT");									//フォント設定
	SetFontSize(30);
	WaitTimer(1000);
	//先攻の決定//
	ClearDrawScreen();
	SetDrawScreen(DX_SCREEN_BACK);
	DrawBox(0, 0, 680, 680, col_bann, TRUE);						//盤の緑
	ScreenFlip();
	WaitTimer(800);
	DrawFormatString(700, 50, col_whi, "先攻後攻の決定\n");
	ScreenFlip();
	updowndecision();
	WaitTimer(1800);
	kakushiClose();
	WaitTimer(880);
	SetDrawScreen(DX_SCREEN_BACK);
	DrawFormatString(700, 50, col_whi, "先攻後攻の決定\n");
	DrawFormatString(100, 20, col_whi, "下の箱の中に石が一つ置かれました。");
	DrawFormatString(100, 60, col_whi, "石の色はランダム.先攻は黒です");
	DrawFormatString(100, 100, col_whi, "上か下をで選んでね");
	DrawFormatString(250, 140, col_whi, "↑キー：上, \n↓キー：下");
	ScreenFlip();
	for (;;) {    //上下キーで画面が切り替わる
		int key = WaitKey();
		if (key == KEY_INPUT_UP) {
			player_col = ue;
			CPU_col = shita;
		}
		else if (key == KEY_INPUT_DOWN) {
			player_col = shita;
			CPU_col = ue;
		}
		break;
	}
	kakushiOpen();
	WaitTimer(800);
	if (player_col == KURO) {
		DrawFormatString(100, 60, col_whi, "あなたは黒");
		senkou = PLAYER;
		koukou = CPU;
	}
	else if(player_col==SHIRO){
		DrawFormatString(100, 60, col_whi, "あなたは白");
		senkou = CPU;
		koukou = PLAYER;
	}
	DrawFormatString(100, 100, col_whi, "先攻は黒");
	ScreenFlip();
	WaitTimer(2000);
	start();
	WaitTimer(100);
	///ゲーム本体///
	while (ProcessMessage() == 0)
	{
		ClearDrawScreen();
		SetDrawScreen(DX_SCREEN_BACK);
		DrawBox(0, 0, 680, 680, col_bann, TRUE);
		lattice();					//盤面の格子
		switch (turn % 2) {
		case 1:						//黒番
			MyCol = KURO;
			OppCol = SHIRO;
			break;
		case 0:						//白番
			MyCol = SHIRO;
			OppCol = KURO;
			break;
		}
		//盤面の表示//
		kuro_num = NumberofStone(KURO);				//白と黒の石の数を調べる
		shiro_num = NumberofStone(SHIRO);
		zero_num = NumberofStone(0);
		SetFontSize(70);							//右側の表記
		switch (turn % 2) {
		case 0:
			DrawFormatString(760, 60, col_whi, "白番");
			break;
		case 1:
			DrawFormatString(760, 60, col_whi, "黒番");
			break;
		}
		SetFontSize(30);
		DrawLine(700, 145, 980, 145, col_whi);
		if (senkou == PLAYER) {
			DrawFormatString(750, 280, col_whi, "黒：player");
			DrawFormatString(750, 320, col_whi, "白：CPU");
		}
		else if (senkou == CPU) {
			DrawFormatString(750, 280, col_whi, "黒：CPU");
			DrawFormatString(750, 320, col_whi, "白：player");
		}
		DrawFormatString(700, 400, col_whi, "石の数");
		DrawFormatString(720, 440, col_whi, "黒%8d石", kuro_num);
		DrawFormatString(720, 480, col_whi, "白%8d石", shiro_num);
		DrawFormatString(820, 160, col_whi, "%2d手目", turn);
		DrawBox(720, 565, 960, 650, col_pink, TRUE);				//ピンクのボックス
		DrawFormatString(740, 575, col_ao, "Pキーで置ける\n  場所を表示");
		//石の表示//
		for (y = 0; y < 8; y++) {
			for (x = 0; x < 8; x++) {
				if (field[y][x] == KURO)
					DrawGraph(x*LAT + MARGIN, y*LAT + MARGIN, Handle_kuro, TRUE);
				else if (field[y][x] == SHIRO)
					DrawGraph(x*LAT + MARGIN, y*LAT + MARGIN, Handle_shiro, TRUE);
			}
		}
		ScreenFlip();
		//盤面が全部埋まった、どちらかの石がなくなった場合は終了
		if (kuro_num == 0 || shiro_num == 0 || zero_num == 0)
			break;
		PutCanOrNot(MyCol, OppCol);				//MyCol(黒か白)の置ける場所を調べる
		for (y = 0; y < 8; y++) {				//置ける場所(OK)があるか調べる
			for (x = 0; x < 8; x++) {
				if (Myfield[y][x] == OK)
					O_num++;
			}
		}
		if (O_num == 0) {			//置ける場所が０個のとき
			SetFontSize(40);
			DrawBox(150, 240, 850, 440, col_pink, TRUE);				//ピンクのボックス
			DrawFormatString(200, 300, col_ao, "石を置ける場所がなくなりました\n\t\t相手のターンになります");
			ScreenFlip();
			for (;;) {
				if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)			//押されている
					break;
			}
			turn++, O_num = 0;											//初期化
			SetFontSize(30);
			continue;
		}
		if (player_col == MyCol) {										//playerの入力
			while (1) {
				//石を置ける場所の表示/非表示
				if (CheckHitKey(KEY_INPUT_P) == 0) {					//押されていない
					if (marka > 0)										//離した瞬間
						marka = -1;
					else												//押されていないとき
						marka = 0;
				}
				else
					marka++;
				if (marka == -1)
					mark = (mark + 1) % 2;
				if (mark == 1) {
					for (y = 0; y < 8; y++) {
						for (x = 0; x < 8; x++) {						//赤い丸で置ける場所を表示
							if (Myfield[y][x] == OK)
								DrawCircle(MARGIN + LAT * x + LAT / 2, MARGIN + LAT * y + LAT / 2, 20, col_put, TRUE);
						}
					}
				}
				ScreenFlip();
				if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {		//押されている
					fieldX = MousePosition(24);
					fieldY = MousePosition(25);
					if (Myfield[fieldY][fieldX] == OK)					//石を置ける場所だったらwhile文をぬける
						break;
				}
			}
		}
		if (CPU_col == MyCol) {																//CPU
			WaitTimer(1000);
			CornerAttack();
			if (CornerAttack() == 0) {}														//隅に置けた場合
			else {
				FileAttack_kai(MyCol, OppCol);
				if (FileAttack_kai(MyCol, OppCol) == 0) {}									//ヤスリ攻め成功
				else{																		//開放度理論
					do {
						for (y = 0; y < 8; y++) {											//すべてのマスで開放度を計算
							for (x = 0; x < 8; x++) {
								if (Myfield[y][x] != OK)									//石を置ける時だけ開放度を計算する
									continue;
								if ((board[y][x] == C || board[y][x] == X) && loop == 1)
									continue;
								openness[y][x] = DegreeofOpenness(x, y, MyCol, OppCol);
								if (openness[y][x] < least) {								//開放度の最小値を探す
									least = openness[y][x];									//最小値を保存
									leastX = x, leastY = y;									//x,y座標を保存
								}
							}
						}
						fieldX = leastX, fieldY = leastY;
						loop++;
					} while (OnBoard(fieldX, fieldY, 0, 0) == 1);
				}
			}
		}
		loop = 1;										//初期化
		field[fieldY][fieldX] = MyCol;					//選択した場所を裏返す
		WaitTimer(400);
		PlaySoundMem(Handle_sound, DX_PLAYTYPE_BACK);	//効果音
		switch (MyCol) {
		case KURO:
			DrawGraph(fieldX*LAT + MARGIN, fieldY*LAT + MARGIN, Handle_kuro, TRUE);
			break;
		case SHIRO:
			DrawGraph(fieldX*LAT + MARGIN, fieldY*LAT + MARGIN, Handle_shiro, TRUE);
			break;
		}
		WaitTimer(800);
		TurnOver(fieldX, fieldY, MyCol, OppCol);		//間を裏返す
		//Myfield[][] = field[][]、裏返したところを反映
		memcpy(Myfield, field, sizeof(Myfield));		//fieldをMyfieldに全体コピー
		for (y = 0; y < 8; y++) {						//opennessを初期化
			for (x = 0; x < 8; x++) {
				openness[y][x] = 60;
			}
		}
		least = 60;
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_L))						//確認用
			break;
		turn++, O_num = 0;									//初期化
	}
	DrawBox(150, 240, 540, 440, col_pink, TRUE);			//ピンクのボックス
	DrawFormatString(200, 300, col_ao, "　!!ゲーム終了!!　\nクリックで結果発表");
	ScreenFlip();
	for (;;) {
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)		//押されている
			break;
	}
	ClearDrawScreen();
	//ゲーム終了//
	kuro_num = NumberofStone(KURO);							//白と黒の石の数を調べる
	shiro_num = NumberofStone(SHIRO);
	zero_num = NumberofStone(0);
	if (kuro_num > shiro_num)								//多い方が勝ち
		winner = senkou;
	else if (kuro_num < shiro_num)
		winner = koukou;
	SetFontSize(100);
	//結果表示//
	DrawBox(200, 100, 800, 240, col_pink, TRUE);				//ピンクのボックス
	if (winner == PLAYER)
		DrawFormatString(290, 120, col_ao, "You Win.");
	else if (winner == CPU)
		DrawFormatString(280, 120, col_ao, "You Lose.");
	else if (kuro_num == shiro_num)
		DrawFormatString(370, 120, col_ao, "Draw.");
	SetFontSize(65);
	DrawBox(520, 310, 860, 415, col_pink, TRUE);				//ピンクのボックス
	DrawFormatString(540, 330, col_ao, "Try Again");
	SetFontSize(50);
	DrawFormatString(100, 300, col_whi, "石の数");
	DrawFormatString(150, 390, col_whi, "黒%4d石", kuro_num);
	DrawFormatString(150, 470, col_whi, "白%4d石", shiro_num);
	SetFontSize(30);
	ScreenFlip();
	for (;;) {
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)			//押されている
		break;
	}
	WaitKey();
	DxLib_End();
	return 0;
}