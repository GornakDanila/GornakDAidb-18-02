#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}



void Vipuklost1();
void Vipuklost3();

void normal(double a[3], double b[3], double c[3], double n[])
{
	double x1, x2, y1, y2, z1, z2, nx, ny, nz;
	x1 = a[0] - b[0];
	y1 = a[1] - b[1];
	z1 = a[2] - b[2];

	x2 = c[0] - b[0];
	y2 = c[1] - b[1];
	z2 = c[2] - b[2];

	n[0] = y1 * z2 - y2 * z1;
	n[1] = -x1 * z2 + x2 * z1;
	n[2] = x1 * y2 - x2 * y1;

}

void Render(OpenGL *ogl)
{

	
	

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


   //��������� ������ ������

	glBindTexture(GL_TEXTURE_2D, texId);
	double A[] = { 0,0,0 };
	double A1[] = { 3,3,0 };
	double A2[] = { 3,13,0 };
	double A3[] = { 7,9,0 };
	double A4[] = { 5,5,0 };
	double A5[] = { 9,1,0 };
	double n[] = { 0,0,0 };
	


	glBegin(GL_TRIANGLES);

	glNormal3d(0, 0, -1);
	glColor3d(1, 0, 1);
	glVertex3dv(A1);
	glVertex3dv(A4);
	glVertex3dv(A2);

	glNormal3d(0, 0, -1);
	glColor3d(1, 1, 0);
	glVertex3dv(A2);
	glVertex3dv(A4);
	glVertex3dv(A3);


	glEnd();

	double B[] = { 0,0,2 };
	double B1[] = { 3,3,2 };
	double B2[] = { 3,13,2 };
	double B3[] = { 7,9,2 };
	double B4[] = { 5,5,2 };
	double B5[] = { 9,1,2 };


	//glBegin(GL_TRIANGLES);

	//glNormal3d(0, 0, 1);
	//glColor3d(1, 0, 1);
	//glVertex3dv(B1);
	//glVertex3dv(B4);
	//glVertex3dv(B2);

	//glNormal3d(0, 0, 1);
	//glColor3d(1, 0, 0);
	//glVertex3dv(B2);
	//glVertex3dv(B4);
	//glVertex3dv(B3);
	//glEnd();

	glBegin(GL_QUADS);

	normal(A, B, B1,n);
	glColor3d(0, 0, 0);
	n[0] *= -1;n[1] *= -1;n[2] *= -1;
	glNormal3d(n[0], n[1], n[2]);

	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(B1);
	glVertex3dv(A1);

	normal(A1, B1, B2, n);
	glColor3d(0, 0, 0);
	n[0] *= -1;n[1] *= -1;n[2] *= -1;
	glNormal3d(n[0], n[1], n[2]);
	glTexCoord2d(0, 1);glVertex3dv(A1);
	glTexCoord2d(0, 0);	glVertex3dv(B1);
	glTexCoord2d(1, 0);glVertex3dv(B2);
	glTexCoord2d(1, 1);glVertex3dv(A2);


	normal(A3, B3, B4, n);
	glColor3d(0, 0, 0);
	n[0] *= -1;n[1] *= -1;n[2] *= -1;
	glNormal3d(n[0], n[1], n[2]);
	glVertex3dv(A3);
	glVertex3dv(B3);
	glVertex3dv(B4);
	glVertex3dv(A4);

	normal(A4, B4, B5, n);
	glColor3d(0, 0, 0);
	n[0] *= -1;n[1] *= -1;n[2] *= -1;
	glNormal3d(n[0], n[1], n[2]);
	glVertex3dv(A4);
	glVertex3dv(B4);
	glVertex3dv(B5);
	glVertex3dv(A5);


	glEnd();

	for (double i = 0; i <= 180; i += 0.5)
	{

		glPushMatrix();
		glTranslated(5, 11, 0);
		glRotated(-i, 0, 0, 1);
		Vipuklost1(); //������� ��������� ������ � ��������� �������
		Vipuklost3(); //������� ��������� ������ � ��������� �������
		glPopMatrix();
	}

	double x1 = 0, x2 = 0.125, y3, y2, y1 = 0, a[3], b[3], c[3];
	for (double i = 1; i <= 72; i++)
	{
		y2 = sqrt(7.73 * 7.73 - (x2 - 5.19231) * (x2 - 5.19231)) - 5.73077;

		a[0] = x1; a[1] = y1; a[2] = 2;
		b[0] = x2; b[1] = y2; b[2] = 2;
		c[0] = x2; c[1] = y2; c[2] = 0;
		glBegin(GL_QUADS);
		
		normal(a, b, c, n);
		glNormal3d(n[0], n[1], n[2]);
		glColor3d(0.5, 0, 0.5);

		glVertex3d(x1, y1, 2);
		glVertex3d(x2, y2, 2);
		glVertex3d(x2, y2, 0);
		glVertex3d(x1, y1, 0);

		glEnd();

		glBegin(GL_TRIANGLES);

	

		
		glColor3d(1, 0, 0);
		glNormal3d(0, 0, 1);
		glVertex3d(x1, y1, 0);
		glVertex3d(x2, y2, 0);
		glVertex3d(5, 5, 0);
		glEnd();

		x1 = x2; x2 += 0.125; y1 = y2;
	}

	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.2, 0.4, 0.3, 0.9);

	glBegin(GL_TRIANGLES);

	glNormal3d(0, 0, 1);

	glVertex3dv(B1);
	glVertex3dv(B4);
	glVertex3dv(B2);

	glNormal3d(0, 0, 1);
	
	glVertex3dv(B2);
	glVertex3dv(B4);
	glVertex3dv(B3);
	glEnd();

	x1 = 0; x2 = 0.125; y1 = 0;
	for (double i = 1; i <= 72; i++)
	{
		y2 = sqrt(7.73 * 7.73 - (x2 - 5.19231) * (x2 - 5.19231)) - 5.73077;

		

		glBegin(GL_TRIANGLES);



		
		glNormal3d(0, 0, 1);
		glVertex3d(x1, y1, 2);
		glVertex3d(x2, y2, 2);
		glVertex3d(5, 5, 2);
		glEnd();

		x1 = x2; x2 += 0.125; y1 = y2;
	}


	for (double i = 0; i <= 180; i += 0.5)
	{

		glPushMatrix();
		glTranslated(5, 11, 0);
		glRotated(-i, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glNormal3d(0, 0, 1);
		glVertex3d(-2, 2, 2);
		glVertex3d(0, 0, 2);
		glVertex3d(-2.01737, 1.98247, 2);
		glEnd();
		glPopMatrix();
	}


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}


void Vipuklost1()
{
	glBegin(GL_TRIANGLES);

	glNormal3d(0, 0, -1);
	glColor3d(1, 0, 0);
	glVertex3d(-2, 2, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(-2.01737, 1.98247, 0);


	glEnd();

}



void Vipuklost3()
{
	double a[3], b[3], c[3], n[3];
	a[0] = -2; a[1] = 2; a[2] = 2;
	b[0] = -2; b[1] = 2; b[2] = 0;
	c[0] = -2.01737; c[1] =1.98247; c[2] = 0;
	glBegin(GL_QUADS);

	normal(a, b, c, n);
	n[0] *= -1;n[1] *= -1;n[2] *= -1;
	glNormal3d(n[0], n[1], n[2]);
	glColor3d(1, 0, 1);
	glVertex3d(-2, 2, 2);
	glVertex3d(-2, 2, 0);
	glVertex3d(-2.01737, 1.98247, 0);
	glVertex3d(-2.01737, 1.98247, 2);

	glEnd();

}