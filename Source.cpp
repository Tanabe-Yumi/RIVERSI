//////////////////////////////////////////////////////////////
//
//	<J2�v���O���~���O �ċx�݉ۑ�>
//
//		���o�[�V
//					j1819 �c�� �Д�
//
//		CPU�Ƃ̑ΐ�A��΂������
//		�E�����Ƃ�	�E���X���U��	�E������	�E�J���x���_
//
//				�ŏI�X�V��:2019/09/27
//
//////////////////////////////////////////////////////////////

#include<DxLib.h>
#include<stdio.h>
#include <string.h>			//memcpy()

#define MARGIN 20			//�Ֆʂ̊O���̗]��
#define LAT 80				//�P�i�q�̑傫��
#define KURO 3				//���̐�
#define SHIRO 4				//���̐�
#define PLAYER 80			//winner,senkou
#define CPU 67				//winner,senkou
#define OK 6				//�΂�u����ꏊ
#define C 61
#define X 62

int field[8][8] = {					//�Ֆ�
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,4,3,0,0,0},
	{0,0,0,3,4,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0} };
int Myfield[8][8] = {				//�u����ꏊ���L�^����
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,4,3,0,0,0},
	{0,0,0,3,4,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0} };
int openness[8][8] = {				//�J���x
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

int col_bann = GetColor(0, 169, 95);							//�Ղ̗�
int col_whi = GetColor(255, 255, 255);							//�����̔�
int col_bla = GetColor(0, 0, 0);								//��
int col_ao = GetColor(43, 164, 223);							//����
int col_aka = GetColor(252, 57, 41);							//�ԕ���
int col_kin = GetColor(246, 221, 131);							//���F
int col_pink = GetColor(249, 162, 164);							//�s���N�F
int col_gre = GetColor(84, 252, 96);							//�^�C�g���\���̃J���[
int col_put = GetColor(228, 208, 4);							//�u����ꏊ�̕\���F
int blightness = 225;											//�\���𖾂邳�̐ݒ�
int Handle_kuro, Handle_shiro;									//�΂̉摜�n���h��
int Handle_ukuro, Handle_ushiro;								//��������̉摜�n���h��
int Handle_walll, Handle_wallr, Handle_wallc, Hnadle_wallo;		//�B���̉摜�n���h��
int Handle_sound;												//���ʉ��̃n���h��
int al = -150, ar = -150, ac = -150, ao = -150;					//�B���̃A�j���[�V����
int player_col,CPU_col, ue, shita;								//������������
int kuro_num, shiro_num;										//���Ɣ��̐΂̐�
int zero_num, O_num = 0;										//�΂�u���ĂȂ��ꏊ�A�u����ꏊ�̐�
int turn = 1;													//����ڂ�
int senkou, koukou;												//PLAYER or CPU
int MyCol, OppCol;												//�����̐F�ƁA����̐F
int MX, MY;														//�}�E�X���W
int least = 60, leastX, leastY;									//�J���x���ŏ��̋؂ƁA����x,y���W
int fieldX, fieldY;												//���Ԃ��ꏊ��x,y���W
int winner;														//����
int x, y;														//field[y][x]
int v = 1, g = 3;												//�����Ɖ����x
int mark = 0, marka = 0;										//�΂�u����ꏊ�̕\��/��\��
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

void updowndecision() {										//��A��������
	if (GetRand(1) == 0) {
		ue = KURO;
		shita = SHIRO;
	}
	else {
		ue = SHIRO;
		shita = KURO;
	}
}

void kakushiClose() {										//�B���A�j���[�V����
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
		DrawBox(0, 0, 680, 680, col_bann, TRUE);			//�Ղ̗�
		DrawFormatString(700, 50, col_whi, "��U��U�̌���\n");
		DrawGraph(240, al, Handle_walll, TRUE);
		DrawGraph(240, ar, Handle_wallr, TRUE);
		DrawGraph(240, ac, Handle_wallc, TRUE);
		DrawGraph(240, ao, Hnadle_wallo, TRUE);
		ScreenFlip();
		if (ao == 240)
			break;
	}
}

void kakushiOpen() {										//�B������ɂ�����
	while (ProcessMessage() == 0) {
		SetDrawScreen(DX_SCREEN_BACK);
		DrawBox(0, 0, 680, 680, col_bann, TRUE);			//�Ղ̗�
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
	v = 1;								//������
}

int NumberofStone(int col) {			//�Ֆʂ̐΂̐��𐔂���
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

int OnBoard(int x, int y, int a, int b) {								//�n�����ꏊ���Տ�ɂ��邩�𒲂ׂ�
	if ((x + a) < 0 || (x + a) > 7 || (y + b) < 0 || (y + b) > 7)		//x����a�����Ay����b�����ɐi��ŁA�O�ɏo��
		return 1;
	return 0;	//���ɂ���
}

int LineResearch(int q,int p,int b,int a,int Col,int nCol) {			//�����̐΂܂ł̑���̐΂̐���Ԃ�
	int i;
	for (i = 1; field[q + i * b][p + i * a] == nCol; i++){}
	if (OnBoard(p, q, a*i, b * i) == 0 && field[q + i * b][p + i * a] == Col)
		return i - 1;
	else
		return 0;
}

int PutCanOrNot(int Col,int nCol) {										//�΂��u����ꏊ�̔���
	int x, y, a, b;
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {										//�����̏ꏊ
			if (Myfield[y][x] != KURO && Myfield[y][x] != SHIRO) {		//�΂����鎞�͕ς��Ȃ�
				for (b = -1; b <= 1; b++) {								//8����
					for (a = -1; a <= 1; a++) {
						if (a == 0 && b == 0)
							continue;
						if (LineResearch(y, x, b, a, Col, nCol) > 0)
							Myfield[y][x] = OK;							//OK�̎��u����
					}
				}
			}
		}
	}
	return 0;
}

int MousePosition(int a) {						//�J�[�\���́A�Ֆʂ̈ʒu��Ԃ�
	int MouseX, MouseY;
	GetMousePoint(&MouseX, &MouseY);
	if (a == 24)								//x���W��Ԃ�
		return (MouseX - MARGIN) / LAT;
	else if (a == 25)							//y���W��Ԃ�
		return (MouseY - MARGIN) / LAT;
}

void TurnOver(int fieldX, int fieldY, int Col, int nCol) {				//�Ԃ̐΂𗠕Ԃ�
	int number, a, b, i;
	for (b = -1; b <= 1; b++) {
		for (a = -1; a <= 1; a++) {										//8�����ƍ��̏ꏊ
			if (a == 0 && b == 0)										//���̏ꏊ�͏������Ȃ�
				continue;
			number = LineResearch(fieldY, fieldX, b, a, Col, nCol);		//�Ԃɂ��鑊��̐΂̐�
			for (i = 1; i <= number; i++) {								//�Ԃ̐΂̐�����
				field[fieldY + b * i][fieldX + a * i] = Col;			//���Ԃ�
			}
		}
	}
}

int CornerAttack() {									//�Ղ̋�����Ƃ�
	int i, j;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			if (Myfield[i * 7][j * 7] == OK) {			//�u����ꍇ
				fieldX = j * 7, fieldY = i * 7;
				return 0;
			}
		}
	}
	return 1;											//�u���Ȃ��ꍇ
}

int Direction(int num) {//���X���U�߂̕��������߂�
	switch (num) {
	case 0:				//�� or ��ɂ���
		return 1;		//�E or ��������Ԃ�
	case 7:				//�E or ���ɂ���
		return -1;		//�� or �������Ԃ�
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
				dir = Direction(j * 7);									//������
				for (k = 1; k < 7; k++) {								//�����̐F�̎��A����̐F�̎��͂��̗�
					if (Myfield[i * 7 + 0][j * 7 + k * dir] == Col|| Myfield[i * 7 + 0][j * 7 + k * dir] == nCol)
						continue;
					if (Myfield[i * 7 + 0][j * 7 + k * dir] == OK) {	//�u����
						fieldX = j * 7 + k * dir, fieldY = i * 7;
						return 0;										//����
					}
					else
						break;											//�ׂɍs���Ȃ�
				}
			}
		}
	}
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			if (Myfield[i * 7][j * 7] == Col) {
				dir = Direction(i * 7);									//�㉺����
				for (k = 1; k < 7; k++) {
					if (Myfield[i * 7 + k * dir][j * 7 + 0] == Col || Myfield[i * 7 + k * dir][j * 7 + 0] == nCol)
						continue;
					if (Myfield[i * 7 + k * dir][j * 7 + 0] == OK) {
						fieldX = j * 7, fieldY = i * 7 + k * dir;
						return 0;										//����
					}
					else
						break;
				}
			}
		}
	}
	return 1;															//���X���U�߂ł��Ȃ��ꍇ
}

int DegreeofOpenness(int fieldX, int fieldY, int Col, int nCol) {					//�J���x���_
	int number = 0, a, b, c, d, i, count;
	for (b = -1; b <= 1; b++) {														//�ǂ̕����ɉ����Ԃ��邩
		for (a = -1; a <= 1; a++) {													//8�����ƍ��̏ꏊ
			if (a == 0 && b == 0)													//���̏ꏊ�͏������Ȃ�
				continue;
			count = LineResearch(fieldY, fieldX, b, a, Col, nCol);					//�Ԃɂ��鑊��̐΂̐�
			for (i = 1; i <= count; i++) {											//���Ԃ����e�΂ɂ��āA�J���x�𐔂���
				for (d = -1; d <= 1; d++) {
					for (c = -1; c <= 1; c++) {										//8�����ŁA�󂢂Ă���ꏊ�̐�
						if (c == 0 && d == 0)
							continue;
						if (field[fieldY + b * i + d][fieldX + a * i + c] == 0)		//�΂��u���Ă��Ȃ���
							number++;
						/* a*i,b*i�Ői�ޕ������`���Ac,d�Ŋm�F����������`���Ă��� */
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
	ChangeWindowMode(TRUE);							//��S��ʂɃZ�b�g
	SetGraphMode(1000, 680, 32);					//��ʃT�C�Y�w��
	SetOutApplicationLogValidFlag(FALSE);			//Log.txt�𐶐����Ȃ��悤�ɐݒ�
	if (DxLib_Init() == 1) { return -1; }			//�������Ɏ��s���ɃG���[��f�����ďI��
	SetMouseDispFlag(TRUE);							// �}�E�X��\����Ԃɂ���
	//�摜�̓ǂݍ���//
	Handle_kuro = LoadGraph("black.PNG");
	Handle_shiro = LoadGraph("white.PNG");
	Handle_ukuro = LoadGraph("black_up.PNG");
	Handle_ushiro = LoadGraph("white_up.PNG");
	Handle_walll = LoadGraph("wall_left2.PNG");
	Handle_wallr = LoadGraph("wall_right2.PNG");
	Handle_wallc = LoadGraph("wall_center2.PNG");
	Hnadle_wallo = LoadGraph("wall_up2.PNG");
	Handle_sound = LoadSoundMem("sound1.mp3");

	ClearDrawScreen();												//����ʏ���
	SetDrawScreen(DX_SCREEN_BACK);									//�`���𗠉�ʂ�
	ChangeFont("Snap ITC");
	SetFontSize(180);
	DrawFormatString(0, 260, col_gre, "REVERSI");					//�^�C�g���\��
	ScreenFlip();
	WaitTimer(3000);
	for (int i = 0; i < 45; i++) {									//�Â��Ȃ��ď����Ă���
		ClearDrawScreen();
		blightness -= 5;
		SetDrawBright(blightness, blightness, blightness);
		DrawFormatString(0, 260, col_gre, "REVERSI");
		ScreenFlip();
	}
	SetDrawBright(225, 225, 225);									//���邳��߂�
	ChangeFont("Arial-ltalicMT");									//�t�H���g�ݒ�
	SetFontSize(30);
	WaitTimer(1000);
	//��U�̌���//
	ClearDrawScreen();
	SetDrawScreen(DX_SCREEN_BACK);
	DrawBox(0, 0, 680, 680, col_bann, TRUE);						//�Ղ̗�
	ScreenFlip();
	WaitTimer(800);
	DrawFormatString(700, 50, col_whi, "��U��U�̌���\n");
	ScreenFlip();
	updowndecision();
	WaitTimer(1800);
	kakushiClose();
	WaitTimer(880);
	SetDrawScreen(DX_SCREEN_BACK);
	DrawFormatString(700, 50, col_whi, "��U��U�̌���\n");
	DrawFormatString(100, 20, col_whi, "���̔��̒��ɐ΂���u����܂����B");
	DrawFormatString(100, 60, col_whi, "�΂̐F�̓����_��.��U�͍��ł�");
	DrawFormatString(100, 100, col_whi, "�ォ�����őI��ł�");
	DrawFormatString(250, 140, col_whi, "���L�[�F��, \n���L�[�F��");
	ScreenFlip();
	for (;;) {    //�㉺�L�[�ŉ�ʂ��؂�ւ��
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
		DrawFormatString(100, 60, col_whi, "���Ȃ��͍�");
		senkou = PLAYER;
		koukou = CPU;
	}
	else if(player_col==SHIRO){
		DrawFormatString(100, 60, col_whi, "���Ȃ��͔�");
		senkou = CPU;
		koukou = PLAYER;
	}
	DrawFormatString(100, 100, col_whi, "��U�͍�");
	ScreenFlip();
	WaitTimer(2000);
	start();
	WaitTimer(100);
	///�Q�[���{��///
	while (ProcessMessage() == 0)
	{
		ClearDrawScreen();
		SetDrawScreen(DX_SCREEN_BACK);
		DrawBox(0, 0, 680, 680, col_bann, TRUE);
		lattice();					//�Ֆʂ̊i�q
		switch (turn % 2) {
		case 1:						//����
			MyCol = KURO;
			OppCol = SHIRO;
			break;
		case 0:						//����
			MyCol = SHIRO;
			OppCol = KURO;
			break;
		}
		//�Ֆʂ̕\��//
		kuro_num = NumberofStone(KURO);				//���ƍ��̐΂̐��𒲂ׂ�
		shiro_num = NumberofStone(SHIRO);
		zero_num = NumberofStone(0);
		SetFontSize(70);							//�E���̕\�L
		switch (turn % 2) {
		case 0:
			DrawFormatString(760, 60, col_whi, "����");
			break;
		case 1:
			DrawFormatString(760, 60, col_whi, "����");
			break;
		}
		SetFontSize(30);
		DrawLine(700, 145, 980, 145, col_whi);
		if (senkou == PLAYER) {
			DrawFormatString(750, 280, col_whi, "���Fplayer");
			DrawFormatString(750, 320, col_whi, "���FCPU");
		}
		else if (senkou == CPU) {
			DrawFormatString(750, 280, col_whi, "���FCPU");
			DrawFormatString(750, 320, col_whi, "���Fplayer");
		}
		DrawFormatString(700, 400, col_whi, "�΂̐�");
		DrawFormatString(720, 440, col_whi, "��%8d��", kuro_num);
		DrawFormatString(720, 480, col_whi, "��%8d��", shiro_num);
		DrawFormatString(820, 160, col_whi, "%2d���", turn);
		DrawBox(720, 565, 960, 650, col_pink, TRUE);				//�s���N�̃{�b�N�X
		DrawFormatString(740, 575, col_ao, "P�L�[�Œu����\n  �ꏊ��\��");
		//�΂̕\��//
		for (y = 0; y < 8; y++) {
			for (x = 0; x < 8; x++) {
				if (field[y][x] == KURO)
					DrawGraph(x*LAT + MARGIN, y*LAT + MARGIN, Handle_kuro, TRUE);
				else if (field[y][x] == SHIRO)
					DrawGraph(x*LAT + MARGIN, y*LAT + MARGIN, Handle_shiro, TRUE);
			}
		}
		ScreenFlip();
		//�Ֆʂ��S�����܂����A�ǂ��炩�̐΂��Ȃ��Ȃ����ꍇ�͏I��
		if (kuro_num == 0 || shiro_num == 0 || zero_num == 0)
			break;
		PutCanOrNot(MyCol, OppCol);				//MyCol(������)�̒u����ꏊ�𒲂ׂ�
		for (y = 0; y < 8; y++) {				//�u����ꏊ(OK)�����邩���ׂ�
			for (x = 0; x < 8; x++) {
				if (Myfield[y][x] == OK)
					O_num++;
			}
		}
		if (O_num == 0) {			//�u����ꏊ���O�̂Ƃ�
			SetFontSize(40);
			DrawBox(150, 240, 850, 440, col_pink, TRUE);				//�s���N�̃{�b�N�X
			DrawFormatString(200, 300, col_ao, "�΂�u����ꏊ���Ȃ��Ȃ�܂���\n\t\t����̃^�[���ɂȂ�܂�");
			ScreenFlip();
			for (;;) {
				if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)			//������Ă���
					break;
			}
			turn++, O_num = 0;											//������
			SetFontSize(30);
			continue;
		}
		if (player_col == MyCol) {										//player�̓���
			while (1) {
				//�΂�u����ꏊ�̕\��/��\��
				if (CheckHitKey(KEY_INPUT_P) == 0) {					//������Ă��Ȃ�
					if (marka > 0)										//�������u��
						marka = -1;
					else												//������Ă��Ȃ��Ƃ�
						marka = 0;
				}
				else
					marka++;
				if (marka == -1)
					mark = (mark + 1) % 2;
				if (mark == 1) {
					for (y = 0; y < 8; y++) {
						for (x = 0; x < 8; x++) {						//�Ԃ��ۂŒu����ꏊ��\��
							if (Myfield[y][x] == OK)
								DrawCircle(MARGIN + LAT * x + LAT / 2, MARGIN + LAT * y + LAT / 2, 20, col_put, TRUE);
						}
					}
				}
				ScreenFlip();
				if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) {		//������Ă���
					fieldX = MousePosition(24);
					fieldY = MousePosition(25);
					if (Myfield[fieldY][fieldX] == OK)					//�΂�u����ꏊ��������while�����ʂ���
						break;
				}
			}
		}
		if (CPU_col == MyCol) {																//CPU
			WaitTimer(1000);
			CornerAttack();
			if (CornerAttack() == 0) {}														//���ɒu�����ꍇ
			else {
				FileAttack_kai(MyCol, OppCol);
				if (FileAttack_kai(MyCol, OppCol) == 0) {}									//���X���U�ߐ���
				else{																		//�J���x���_
					do {
						for (y = 0; y < 8; y++) {											//���ׂẴ}�X�ŊJ���x���v�Z
							for (x = 0; x < 8; x++) {
								if (Myfield[y][x] != OK)									//�΂�u���鎞�����J���x���v�Z����
									continue;
								if ((board[y][x] == C || board[y][x] == X) && loop == 1)
									continue;
								openness[y][x] = DegreeofOpenness(x, y, MyCol, OppCol);
								if (openness[y][x] < least) {								//�J���x�̍ŏ��l��T��
									least = openness[y][x];									//�ŏ��l��ۑ�
									leastX = x, leastY = y;									//x,y���W��ۑ�
								}
							}
						}
						fieldX = leastX, fieldY = leastY;
						loop++;
					} while (OnBoard(fieldX, fieldY, 0, 0) == 1);
				}
			}
		}
		loop = 1;										//������
		field[fieldY][fieldX] = MyCol;					//�I�������ꏊ�𗠕Ԃ�
		WaitTimer(400);
		PlaySoundMem(Handle_sound, DX_PLAYTYPE_BACK);	//���ʉ�
		switch (MyCol) {
		case KURO:
			DrawGraph(fieldX*LAT + MARGIN, fieldY*LAT + MARGIN, Handle_kuro, TRUE);
			break;
		case SHIRO:
			DrawGraph(fieldX*LAT + MARGIN, fieldY*LAT + MARGIN, Handle_shiro, TRUE);
			break;
		}
		WaitTimer(800);
		TurnOver(fieldX, fieldY, MyCol, OppCol);		//�Ԃ𗠕Ԃ�
		//Myfield[][] = field[][]�A���Ԃ����Ƃ���𔽉f
		memcpy(Myfield, field, sizeof(Myfield));		//field��Myfield�ɑS�̃R�s�[
		for (y = 0; y < 8; y++) {						//openness��������
			for (x = 0; x < 8; x++) {
				openness[y][x] = 60;
			}
		}
		least = 60;
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_L))						//�m�F�p
			break;
		turn++, O_num = 0;									//������
	}
	DrawBox(150, 240, 540, 440, col_pink, TRUE);			//�s���N�̃{�b�N�X
	DrawFormatString(200, 300, col_ao, "�@!!�Q�[���I��!!�@\n�N���b�N�Ō��ʔ��\");
	ScreenFlip();
	for (;;) {
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)		//������Ă���
			break;
	}
	ClearDrawScreen();
	//�Q�[���I��//
	kuro_num = NumberofStone(KURO);							//���ƍ��̐΂̐��𒲂ׂ�
	shiro_num = NumberofStone(SHIRO);
	zero_num = NumberofStone(0);
	if (kuro_num > shiro_num)								//������������
		winner = senkou;
	else if (kuro_num < shiro_num)
		winner = koukou;
	SetFontSize(100);
	//���ʕ\��//
	DrawBox(200, 100, 800, 240, col_pink, TRUE);				//�s���N�̃{�b�N�X
	if (winner == PLAYER)
		DrawFormatString(290, 120, col_ao, "You Win.");
	else if (winner == CPU)
		DrawFormatString(280, 120, col_ao, "You Lose.");
	else if (kuro_num == shiro_num)
		DrawFormatString(370, 120, col_ao, "Draw.");
	SetFontSize(65);
	DrawBox(520, 310, 860, 415, col_pink, TRUE);				//�s���N�̃{�b�N�X
	DrawFormatString(540, 330, col_ao, "Try Again");
	SetFontSize(50);
	DrawFormatString(100, 300, col_whi, "�΂̐�");
	DrawFormatString(150, 390, col_whi, "��%4d��", kuro_num);
	DrawFormatString(150, 470, col_whi, "��%4d��", shiro_num);
	SetFontSize(30);
	ScreenFlip();
	for (;;) {
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)			//������Ă���
		break;
	}
	WaitKey();
	DxLib_End();
	return 0;
}