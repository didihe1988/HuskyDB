#ifndef FILE_H
#define FILE_H

#include<fstream>
#include<string>
#include "page.h"

struct FileHeader{
    int page_num;
    int free_page_num;
    int first_page;
    int first_free_page;
};

class File
{
private:
    std::fstream* stream;
    std::string filename;
public:
     File(std::string filename);
     ~File();
    Page readPage(const int page_id) const;
    void writePage(const int page_id,const Page& new_page);

private:
    FileHeader readFileHeader() const;
    void writeFileHeader(const FileHeader& header);
    inline std::streamoff pagePosition(const int page_id) const{
        return sizeof(FileHeader)+page_id*Page::SIZE;
    }

};

#endif // FILE_H
