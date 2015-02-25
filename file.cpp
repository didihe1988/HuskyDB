#include "file.h"


File::File(std::string filename)
{
    this->filename=filename;
    this->stream->open (this->filename,std::ios_base::ate);

}

File::~File (){
    this->stream->close ();
}

Page File::readPage(const int page_id) const
{
    Page page;
    this->stream->seekg (pagePosition (page_id),std::ios_base::beg);
    this->stream->read (reinterpret_cast<char*> (&page.header),sizeof(PageHeader));
    this->stream->read (reinterpret_cast<char*> (&page.data[0]),Page::DATA_SIZE);
    return page;
}

void File::writePage(const int page_id, const Page &new_page)
{
    Page old_page=readPage (page_id);
    const int next_page_id=old_page.header.next_pos_in_file;
    old_page=new_page;
    old_page.header.next_pos_in_file=next_page_id;
    this->stream->seekp (pagePosition (page_id),std::ios_base::beg);
    this->stream->write (reinterpret_cast<char*> (&old_page.header),sizeof(PageHeader));
    this->stream->write (reinterpret_cast<char*> (&old_page.data[0]),Page::DATA_SIZE);
}



FileHeader File::readFileHeader() const
{
    FileHeader header;
    this->stream->seekg (0,std::ios_base::beg);
    this->stream->read (reinterpret_cast<char*> (&header),sizeof(FileHeader));
    return header;
}

void File::writeFileHeader(const FileHeader &header)
{
    this->stream->seekp (0,std::ios_base::beg);
    this->stream->write (reinterpret_cast<const char*> (&header),sizeof(FileHeader));
}






