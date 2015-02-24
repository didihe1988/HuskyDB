#ifndef PAGE
#define PAGE
#include <string>
#include "cstddef"

//rid
struct RecordId{
    int page_id;
    int slot_id;
};

struct PageHeader{
    int cur_pos_in_file;

    int next_pos_in_file;

    int slot_num;

    int free_slot_num;

    int free_space_lower_bound;

    int free_space_upper_bound;
};

struct Slot{
    bool used;

    int record_offset;

    int record_length;
};

class Page{
public:
    const static int SIZE=8192;
    const static int DATA_SIZE=SIZE-sizeof(PageHeader);
private:
    typedef int SlotId;
    const static int INVALID_SLOTID=-1;

    PageHeader header;
    std::string data;

public:
    Page();
    ~Page();
    bool hasFreeSpaceForRecord(const std::string& record);
    RecordId insertRecord(const std::string& record);
    void updateRecord(const RecordId& record_id,const std::string& record);
    void deleteRecord(const RecordId &record_id);
 private:
    int getFreeSpace() const;
    Slot& getSlot(const SlotId slot_id);
    SlotId getAvailableSlot();
    void insertRecordInSlot(const std::string& record,const SlotId slot_id);
    void deleteRecord(const RecordId& record_id,const bool allow_to_compact_slots);

};

#endif
