#include <cmath>
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
            std::cout<<"lctime 1.0.1"<<'\n';
            return 0;
        }else {
            std::cerr<<"error: unknown option "+arg<<'\n';
            return 2;
        }
    }
    
    
   std::ifstream time_last_f("/var/lib/lctime/lctime");
    
    uint64_t time_last,time_now,time_nt,time_hour;
    long double time_day;
    
    
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
    
    
    
    time_nt=time_last+time_now;
    time_day=static_cast<double>(time_nt)/86400;
    time_hour=static_cast<uint64_t>(time_day*24)%24;

    std::cout<<std::floor(time_day)<<" days ";
    std::cout<<time_hour<<" hours"<<'\n';
    if (core) {
        std::cout<<"lctime_core is "<<"inactive"<<'\n';
    }else {
        std::cout<<"lctime_core is "<<"active"<<'\n';
    }

    return 0;
}