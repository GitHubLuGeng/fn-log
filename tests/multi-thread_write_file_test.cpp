#include "fn_log.h"



constexpr int size = sizeof(FNLog::Logger);
static const std::string example_config_text =
R"----(
 # ѹ�����  
 # 0ͨ��Ϊ���߳��ļ������һ��CLSɸѡ��������� 
 - channel: 0
    sync: null
    -device: 0
        disable: false
        out_type: file
        filter_level: trace
        path: "./log/"
        file: "$PNAME_$YEAR$MON$DAY"
        rollback: 4
        limit_size: 100 m #only support M byte
    -device:1
        disable: false
        out_type: screen
        filter_level: info
 # 1ͨ��Ϊ���̲߳����κ������ 
 - channel: 1

)----";



enum State
{
    WAIT,
    WRITE,
    END
};

State state = WRITE;

void thread_proc(int index)
{
    LOGI() << "thread:<" << index << "> begin.";
    while (state != END)
    {
        if (state == WRITE)
        {
            LOGD().write_buffer("rrrrrrrrrrrrrrrrrrrradfads33333333333333rrd",
                sizeof("rrrrrrrrrrrrrrrrrrrradfads33333333333333rrd") - 1)
                << -23 << ": " << 32.2223 << (void*) nullptr;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    LOGI() << "thread:<" << index << "> end.";
}

const int WRITE_THREAD_COUNT = 6;
std::thread g_multi_proc[WRITE_THREAD_COUNT];

int main(int argc, char* argv[])
{
    FNLog::Logger& logger = FNLog::GetDefaultLogger();
    FNLog::GuardLogger gl(logger);
    FNLog::InitFromYMAL(example_config_text, "", logger);
    int ret = FNLog::StartDefaultLogger();
    if (ret != 0 || logger.last_error_ != 0)
    {
        printf("log start error. ret:<%d>.", ret);
        return ret || FNLog::GetDefaultLogger().last_error_;
    }
    int limit_second = 0;
    int thread_id = 0;
    g_multi_proc[thread_id] = std::thread(thread_proc, thread_id);

    do
    {
        long long last_writed = logger.channels_[0].devices_[0].log_fields_[FNLog::DEVICE_LOG_TOTAL_WRITE_LINE].num_;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        long long now_writed = logger.channels_[0].devices_[0].log_fields_[FNLog::DEVICE_LOG_TOTAL_WRITE_LINE].num_;
        LOGI() << "now thread:" << thread_id+1 << ": writed:" << now_writed - last_writed << ", cache hit:"
            << (double)logger.channels_[0].log_fields_[FNLog::CHANNEL_LOG_ALLOC_CACHE].num_
            / logger.channels_[0].log_fields_[FNLog::CHANNEL_LOG_ALLOC_CALL].num_ * 100.0;
        if (limit_second/10 > thread_id && thread_id+1 < WRITE_THREAD_COUNT)
        {
            thread_id++;
            LOGI() << "add new thread:" << thread_id;
            g_multi_proc[thread_id] = std::thread(thread_proc, thread_id);
        }
        limit_second++;
    } while (limit_second < 40);

    LOGA() << "finish";
    state = END;
    for (int i = 0; i <= thread_id; i++)
    {
        if (g_multi_proc[i].joinable())
        {
            g_multi_proc[i].join();
        }
    }
    return 0;
}

