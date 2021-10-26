#include "files.h"
#include <iostream>
#include <fstream> 

File::File(const char* path)
{
    file_path = path;
    ifstream t(path);
    if (t.good())
        file_buffer << t.rdbuf();
    else
        cout << "File " << path << " does not exist" << endl;
}

string File::get_value(const char* key_to_find){
    cout << "Searching for " << key_to_find << endl;
    istringstream is_file(file_buffer.str());

    string line;
    while( getline(is_file, line) )
    {
        istringstream is_line(line);
        string key;
        if( getline(is_line, key, '=') )
        {
            string value;
            if (key.compare(key_to_find) == 0){
                if( getline(is_line, value) ){
                    return value;
                }
            }
        }
    }
    cout << key_to_find << " is not found" << endl;
    return string("\0");
}

void File::write_binary_data(char *buf, int size){
    ofstream wf(file_path, ios::out | ios::binary);
    if(!wf) {
        cout << "Cannot open file!" << endl;
    }
    else{
        wf.write(buf, size);
    }
    wf.close();
    if(!wf.good()) {
        cout << "Error occurred at writing time!" << endl;
    }
}
