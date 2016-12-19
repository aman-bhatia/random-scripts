#include <iostream>
#include <vector>
#include <dirent.h>

using namespace std;

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

int main(){

	vector<string> vs = read_directory("./test_dir/");
	
	for (int i=0;i<vs.size();i++){
		cout << vs[i] << endl;
	}
}