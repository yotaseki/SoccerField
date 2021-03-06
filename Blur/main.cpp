#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
using namespace std;
namespace fs = boost::filesystem;

void Blur(cv::Mat &src)
{
	int N = 8;
	cv::Mat org = src.clone();
	for(int y=0;y<src.rows;y++)
	{
		for(int x=N; x<(src.cols-N);x++)
		{
			int bgr[] = {0, 0, 0};
			for(int n=-N;n<=N;n++)
			{
				bgr[0] += org.at<cv::Vec3b>(y,x+n)[0];
				bgr[1] += org.at<cv::Vec3b>(y,x+n)[1];
				bgr[2] += org.at<cv::Vec3b>(y,x+n)[2];
			}
			src.at<cv::Vec3b>(y,x)[0] = (double)bgr[0] / (2*N+1);
			src.at<cv::Vec3b>(y,x)[1] = (double)bgr[1] / (2*N+1);
			src.at<cv::Vec3b>(y,x)[2] = (double)bgr[2] / (2*N+1);
		}
	}
}

void bgr2gray(cv::Mat &src)
{
	cv::cvtColor(src,src,CV_BGR2GRAY);
	for(int y=0;y<src.rows;y++)
	{
		for(int x=0;x<src.cols;x++)
		{
			if(src.at<unsigned char>(y,x) > 0)
			{
				src.at<unsigned char>(y,x) = 1;
			}
		}
	}
}

int run(char *dirpath, char *output, int color)
{
    /*
	if(argc < 3)
	{
		cout << "./Blur <path> <color>" << endl;
		exit(0);
	}
    */

	const fs::path path(dirpath);
	BOOST_FOREACH(const fs::path& p, std::make_pair(fs::directory_iterator(path),
				fs::directory_iterator())) {
		if (!fs::is_directory(p))
		{
			fs::path extension = p.extension();
			if((extension.generic_string() == ".png") || (extension.generic_string() == ".jpg"))
			{
				cv::Mat m = cv::imread(p.c_str());
				if(m.empty())
				{
					cout << "input error" << endl;
					exit(0);
				}
				if(color==1)
				{
					std::cout << "blur "<< p.filename()  << std::endl;
					Blur(m);
				}
				else if(color==-1)
				{
					std::cout << "gray "<< p.filename()  << std::endl;
					bgr2gray(m);
				}
				else
				{
					std::cout << "resize "<< p.filename()  << std::endl;
				}
				cv::resize(m,m,cv::Size(400,400));
                string out(output);
				string fn = out + "/"  + p.filename().string();
				cv::imwrite(fn, m);
			}
		}
	}
	return 0;
}

int main(void ){
    run("in", "out", 1);
    /*
    run("img", "original", 0);
	run("img", "blur", 1);

	run("img_wearout", "wearout", 0);
    run("img_wearout", "wearout_blur", 1);
    
    run("img_gain", "gain", 0);
	run("img_gain", "gain_blur", 1);
    
    run("img_gain_wearout", "gain_wearout", 0);
	run("img_gain_wearout", "gain_wearout_blur", 1);
    
    run("labels_tmp", "labels_gray", -1);
    */
    return 0;
}

