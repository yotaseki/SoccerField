#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
//#include <cstdlib>
#include <cmath>
#include <sstream>
#include <cassert>
#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <random>
#include <boost/filesystem.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
/*
#define PRINT_MAT(X) cout << #X << ":\n" << X << endl << endl
#define PRINT_MAT2(X,DESC) cout << DESC << ":\n" << X << endl << endl
#define PRINT_FNC    cout << "[" << __func__ << "]" << endl
*/
using namespace std;

int field_width  = 9000;
int field_height = 6000;
const int windowWidth  = 1280;
const int windowHeight = 960;
GLuint g_texID[3];
int mouse_flag=1;
int save_flag=0;
int random_light=0;
int tex_id=0;
struct Point3D{
	double x;
	double y;
	double z;
};
struct CameraAngle{
	double tilt;
	double pan;
	double height;
}cang = {M_PI*47/180, 0.0, 0.1};
Point3D cam = {0.0,0.0,cang.height};
Point3D obj = {0.0,cang.height/tan(cang.tilt),0.0};

int file_count_boost(const boost::filesystem::path& root) {
	namespace fs = boost::filesystem;
	if (!fs::exists(root) || !fs::is_directory(root)) return 0;

	std::size_t result = 0;
	fs::directory_iterator last;
	for (fs::directory_iterator pos(root); pos != last; ++pos) {
		++result;
		if (fs::is_directory(*pos)) result += file_count_boost(pos->path());
	}

	return (int)result;
}

std::string getFileName(std::string path)
{
	int cnt = file_count_boost(path);
	std::ostringstream ostr;
	ostr << path <<"/field"<<std::setfill('0') << std::setw(5) << cnt << ".png";
	cout << ostr.str() << endl;
	return ostr.str();
}

void Affine(Point3D &src, Point3D &dst, const Eigen::Matrix4d &H)
{
	//cout << "Affine" << endl;
	Eigen::Vector4d tmp = Eigen::Vector4d::Ones(4); 
	Eigen::Vector4d p(4); 
	p<< src.x, src.y, src.z, 1;
	//PRINT_MAT(p);
	tmp = H * p;
	dst.x = tmp(0);
	dst.y = tmp(1);
	dst.z = tmp(2);
	/*
	   for(int r=0;r<4;r++)
	   {
	   for(int c=0;c<4;c++)
	   {
	   cout << "[" << H[r][c]<<"\t";
	   }
	   cout << "]\t[" << p[r]<< "\t]";
	   cout << "\t[" << tmp[r]<< "\t]" << endl;
	   }
	   cout << "x"<< dst.x <<endl;
	   cout << "y"<< dst.y <<endl;
	   cout << "z"<< dst.z <<endl;
	   */
}

void saveImage(std::string fn, GLFWwindow *window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	int type = CV_8UC4;
	int format = GL_BGRA_EXT;
	glReadBuffer(GL_FRONT);
	cv::Mat out_img(cv::Size(width, height), type);
	glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, out_img.data);
	cv::flip(out_img, out_img, 0);
    cv::resize(out_img,out_img,cv::Size(windowWidth,windowHeight));
	cv::imwrite(fn,out_img);
}

double random_mt()
{
	std::random_device rnd;
	std::mt19937 mt(rnd());
	return (double)mt() / 0xffffffff;
}

void resetPosition()
{
	cang.tilt = M_PI/6;
	cang.pan = 0.0;
	cang.height = 0.1;
	cam.x = 0.0;
	cam.y = 0.0;
	cam.z = cang.height;
	obj.x = 0.0;
	obj.y = cang.height/tan(cang.tilt);
	obj.z = 0.0;
}

void setRandomPosition()
{
	resetPosition();

	cang.pan = (2*M_PI) * random_mt();
	Eigen::Matrix4d I4 = Eigen::Matrix4d::Identity(4,4);
	I4(0,0) = cos(cang.pan);	I4(0,1) =-sin(cang.pan);
	I4(1,0) = sin(cang.pan);	I4(1,1) = cos(cang.pan);
	Affine(obj,obj,I4);

	cam.x = 0.7 - 1.4*random_mt();
	cam.y = 0.4 - 0.8*random_mt();
	obj.x += cam.x;
	obj.y += cam.y;
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, (double)w / (double)h, 0.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam.x,cam.y,cam.z,obj.x,obj.y,obj.z,0.0,0.0,1.0);
}

void display(GLFWwindow *window)
{
	double w = (double)field_width / 10e+03;
	double h = (double)field_height / 10e+03;
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(cam.x,cam.y,cam.z,obj.x,obj.y,obj.z,0.0,0.0,1.0);
	glBindTexture(GL_TEXTURE_2D, g_texID[tex_id]);
	glEnable(GL_TEXTURE_2D);
	if(tex_id < 2 && random_light==1)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	glBegin(GL_POLYGON);
	glTexCoord2d(0 , 0); glVertex2d(-w , h);
	glTexCoord2d(0 , 1); glVertex2d(-w ,-h);
	glTexCoord2d(1 , 1); glVertex2d( w ,-h);
	glTexCoord2d(1 , 0); glVertex2d( w , h);
	glEnd();
    glFlush();
	if(tex_id < 2 && random_light==1)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	glDisable(GL_TEXTURE_2D);
	/*
	   cout << " --- " << endl;
	   cout << "\tcam.x " << cam.x << "\tobj.x " << obj.x<< endl;
	   cout << "\tcam.y " << cam.y << "\tobj.y " << obj.y<< endl;
	   cout << "\tcam.z " << cam.z << "\tobj.z " << obj.z<< endl;
	   cout << "tilt:" << cang.tilt << "\tpan" << cang.pan << endl;
	   */
}

void init_light(float *color)
{
    float light[ ] = {0.0,0.0,0.1,0.0};
    glLightfv(GL_LIGHT0,GL_POSITION,light);
	float direction[] = {0.0, 0.0, -1.0};
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,direction);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,color);
}

void init()
{
	if(tex_id==0)
		glClearColor(1.0, 1.0, 1.0, 0.0);
	else
		glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0,1.0,1.0);
}

void setupTexture( GLuint &texID,std::string file)
{
	cv::Mat image = cv::imread(file);
	if (image.empty()) assert(!"OpenCV load image error");
	cv::flip(image,image,0);
	cv::cvtColor(image,image,CV_BGRA2RGBA);
	//cv::imshow("img",image);
	//cv::waitKey(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.cols,image.rows, GL_RGBA,GL_UNSIGNED_BYTE,image.data); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void error_callback(int eror, const char* description)
{
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	double step = 0.01;
	double d ;
	Eigen::Matrix4d I4 = Eigen::Matrix4d::Identity(4,4);
	glLoadIdentity();
	//printf("%d\n",key);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_W && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))
	{
		d = cang.pan;
		I4(0,3)+=sin(d)*step;
		I4(1,3)+=cos(d)*step;
		Affine(cam,cam,I4);
		Affine(obj,obj,I4);
	}
	else if (key == GLFW_KEY_S && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))
	{
		d = cang.pan + M_PI;
		I4(0,3)+=sin(d)*step;
		I4(1,3)+=cos(d)*step;
		Affine(cam,cam,I4);
		Affine(obj,obj,I4);
	}
	else if (key == GLFW_KEY_D && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))	
	{
		d = cang.pan + M_PI/2;
		I4(0,3)+=sin(d)*step;
		I4(1,3)+=cos(d)*step;
		Affine(cam,cam,I4);
		Affine(obj,obj,I4);
	}
	else if (key == GLFW_KEY_A && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))	
	{
		d = cang.pan - M_PI/2;
		I4(0,3)+=sin(d)*step;
		I4(1,3)+=cos(d)*step;
		Affine(cam,cam,I4);
		Affine(obj,obj,I4);
	}
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		if(mouse_flag)
		{
			mouse_flag = 0;
		}
		else
		{
			mouse_flag = 1;
		}
	}
	else if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		if(tex_id==0)
		{
			tex_id = 1;
			glClearColor(1.0, 1.0, 1.0, 0.0);
		}
		else if(tex_id==1)
		{
			tex_id = 2;
			glClearColor(0.0, 0.0, 0.0, 0.0);
		}
        else{
			tex_id = 0;
			glClearColor(1.0, 1.0, 1.0, 0.0);
        }

	}
	else if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		tex_id=0;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img"),window);
		tex_id=1;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img_wearout"),window);
		tex_id=2;
		glClearColor(0.0, 0.0, 0.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("labels_tmp"),window);
	}
	else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		resetPosition();
	}
	else if (key == GLFW_KEY_R && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))
	{
		setRandomPosition();
		tex_id=0;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img"),window);
		tex_id=1;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img_wearout"),window);
		tex_id=2;
		glClearColor(0.0, 0.0, 0.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("labels_tmp"),window);
	}
	else if (key == GLFW_KEY_C && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))
	{
		random_light=1;
	    float color[] = {(float)random_mt(), (float)random_mt(), (float)random_mt(), 1.0};
        init_light(color);
		setRandomPosition();
		tex_id=0;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img_gain"),window);
		tex_id=1;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img_gain_wearout"),window);
		tex_id=2;
		glClearColor(0.0, 0.0, 0.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("labels_tmp"),window);
		random_light=0;
	}
	else if (key == GLFW_KEY_Z && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))
	{
		random_light=1;
	    float color[] = {1.0-(0.2*(float)random_mt()), 1.0-(0.2*(float)random_mt()), 1.0-(0.2*(float)random_mt()), 1.0};
        init_light(color);
		setRandomPosition();
		tex_id=0;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img_gain"),window);
		tex_id=1;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img_gain_wearout"),window);
		random_light=0;
		
		tex_id=0;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img"),window);
		tex_id=1;
		glClearColor(1.0, 1.0, 1.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("img_wearout"),window);
		tex_id=2;
		glClearColor(0.0, 0.0, 0.0, 0.0);
		display(window);
		glfwSwapBuffers(window);
		saveImage(getFileName("labels_tmp"),window);
	}
	else if (key == GLFW_KEY_L && ((action == GLFW_PRESS)||(action == GLFW_REPEAT)))
	{
		random_light=1;
	    float color[] = {(float)random_mt(), (float)random_mt(), (float)random_mt(), 1.0};
		init_light(color);
        display(window);
		random_light=0;
    }
}

static void cursor_pos_callback(GLFWwindow* window, double x, double y)
{
	double step = M_PI/180;
	Eigen::Matrix4d I4 = Eigen::Matrix4d::Identity(4,4);
	static bool wrap = false;
	if(mouse_flag)
	{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		if(!wrap) {
            int width,height;
            glfwGetFramebufferSize(window, &width, &height);
            glfwSetCursorPos(window, width/2, height/2);
			int dx = x - width / 2;
			int dy = y - height / 2;
			if(dx <-1)
			{
				I4(0,0) = cos(step);	I4(0,1) =-sin(step);
				I4(1,0) = sin(step);	I4(1,1) = cos(step);
				obj.x -= cam.x;
				obj.y -= cam.y;
				Affine(obj,obj,I4);
				obj.x += cam.x;
				obj.y += cam.y;
				cang.pan -= step;
			}
			if(dx > 1)
			{
				I4(0,0) = cos(-step);	I4(0,1) =-sin(-step);
				I4(1,0) = sin(-step);	I4(1,1) = cos(-step);
				obj.x -= cam.x;
				obj.y -= cam.y;
				Affine(obj,obj,I4);
				obj.x += cam.x;
				obj.y += cam.y;
				cang.pan += step;
			}
			wrap = true;
        } 
        else {
            wrap = false;
        }
	}
}

int main(int argc, char **argv)
{
	int width, height;
    bool show_demo_window = true;
    bool show_another_window = false;
	float color[] = {1.0, 1.0, 1.0, 1.0};
    ImVec4 imcolor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		return 1;
	GLFWwindow *window = glfwCreateWindow(1280, 960, "ImGui OpenGL2 example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	ImGui_ImplGlfwGL2_Init(window, true);
	ImGui::StyleColorsClassic();
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(10);
	setupTexture( g_texID[0], "../texture/SoccerField3.png");
	setupTexture( g_texID[1], "../texture/SoccerField3_blur.png");
	setupTexture( g_texID[2], "../texture/whiteline.png");
	while(!glfwWindowShouldClose(window))
	{
		init();
        init_light(color);
		glfwGetFramebufferSize(window, &width, &height);
		reshape(width, height);
		display(window);
		if(!mouse_flag)
		{
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			ImGui_ImplGlfwGL2_NewFrame();
			ImGui::Text("Light Config");
			int r = imcolor.x*255;
			int g = imcolor.y*255;
			int b = imcolor.z*255;
			ImGui::SliderInt(":R",&r,0,255);
			ImGui::SliderInt(":G",&g,0,255);
			ImGui::SliderInt(":B",&b,0,255);
			imcolor.x = (float)r/255;
			imcolor.y = (float)g/255;
			imcolor.z = (float)b/255;
			ImGui::ColorEdit3("Color", (float*)&imcolor);
			color[0] = imcolor.x;
			color[1] = imcolor.y;
			color[2] = imcolor.z;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Render();
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
