#include <opencv2/opencv.hpp>
#include<stdlib.h>
#include <math.h>
using namespace cv;
int pieces_x, pieces_y, board_x, board_y;
double distance;

void getSRC()
{
	system("adb shell /system/bin/screencap -p /sdcard/screenshot.png");
	cvWaitKey(2000);
	printf("��ͼ�ɹ�...\n");
	system("adb pull /sdcard/screenshot.png E:/opencv/1.png");
	cvWaitKey(500);
	printf("�����ļ�������...\n");
}

void adb_move(double distance)
{
	int d = int(distance * 1.38);
	printf("t = %d ms\n",d);
	char cmd[500] = {0};
	int randx = rand() % 200 + 200;
	int randy = rand() % 200 + 1300;
	sprintf(cmd, "adb shell input swipe %d %d %d %d %d", randx, randy , randx, randy, d);
	system((const char *)cmd);
	cvWaitKey(3000);
}

void get_pieces_xy(Mat& src)
{
	int h = src.rows;
	int w = src.cols;
	int flag = 0;
	//���ĵ�����
	int ret_x = 0;
	int ret_y = 0;
	//ǰ��ǵ�
	int qian_x = 0;
	int qian_y = 0;
	//���ǵ�
	int hou_x = 0;
	int hou_y = 0;
	printf("start = %d , end = %d\n", h / 3, h * 2 / 3);
	for (int row = h / 3; row < (h * 4 / 5); row++)
	{
		flag = 0;
		for (int col = 0; col < w; col++)
		{
			int temp_b = src.at<Vec3b>(row, col)[0];
			int temp_g = src.at<Vec3b>(row, col)[1];
			int temp_r = src.at<Vec3b>(row, col)[2];
			if ( (temp_r>50&&temp_r<60) && (temp_g>53 && temp_g<63) && (temp_b>95 && temp_b<110) && flag == 0 )
			{
				qian_x = col;
				qian_y = row;
				flag = 1;
			}
			if ((temp_r > 50 && temp_r < 60) && (temp_g > 53 && temp_g < 63) && (temp_b > 95 && temp_b < 110) && flag == 1)
			{
				hou_x = col;
				hou_y = row;
			}
		}
	}
	ret_x = (qian_x + hou_x) / 2;
	ret_y = (qian_y + hou_y) / 2;
	pieces_x = ret_x;
	pieces_y = ret_y;
	printf("pieces_x = %d , pieces_y = %d\n", ret_x, ret_y);
}
void get_board_xy(Mat& src)
{
	//������ĵ�����
	int ret_x = 0;
	int ret_y = 0;
	//�����ǵ�
	int ding_x = 0;
	int ding_y = 0;
	//��Ե��ǵ�
	int bian_x = 0;
	int bian_y = 0;
	//��ʼ����
	int h = src.rows;
	int w = src.cols;
	int bc_b = src.at<Vec3b>(h / 3, 0)[0];
	int bc_g = src.at<Vec3b>(h / 3, 0)[1];
	int bc_r = src.at<Vec3b>(h / 3, 0)[2];
	int mark_b = 0;
	int mark_g = 0;
	int mark_r = 0;
	int flag = 0;
	for (int row = h / 3; row < (h * 2 / 3); row++)
	{
		if (bian_x != 0 && bian_y != 0)
		{
			break;
		}
		for (int col = 0; col < w; col++)
		{
			int temp_b = src.at<Vec3b>(row, col)[0];
			int temp_g = src.at<Vec3b>(row, col)[1];
			int temp_r = src.at<Vec3b>(row, col)[2];
			if ((temp_r > 50 && temp_r < 60) && (temp_g > 53 && temp_g < 63) && (temp_b > 95 && temp_b < 110))
			{
				continue;
			}
			if ((abs(temp_b-bc_b)>30 || abs(temp_g - bc_g)>30 || abs(temp_r - bc_r)>30)&& flag == 0)
			{
				mark_b = temp_b;
				mark_g = temp_g;
				mark_r = temp_r;
				flag = 1;
				ding_x = col;
				ding_y = row;
				continue;
			}
			if (abs(temp_b - mark_b)<30 && abs(temp_g - mark_g)<30 && abs(temp_r - mark_r)<30 && flag == 1)
			{
				break;
			}
			if (abs(temp_b - bc_b)<30 && abs(temp_g - bc_g)<30 && abs(temp_r - bc_r)<30)
			{
				continue;
			}
			if (
				(abs(temp_b - bc_b)>30 || abs(temp_g - bc_g)>30 || abs(temp_r - bc_r)>30)
				&&
				(abs(temp_b - mark_b)>30 || abs(temp_g - mark_g)>30 || abs(temp_r - mark_r)>30)
				)
			{
				bian_x = col;
				bian_y = row;
				break;
			}
		}
	}
	ret_x = ding_x;
	ret_y = bian_y;
	board_x = ret_x;
	board_y = ret_y;
	printf("board_x = %d, board_y = %d\n", ret_x,ret_y);
}

int main(int argc, char** argv)
{
	namedWindow("test");
	while (1)
	{
		//adb��ͼ
		getSRC();
		//��ͼ����
		printf("��ʼͼ�����...\n");
		Mat src = imread("E:/opencv/1.png");
		if (!src.data) {
			printf("SRCFile open failed...\n");
			return -1;
		}
		get_board_xy(src);
		get_pieces_xy(src);
		distance = sqrt(abs(pieces_x - board_x)*abs(pieces_x - board_x) + abs(pieces_y - board_y)*abs(pieces_y - board_y));
		printf("d = %d\n", (int)distance);
		//չʾ����Ŀ���
		ellipse(src, Point(board_x, board_y), Size(10, 10), 90, 0, 360, Scalar(255, 0, 0), 2, LINE_8);
		ellipse(src, Point(pieces_x, pieces_y), Size(10, 10), 90, 0, 360, Scalar(255, 0, 0), 2, LINE_8);
		CvSize sz;
		double scale = 0.2;
		sz.width = src.cols*scale;
		sz.height = src.rows*scale;
		Mat desc(sz,src.type());
		resize(src, desc, sz, 0, 0);
		imshow("test", desc);
		//adb����
		adb_move(distance);
		printf("�����ͼ�ȴ�ָ��...\n");
		char temp = getchar();
	}
	return 0;
}
