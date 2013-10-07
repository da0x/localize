//
//   Copyright 2013 Daher Alfawares
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#include <fstream>
#include <sstream>
#include <string>
#include <iterator>
#include <fstream>
#include <iostream>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

// Purpose:
// Takes the content of an objective-c file and replaces @"" with NSLocalizedString(@"",nil)

namespace localize
{
    const char* expression_text =
    // preprocessor directives: index 1
    "(^[[:blank:]]*#(?:[^\\\\\\n]|\\\\[^\\n[:punct:][:word:]]*[\\n[:punct:][:word:]])*)|"
    // comment: index 2
    "(//[^\\n]*|/\\*.*?\\*/)|"
    // already localized: index 3
    "('(?:[^\\\\']|\\\\.)*'|NSLocalizedString\\((?:[^\\\\\(]|\\\\.)*\\))|"
    // objective-c string literals: index 4
    "('(?:[^\\\\']|\\\\.)*'|@\"(?:[^\\\\\"]|\\\\.)*\")"
    ;
    
    const char* format_string =
    "(?1$&)"
    "(?2$&)"
    "(?3$&)"
    "(?4NSLocalizedString\\($&,nil\\))";
    
    void load_file(std::string& s, std::istream& is)
    {
        s.erase();
        s.reserve(is.rdbuf()->in_avail());
        char c;
        while(is.get(c))
        {
            if(s.capacity() == s.size())
                s.reserve(s.capacity() * 3);
            s.append(1, c);
        }
    }
    
    void localize_file( std::string filename )
    {
        boost::regex expression(expression_text);

        std::cout << "Processing file " << filename << std::endl;
        std::string in;
        {
            std::ifstream fs(filename);
            load_file(in, fs);
        }
        
        std::string out_name(filename);
        std::ofstream os(out_name.c_str());
        
        std::ostream_iterator<char, char> out(os);
        boost::regex_replace(out,
                             in.begin(),
                             in.end(),
                             expression,
                             format_string,
                             boost::regex_constants::match_default | boost::format_all);
    }
    
    bool parse_files( const boost::filesystem::path & dir_path,
                      const std::string             & extension )
    {
        using namespace boost::filesystem;
        
        if ( !exists( dir_path ) )
            return false;
        
        directory_iterator end_itr; // default construction yields past-the-end
        for ( directory_iterator itr( dir_path );
             itr != end_itr;
             ++itr )
        {
            if ( is_directory(itr->status()) )
            {
                if ( parse_files( itr->path(), extension ) )
                    return true;
            }
            else
            {
                std::string path = itr->path().native();
                if( path.compare( path.size() - extension.length(), 1, extension ) )
                {
                    localize_file( path );
                }
                    
                return true;
            }
        }
        return false;
    }
}
/*
int main(int argc, const char** argv)
{
    try{
        std::cout << "Parsing " << boost::filesystem::initial_path().string() << std::endl;
        
        localize::parse_files( "/", ".m" );
    }
    catch(...)
    { return -1; }
    return 0;
}

*/

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

/*function... might want it in some class?*/
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

int main()
{
    string dir = string("/Users/my-username/folder-where-source-files-exist");
    vector<string> files;
    
    getdir(dir,files);
    
    for (unsigned int i = 0;i < files.size();i++) {
        std::string path = files[i];
        
        if( path.size() > 2 && !path.compare( path.size() - 2, 2, ".m" ) )
        {
            localize::localize_file( dir + path );
        }
    }
    return 0;
}
