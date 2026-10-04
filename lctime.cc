#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

bool lctime_conf_r (const std::string& conf){
    std::ifstream conf_f("/etc/lctime/lctime.conf");
    if(!conf_f) {
        std::cerr<<"lctime: failed to open /etc/lctime/lctime.conf for reading"<<'\n';
        return false;
    }else{
        std::string conf_str;
        while (std::getline(conf_f, conf_str)) {
            if(conf_str==conf) {
                return true;
            }
        }
            return false;
    }
}

std::string lctime_argv (std::vector<std::string> args,std::string argv_total){
    for(const auto& arg:args){
        if (argv_total.find(" "+arg+" ")==std::string::npos) {
            std::cerr<<"lctime: invalid option -- '"<<arg<<"'"<<'\n';
            return "error";
        }
            return arg;
        
    }
    return "error";
}
bool lctime_conf_argv (std::string conf,std::string conf_total){  //检查用户输入的配置项是否存在
   if (conf_total.find(" "+conf+" ")==std::string::npos) {
        std::cerr<<"lctime: invalid option -- '"<<conf<<"'"<<'\n';
        return false;
    }else if(conf_total.find(" "+conf+" ")!=std::string::npos) {
        return true;
    }
    return false;
   
   }

bool lctime_conf_w(std::string conf){
    std::ifstream conf_f_r_t("/etc/lctime/lctime.conf");    //写入配置文件
    std::vector<std::string> conf_vec;
    std::string conf_str;
    while (std::getline(conf_f_r_t, conf_str)) {
        conf_vec.push_back(conf_str);
    }
    conf_f_r_t.close();
    int conf_count{0};
    for (const auto& c:conf_vec) {
       if(c=="TIME_NS=true"||c=="TIME_NS=false"||c=="TIME_NS"||c=="TIME_NS=") {
            break;
        }
        ++conf_count;
    }
    std::ofstream conf_f_w_t("/etc/lctime/lctime.conf",std::ios::app);
    if(conf_f_w_t){ 
        if((conf_count)<(conf_vec.size())) {
            conf_vec[conf_count]=conf;
            std::ofstream conf_f_w_t("/etc/lctime/lctime.conf");
            for (const auto& c:conf_vec) {
                conf_f_w_t<<c<<'\n';
            }
            conf_f_w_t.close();
            return true;
        }else{
                conf_f_w_t<<conf<<'\n';
                conf_f_w_t.close();
                return true;
        }
    }else{
        std::cerr<<"lctime: failed to open /etc/lctime/lctime.conf for writing"<<'\n';
        return false;
    }
    
}


int main(int argc, char* argv[]){

    std::string argv_total{" -v --version -h --help -c --conf "};
    std::string conf_total{" TIME_NS  TIME_NS=true  TIME_NS=false "};

    if (argc>=2) {    
    
        std::vector<std::string> args;
        for (int i=1;i<argc;i++) {
            args.push_back(argv[i]);
        }
        if (lctime_argv(args, argv_total)=="error") {
            return 2;
        }else {
            std::string arg=lctime_argv(args, argv_total);
            if (arg=="-v"||arg=="--version") {
                std::cout<<"lctime 1.0.5"<<'\n';
                return 0;
            }else if (arg=="-h"||arg=="--help") {
                std::cout<<"Usage: lctime [OPTION]..."<<'\n'
                         <<"Print the total time since the system was booted."<<'\n'
                         <<'\n'
                         <<"  -v, --version     output version information and exit"<<'\n'
                         <<"  -h, --help        display this help and exit"<<'\n'
                         <<"  -c, --conf  +configuration items or configuration items==true/false      output configuration information and exit"<<'\n';
                return 0;
            }else if (arg=="-c"||arg=="--conf") {
                int conf_count{0};
                 ++conf_count;
                for(const auto& arg:args){
                    if (arg=="-c"||arg=="--conf") {
                        break;
                    }else{
                        std::cerr<<"lctime: invalid option '"<<arg<<"'"<<'\n';
                        return 2;
                    }
                   
                }
                if(argv[conf_count+1]==nullptr) {
                    std::cerr<<"lctime: option requires an argument --'-c'"<<'\n';
                    return 2;
                }else if(argv[conf_count+1]!=nullptr) {
                    std::string conf_argv=argv[conf_count+1];
                    if (conf_argv=="TIME_NS") {
                        
                        bool conf = lctime_conf_r("TIME_NS=true");
                        if(conf) {
                            std::cout<<"TIME_NS=true"<<'\n';
                        }else{
                            std::cout<<"TIME_NS=false"<<'\n';
                        }
                        return 0;
                    }else{
                        if(!lctime_conf_argv(conf_argv,conf_total)) {
                            return 2;
                        }else{
                            bool conf_written = lctime_conf_w(argv[conf_count+1]);
                            if (!conf_written) {
                                std::cerr<<"lctime: failed to write configuration"<<'\n';
                                return 1;
                            }
                            bool conf = lctime_conf_r("TIME_NS=true");
                            if(conf) {
                                std::cout<<"TIME_NS=true"<<'\n';
                            }else{
                                std::cout<<"TIME_NS=false"<<'\n';
                            }
                        return 0;
                        }
                    }                
                }         
            }
        }
    }  
    
    std::ifstream time_last_f("/var/lib/lctime/lctime");
   
   
    bool conf = lctime_conf_r("TIME_NS=true");
   
    uint64_t time_last{0},time_now{0},time_total{0},time_hour{0},time_minute{0},time_day{0},time_second{0},time_now_ns{0},time_ns{0},time_ms{0},time_μs{0},time_last_ns{0},time_total_ns{0};
    
    
    std::string cmd="pgrep -x lctime_core >/dev/null 2>&1";    /*获取后台服务状态*/
    bool core;
    core=(system(cmd.c_str())==0);

    
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
        std::cout<<"lctime_core is "<<"active"<<'\n';
    }else {
        
        std::cout<<"lctime_core is "<<"inactive"<<'\n';
    }

    return 0;
}