#include "page.h"



Page::Page(){
    this->header.free_space_lower_bound=0;
    this->header.free_space_upper_bound=DATA_SIZE;
    this->header.free_slot_num=0;
    this->header.slot_num=0;
    data.assign (DATA_SIZE,'\0');
}

Page::~Page (){

}

bool Page::hasFreeSpaceForRecord(const std::string &record){
    if(this->header.free_slot_num<1){
        return (sizeof(Slot)+record.length ()) <= getFreeSpace ();
    }

    return record.length () <= getFreeSpace ();

}

RecordId Page::insertRecord(const std::string &record){
    if(hasFreeSpaceForRecord (record)){
        const SlotId slot_id=getAvailableSlot ();
        insertRecordInSlot (record,slot_id);
        return {this->header.cur_pos_in_file,slot_id};
    }
    return {this->header.cur_pos_in_file,INVALID_SLOTID};
}

void Page::updateRecord(const RecordId &record_id, const std::string &record)
{
    deleteRecord (record_id,true);
    insertRecordInSlot (record,record_id.slot_id);
}

void Page::deleteRecord(const RecordId &record_id)
{
    deleteRecord (record_id,true);
}

int Page::getFreeSpace() const{
    return this->header.free_space_upper_bound-this->header.free_space_lower_bound;
}

Slot& Page::getSlot(const Page::SlotId slot_id){
    return *( reinterpret_cast<Slot*> (&data[slot_id*sizeof(Slot)]) );
}

Page::SlotId Page::getAvailableSlot(){
    SlotId result=INVALID_SLOTID;
    if(this->header.free_slot_num>0){
        for(SlotId id=0;id<this->header.slot_num;++id){
            const Slot slot=getSlot (id);
            if(!slot.used){
                result=id;
                break;
            }
        }
    }
    else{
        result=++this->header.slot_num;
        this->header.free_slot_num++;
        this->header.free_space_upper_bound+=sizeof(Slot);
    }
    return result;
}

void Page::insertRecordInSlot(const std::string &record, const Page::SlotId slot_id){
    Slot slot=getSlot (slot_id);
    slot.used=true;
    slot.record_length=record.length ();
    slot.record_offset=this->header.free_space_upper_bound-record.length ();

    this->header.free_space_upper_bound=slot.record_offset;
    this->header.free_slot_num--;

    data.replace (slot.record_offset,slot.record_length,record);

}

void Page::deleteRecord(const RecordId &record_id, const bool allow_to_compact_slots){
    const Slot slot=getSlot (record_id.slot_id);
    this->data.replace (slot.record_offset,slot.record_length,'\0');

    int move_length=0;
    int move_offset=slot.record_offset;
    for(int id=0;id<this->header.slot_num;++id){
        Slot other_slot=getSlot(id);
        if(other_slot.record_offset<slot.record_offset){
            move_offset=move_offset<other_slot.record_offset ? move_offset:other_slot.record_offset;
            move_length+=other_slot.record_length;
            other_slot.record_offset+=slot.record_length;
        }
    }

    if(move_length>0){
        std::string move_data=this->data.substr (move_offset,move_length);
        this->data.replace (move_offset+slot.record_length,move_length,move_data);
    }

    if(allow_to_compact_slots && ( (record_id.slot_id+1)==this->header.slot_num)){
        int delete_slot_num=1;
        int id=this->header.slot_num-1;
        for(;id>=0;--id){
            const Slot slot=getSlot (id);
            if(!slot.used){
                ++delete_slot_num;
            }
            else{
                break;
            }
        }

        this->header.slot_num-=delete_slot_num;
        this->header.free_slot_num-=delete_slot_num;
        this->header.free_space_lower_bound-=sizeof(Slot)*delete_slot_num;
    }

}
