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
            std::cout<<"lctime 1.0.4"<<'\n';
            return 0;
        }else {
            std::cerr<<"error: unknown option "+arg<<'\n';
            return 2;
        }
    }
    
    
    std::ifstream time_last_f("/var/lib/lctime/lctime");
   
   
    std::ifstream conf_f("/etc/lctime/lctime.conf");
    bool conf=false;
    if (conf_f) {  
        std::string conf_str;
        std::getline(conf_f,conf_str,'\n');
        conf = conf_str=="TIME_NS=true";
    }
   
    uint64_t time_last{0},time_now{0},time_total{0},time_hour{0},time_minute{0},time_day{0},time_second{0},time_now_ns{0},time_ns{0},time_ms{0},time_μs{0},time_last_ns{0},time_total_ns{0};
    
    
    std::string cmd="pgrep -x lctime_core >/dev/null 2>&1";    /*获取后台服务状态*/
    bool core;
    if (system(cmd.c_str())) {
        core=true;
    }else {
        core=false;
    }

    
    if (!time_last_f) {
        time_last=0;
        time_last_ns=0;
    }else {
       time_last_f>>time_last>>time_last_ns;
    }
    
    
    struct timespec ts;   
    if (clock_gettime(CLOCK_BOOTTIME,&ts)==0){
        time_now=ts.tv_sec;
        time_now_ns=ts.tv_nsec;
    }else {
        std::cerr<<"CLOCK_BOOTTIME,ERR"<<'\n';
        return 1;
    }
    
    
    time_total_ns=time_last_ns+time_now_ns;
    time_total=time_last+time_now+time_total_ns/1000000000ULL;

    time_day=time_total/86400;
    time_hour=(time_total/3600)%24;
    time_minute=(time_total/60)%60;
    time_second=time_total%60;


   
    if(conf) {
        std::cout<<time_day<<"d "
                 <<time_hour<<"h "
                 <<time_minute<<"m "
                <<time_second<<"s ";


        time_ms=(time_total_ns/1000000)%1000;
        time_μs=(time_total_ns/1000)%1000;
        time_ns=time_total_ns%1000;


        std::cout<<time_ms<<"ms "
                <<time_μs<<"μs "
                <<time_ns<<"ns "<<'\n';
    }else{
          std::cout<<time_day<<"d "
                   <<time_hour<<"h "
                   <<time_minute<<"m "
                   <<time_second<<"s "<<'\n';

    }
    
   



    if (core) {
        std::cout<<"lctime_core is "<<"inactive"<<'\n';
    }else {
        std::cout<<"lctime_core is "<<"active"<<'\n';
    }

    return 0;
}