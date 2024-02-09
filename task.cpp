#include <vector>
#include "task.hpp"

Task prepareForBlockCreate(long time){
    Task task;
    task.type = blk_snd;
    task.trigger_time = time;
    return task;
}

Task prepareForBlockSend(long time){
    Task task;
    task.type = blk_snd;
    task.trigger_time = time;
    return task;
}
