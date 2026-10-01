#include <atomic>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <csignal>



std::atomic<bool> running =true;
/*
int now_time(struct timespec *ts){
    clock_gettime(CLOCK_BOOTTIME,ts);
    return ts->tv_sec;
}
*/
void sleeped(int){
        running=false;
}

int main(){
    std::signal(SIGTERM,sleeped);
    std::signal(SIGINT,sleeped);


    uint64_t time_last=0,time_temp=0,time_now=0;

    struct timespec ts;   


    std::filesystem::create_directories("/var/lib/lctime");
    std::ifstream time_lastc_f_r("/var/lib/lctime/lctime_c");
    std::ofstream time_last_f_w_t("/var/lib/lctime/last_time");
    time_lastc_f_r>>time_last;
    time_last_f_w_t<<time_last<<'\n';
    time_last_f_w_t.close();
    time_lastc_f_r.close();

    for(;running;){
        sleep(1800);
        if (!running) break;

        if (clock_gettime(CLOCK_BOOTTIME,&ts)==0){
            time_now=ts.tv_sec;
        }else {
            return 1;
        }

        time_temp=time_last+time_now;

        std::ofstream time_lastc_f_w ("/var/lib/lctime/lctime_c");
        time_lastc_f_w<<time_temp<<'\n';
    }

    clock_gettime(CLOCK_BOOTTIME,&ts);
    time_now=ts.tv_sec;
    time_temp=time_last+time_now;

    std::ofstream time_lastc_f_w ("/var/lib/lctime/lctime_c");
    std::ofstream time_last_f_w ("/var/lib/lctime/last_time");
    time_lastc_f_w<<time_temp<<'\n';
    time_last_f_w<<time_temp<<'\n';

    return 0;
}