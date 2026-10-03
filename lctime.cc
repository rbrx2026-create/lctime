#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>


int main(int argc, char* argv[]){
    if (argc>=2) {    
    
        std::string arg=argv[1];
        if (arg=="-v") {
            std::cout<<"lctime 1.0.2"<<'\n';
            return 0;
        }else {
            std::cerr<<"error: unknown option "+arg<<'\n';
            return 2;
        }
    }
    
    
    std::ifstream time_last_f("/var/lib/lctime/lctime");
    
    uint64_t time_last,time_now,time_total,time_hour,time_minute,time_day,time_second;
    
    
    std::string cmd="pgrep -x lctime_core >/dev/null 2>&1";    /*获取后台服务状态*/
    bool core;
    if (system(cmd.c_str())) {
        core=true;
    }else {
        core=false;
    }

    
    if (!time_last_f) {
        time_last=0;
    }else {
       time_last_f>>time_last;
    }
    
    
    struct timespec ts;   
    if (clock_gettime(CLOCK_BOOTTIME,&ts)==0){
        time_now=ts.tv_sec;
    }else {
        std::cerr<<"CLOCK_BOOTTIME,ERR"<<'\n';
        return 1;
    }
    
    time_total=time_last+time_now;
    time_day=time_total/86400;
    time_hour=(time_total/3600)%24;
    time_minute=(time_total/60)%60;
    time_second=time_total%60;

    std::cout<<time_day<<" days ";
    std::cout<<time_hour<<" hours ";
    std::cout<<time_minute<<" minutes ";
    std::cout<<time_second<<" seconds"<<'\n';

    if (core) {
        std::cout<<"lctime_core is "<<"inactive"<<'\n';
    }else {
        std::cout<<"lctime_core is "<<"active"<<'\n';
    }

    return 0;
}