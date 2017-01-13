#include <algorithm>
#include <dirent.h>
#include <iostream>
#include <vector>

using namespace std;

string dirpath = "/home/ambha/Videos";
vector<string> formats({".mp4",".avi",".mkv",".3gp",".flv", ".webm", ".MP4"});

vector<string> read_directory(string path){
	vector<string> result;
	dirent* de;
	DIR* dp;
	errno = 0;
	dp = opendir(path.c_str());
	if(dp){
		while(true){
			errno = 0;
			de = readdir(dp);
			if (de == NULL) break;
			else if (de->d_type == DT_DIR) {
				string dir_name = string(de->d_name);
				if (dir_name[0]!='.'){
					vector<string> temp = read_directory(path + "/" + dir_name);
					for (int i=0;i<temp.size();i++) result.push_back(dir_name + "/" + temp[i]);
				}
			} else {
				string filename = string(de->d_name);
				result.push_back(filename);
			}
		}
		closedir(dp);
	} else {
		cout << "Unable to open directory : " << path << endl << endl;
	}
	return result;
}


bool has_suffix(const string &str, const string &suffix){
    return (str.size()>=suffix.size() and str.compare(str.size() - suffix.size(), suffix.size(), suffix)==0);
}

vector<string> filter_formats(vector<string> &files){
	vector<string> res(0);
	for (int i=0;i<files.size();i++){
		for (int j=0;j<formats.size();j++){
			if (has_suffix(files[i],formats[j])){
				res.push_back(files[i]);
				break;
			}
		}
	}
	return res;
}


int main () {
	srand(time(NULL));
	vector<string> temp = read_directory(dirpath);
	vector<string> vids = filter_formats(temp);
	
	// for (int i=0;i<vids.size();i++) cout << vids[i] << endl;
	cout << "Total video count = " << vids.size() << endl;
	
	string random_vid_name = dirpath + "/" + vids[rand() % (vids.size()+1)];
	cout << random_vid_name << endl;
	string random_video = "totem \"" + random_vid_name + "\"";

	system(random_video.c_str());
}
