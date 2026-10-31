#include <atomic>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
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



/*
int time_f_w(uint64_t &time_now,uint64_t &time_now_ns,uint64_t &time_last,uint64_t &time_last_ns,struct timespec *ts,const std::atomic<bool>& run){         //已改为lambda函数
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
*/


int main(){
    std::signal(SIGTERM,sleeped);
    std::signal(SIGINT,sleeped);


    uint64_t time_last{0},time_now{0},time_now_ns{0},time_last_ns{0},time_boot{0},time_boot_ns{0};
    std::string boot_id_now,boot_id_last;

    //bool boot_id_bool=false; //boot_id相同则为true

  

    std::filesystem::create_directories("/var/lib/lctime");
    std::filesystem::create_directories("/etc/lctime");

    std::ifstream conf_f_r("/etc/lctime/lctime.conf");
    if (!conf_f_r) {
        conf_f_r.close();
        std::ofstream conf_f_w("/etc/lctime/lctime.conf");
        conf_f_w<<"TIME_NS=false"<<'\n';
        
    }
    conf_f_r.close();

    struct timespec ts;  

    if(clock_gettime(CLOCK_BOOTTIME,&ts)!=0){
        std::ofstream log ("/etc/lctime/lctime.log");
        
        return 1;
    }


    std::ifstream boot_id_f ("/proc/sys/kernel/random/boot_id");
    if (!boot_id_f) {                                                           //获取boot_id
        std::ofstream log ("/etc/lctime/lctime.log",std::ios::app);
        if (!log) {
            return 4;
        }
        log<<ts.tv_sec<<' '<<"Failed to open boot_id file"<<'\n';
        return 5;
    }else {
        if (!(boot_id_f>>boot_id_now)) {
             std::ofstream log ("/etc/lctime/lctime.log",std::ios::app);
        if (!log) {
            return 4;
        }
        log<<ts.tv_sec<<' '<<"Failed to read boot_id"<<'\n';
        return 2;
        }
    }

    std::ifstream lctime_f_c_r_t ("/etc/lctime/lctime_c");
    if (!lctime_f_c_r_t) {
        boot_id_last.clear();
    }else {
        lctime_f_c_r_t>>time_last>>time_last_ns>>boot_id_last>>time_boot>>time_boot_ns;
    }
    lctime_f_c_r_t.close();


    std::ofstream lctime_temp_f_w_t ("/etc/lctime/lctime_temp");
    if (boot_id_last.empty()) {
        
    }else if (boot_id_last==boot_id_now) {
        time_last=time_boot;
        time_last_ns=time_boot_ns;
        lctime_temp_f_w_t<<time_last<<'\n'<<time_last_ns<<'\n';
        lctime_temp_f_w_t.close();
        std::filesystem::rename("/etc/lctime/lctime_temp","/etc/lctime/lctime");
        
    }else {
        std::ofstream lctime_c_temp_f_w_t ("/etc/lctime/lctime_c_temp");
        if (!lctime_c_temp_f_w_t) {
            return 4;
        }else {
            lctime_c_temp_f_w_t<<time_last<<'\n'<<time_last_ns<<'\n'<<boot_id_now<<'\n'<<time_last<<'\n'<<time_last_ns;
            lctime_c_temp_f_w_t.close();
            std::filesystem::rename("/etc/lctime/lctime_c_temp","/etc/lctime/lctime_c");
        }
    }

    stat:auto time_f_w=[&](){
        if (clock_gettime(CLOCK_BOOTTIME,&ts)==0){
            time_now=ts.tv_sec;
            time_now_ns=ts.tv_nsec;
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
        if (!running) {
            std::ofstream time_last_f_w("/var/lib/lctime/lctime_temp");
            time_last_f_w<<time_total<<'\n'<<time_total_ns<<'\n';
            time_last_f_w.close();
            std::filesystem::rename("/var/lib/lctime/lctime_temp","/var/lib/lctime/lctime");

        }
        return 0;
    };

    for(;running;){
        sleep(1800);
        if (!running) break;

        time_f_w();

    }

    time_f_w();


    return 0;
}
