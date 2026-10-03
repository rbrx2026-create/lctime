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


    uint64_t time_last{0},time_total{0},time_now{0},time_now_ns{0},time_last_ns{0},time_total_ns{0};

    struct timespec ts;   


    std::filesystem::create_directories("/var/lib/lctime");
    std::filesystem::create_directories("/etc/lctime");

    std::ifstream conf_f_r("/etc/lctime/lctime.conf");
    if (!conf_f_r) {
        conf_f_r.close();
        std::ofstream conf_f_w("/etc/lctime/lctime.conf");
        conf_f_w<<"TIME_NS=false"<<'\n';
    }

    std::ifstream time_lastc_f_r("/var/lib/lctime/lctime_c");
    std::ofstream time_last_f_w_t("/var/lib/lctime/lctime");
    time_lastc_f_r>>time_last>>time_last_ns;
    time_last_f_w_t<<time_last<<' '<<time_last_ns<<'\n';
    time_last_f_w_t.close();
    time_lastc_f_r.close();

    for(;running;){
        sleep(1800);
        if (!running) break;

        if (clock_gettime(CLOCK_BOOTTIME,&ts)==0){
            time_now=ts.tv_sec;
            time_now_ns=ts.tv_nsec;
        }else {
            return 1;
        }

        time_total_ns=time_last_ns+time_now_ns;
        time_total=time_last+time_now+time_total_ns/1000000000ULL;
        time_total_ns=time_total_ns%1000000000ULL;
    

        std::ofstream time_lastc_f_w ("/var/lib/lctime/lctime_c");
        time_lastc_f_w<<time_total<<' '<<time_total_ns<<'\n';
    }

    clock_gettime(CLOCK_BOOTTIME,&ts);
    time_now=ts.tv_sec;
    time_now_ns=ts.tv_nsec;
    time_total_ns=time_last_ns+time_now_ns;
    time_total=time_last+time_now+time_total_ns/1000000000ULL;
    time_total_ns=time_total_ns%1000000000ULL;

    std::ofstream time_lastc_f_w ("/var/lib/lctime/lctime_c");
    std::ofstream time_last_f_w ("/var/lib/lctime/lctime");
    time_lastc_f_w<<time_total<<' '<<time_total_ns<<'\n';
    time_last_f_w<<time_total<<' '<<time_total_ns<<'\n';

    return 0;
}