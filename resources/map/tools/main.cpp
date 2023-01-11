#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


struct Line{
	int x1, y1;
       	int x2, y2;
};

struct Pos{
	int x, y;
};

int main(int argc, char **argv){
	if(argc!=2){
		cout << "Error: " << argv[0] << " FILE" <<endl;
		return 1;
	}
	string filename = argv[1];
	cout << "Starting clean all the points on the file:" << filename << endl;
	
	cout << "targeting all the information from the old file..." << endl;
	ifstream file_in(filename, ios::binary);
	int size = 0;
	vector<Line> firstLines;
	vector<Line> secondLines;
	vector<Line> thirdLines;
	vector<Pos> points;
	if(file_in.is_open()){
		file_in.read((char*)&size, sizeof(int));
		for(int i(0);i<size;i++){
			Line line;
			file_in.read((char*)&line, sizeof(line));
			firstLines.push_back(line);
		}
	
	
		file_in.read((char*)&size, sizeof(int));
		for(int i(0);i<size;i++){
			Line line;
			file_in.read((char*)&line, sizeof(line));
			secondLines.push_back(line);
		}
	
		file_in.read((char*)&size, sizeof(int));
		for(int i(0);i<size;i++){
			Line line;
			file_in.read((char*)&line, sizeof(line));
			thirdLines.push_back(line);
		}
	
		file_in.read((char*)&size, sizeof(int));
		for(int i(0);i<size;i++){
			Pos pos;
			file_in.read((char*)&pos, sizeof(pos));
			points.push_back(pos);
		}
	}else{
		cout << "Error: file opening" << endl;
		return 1;
	}
	string new_filename = filename+"new.level";
	cout << "Writing in the new file:" << new_filename << endl;
	ofstream file_out(new_filename, ios::binary);

	if(file_out.is_open()){
		size = firstLines.size();
		file_out.write((char*)&size, sizeof(int));
		for(int i(0);i<size;i++){
			file_out.write((char*)&(firstLines[i]), sizeof(Line));
		}


		size = secondLines.size();
		file_out.write((char*)&size, sizeof(int));
		for(int i(0);i<size;i++){
			file_out.write((char*)&(secondLines[i]), sizeof(Line));
		}

		size = thirdLines.size();
		file_out.write((char*)&size, sizeof(int));
		for(int i(0);i<size;i++){
			file_out.write((char*)&(thirdLines[i]), sizeof(Line));
		}
		
		size = 0;
		file_out.write((char*)&size, sizeof(int));
//		for(int i(0);i<size;i++){
//			file_out.write((char*)&(firstLines[i]), sizeof(Line));
//		}
//
	}else{
		cout << "Error: file writing" << endl;
		return 1;
	}


	cout << "file is saved with success" << endl;
	return 0;
}
