#include <iostream>
#include <filesystem>
#include <string>
#include <regex>
#include <fstream>

#define TRAINMODEL_FOLDER "../resources/trained_model/"
#define SAVE_FILE "../resources/extracted/data.txt"

namespace fs = std::filesystem;
using namespace std;



std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}


int main(int argc, char **argv){
    string filename = "";
    if(argc==2){
        cout << "Extraction du réseau de neurones pour l'envoyer dans l'arduino" << endl;
        filename = argv[1];
        cout << "fichier:" << filename << endl;
    }else if(argc==1){
        cout << "Extraction du réseau de neurones pour l'envoyer dans l'arduino" << endl;
        std::string path = TRAINMODEL_FOLDER;
        cout << "Selection du meilleur réseau de neurones dans "<< path << endl;
        int max_score = 0;
        smatch m;
        regex r("_([0-9]+).ml");
        for (const auto & entry : fs::directory_iterator(path)){
            string name = entry.path();
            regex_search(name,m,r);
            int score = 0;
            if(m.size()>0){
                auto v = m[1];
                //cout << "score:" << v << endl;
                score = stoi(v);
                if(score>max_score){
                    max_score = score;
                    filename = name;
                }
            }
            
        }
        cout << "filename:" << filename << endl;
    }else{
        cout << "Argument error" << endl;
        cout << argv[0] << " path_to_neuralnetwork.ml" << endl;
        return 1;
    }

    cout << "Ouverture du fichier..." << endl; 
    ifstream file(filename, ios::binary);
    if(file.is_open()){
        string data = "";
        cout << "Code arduino:" <<endl;
        int size = filesize(filename.c_str());
        data="const PROGMEM char data["+to_string(size)+"]={";
        char buffer = 0;
        for(int i=0;i<size;i++){
            file.read(&buffer,1);
            data+=to_string(int(buffer));
            if(i!=size-1){
                data+=",";
            }
        }
        data+="};\n";
        cout << data;
        
        cout << "Enregistrement dans le fichier " << SAVE_FILE << endl;
        ofstream save_file(SAVE_FILE);
        if(!save_file.is_open()){
            cout << "Erreur lors de l'ouverture du fichier" << endl;
            return 1;
        }
        save_file << data;
        cout << "Enregistrement effectué avec succés" << endl;
    }else{
        cout << "Fichier inexistant" << endl;
        return 1;
    }

    return 0;
}