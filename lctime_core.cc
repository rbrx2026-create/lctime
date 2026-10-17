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

int time_f_w(uint64_t &time_now,uint64_t &time_now_ns,uint64_t &time_last,uint64_t &time_last_ns,struct timespec *ts,bool run){
    if (clock_gettime(CLOCK_BOOTTIME,ts)==0){
            time_now=ts->tv_sec;
            time_now_ns=ts->tv_nsec;
        }else {
            return 1;
        }

        uint64_t time_total{0},time_total_ns{0};

        time_total_ns=time_last_ns+time_now_ns;
        time_total=time_last+time_now+time_total_ns/1000000000ULL;
        time_total_ns=time_total_ns%1000000000ULL;
    

        std::ofstream time_lastc_f_w ("/var/lib/lctime/lctime_c_temp");
        time_lastc_f_w<<time_total<<'\n'<<time_total_ns<<'\n';
        time_lastc_f_w.close();
        std::filesystem::rename("/var/lib/lctime/lctime_c_temp","/var/lib/lctime/lctime_c");
        if (!run) {
            std::ofstream time_last_f_w("/var/lib/lctime/lctime_temp");
            time_last_f_w<<time_total<<'\n'<<time_total_ns<<'\n';
            time_last_f_w.close();
            std::filesystem::rename("/var/lib/lctime/lctime_temp","/var/lib/lctime/lctime");

        }
        return 0;

}



int main(){
    std::signal(SIGTERM,sleeped);
    std::signal(SIGINT,sleeped);


    uint64_t time_last{0},time_now{0},time_now_ns{0},time_last_ns{0};

    struct timespec ts;   
    clock_gettime(CLOCK_BOOTTIME,&ts);


    std::filesystem::create_directories("/var/lib/lctime");
    std::filesystem::create_directories("/etc/lctime");

    std::ifstream conf_f_r("/etc/lctime/lctime.conf");
    if (!conf_f_r) {
        conf_f_r.close();
        std::ofstream conf_f_w("/etc/lctime/lctime.conf");
        conf_f_w<<"TIME_NS=false"<<'\n';
    }

    uint64_t time_temp{0},time_temp_ns{0};

    std::ifstream time_lastc_f_r("/var/lib/lctime/lctime_c");
    std::ifstream time_last_f_r_t("/var/lib/lctime/lctime");
    
    time_lastc_f_r>>time_last>>time_last_ns;
    time_last_f_r_t>>time_temp>>time_temp_ns;
    
    if (((time_temp+ts.tv_sec)>time_last)){
        std::swap(time_last,time_temp);
        std::swap(time_last_ns,time_temp_ns);
        time_lastc_f_r.close();
        time_last_f_r_t.close();
    }else{
        std::ofstream time_last_f_w_t("/var/lib/lctime/lctime");
        time_last_f_w_t<<time_last<<'\n'<<time_last_ns<<'\n';
        time_last_f_w_t.close();
        time_lastc_f_r.close();
        time_last_f_r_t.close();
    }

    for(;running;){
        sleep(1800);
        if (!running) break;

        time_f_w(time_now,time_now_ns,time_last,time_last_ns,&ts,running);

    }

    time_f_w(time_now,time_now_ns,time_last,time_last_ns,&ts,running);


    return 0;
}
