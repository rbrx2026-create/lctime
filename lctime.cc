#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::string lctime_conf_r (const std::string& conf){               //用于检查配置项，若存在则返回该配置项的值，否则返回空字符串
    std::ifstream conf_f("/etc/lctime/lctime.conf");
    if(!conf_f) {
        std::cerr<<"lctime: failed to open /etc/lctime/lctime.conf for reading"<<'\n';
        return "";
    }else{
        std::string conf_str;
        while (std::getline(conf_f, conf_str)) {
              std::erase(conf_str,' ');
            if(!conf_str.starts_with(conf+'=')) {
                continue;
            }else{
                std::string conf_value=conf_str.substr(conf_str.find('=') + 1);
                return conf_value;
            }
        }
    return "";
    }
}
/*
std::string lctime_argv (std::vector<std::string> args,std::string argv_total){             //检查用户输入的参数是否存在，并返回第一个存在的参数，若不存在则返回error   已废弃！！！！
        if (argv_total.find(" "+arg+" ")==std::string::npos) {
            std::cerr<<"lctime: invalid option -- '"<<arg<<"'"<<'\n';
            return "error";
        }
            return arg;
        
    }
    return "error";
}
*/
bool lctime_conf_argv (std::string conf,std::string conf_total){            //检查用户输入的配置项是否存在，若不存在则返回false，存在则返回true
   if (conf_total.find(" "+conf+" ")==std::string::npos) {
        std::cerr<<"lctime: invalid option -- '"<<conf<<"'"<<'\n';
        return false;
    }else if(conf_total.find(" "+conf+" ")!=std::string::npos) {
        return true;
    }
    return false;
   
   }

bool lctime_conf_w(std::string conf,std::string conf_igur){                                       //用于写入配置文件,若配置项存在则修改其值，若不存在则添加该配置项
    std::ifstream conf_f_r_t("/etc/lctime/lctime.conf");    
    std::vector<std::string> conf_vec;
    std::string conf_str;
    while (std::getline(conf_f_r_t, conf_str)) {
        conf_vec.push_back(conf_str);
    }
    conf_f_r_t.close();
    int conf_count{0};
    for (const auto& c:conf_vec) {
       if(c.find(conf)!=std::string::npos) {
            break;
        }
        ++conf_count;
    }
    std::ofstream conf_f_w_t("/etc/lctime/lctime.conf",std::ios::app);
    if(conf_f_w_t){ 
        if((conf_count)<(conf_vec.size())) {
            conf_vec[conf_count]=conf+'='+conf_igur;
            std::ofstream conf_f_w_t("/etc/lctime/lctime.conf");
            for (const auto& c:conf_vec) {
                conf_f_w_t<<c<<'\n';
            }
            conf_f_w_t.close();
            return true;
        }else{
                conf_f_w_t<<conf<<'='<<conf_igur<<'\n';
                conf_f_w_t.close();
                return true;
        }
    }else{
        std::cerr<<"lctime: failed to open /etc/lctime/lctime.conf for writing"<<'\n';
        return false;
    }
    
}


int main(int argc, char* argv[]){

    //std::string argv_total{" -v --version -h --help -c --conf "};           //定义允许的参数  已废弃！！！
    //std::string conf_total{" TIME_NS  TIME_NS=true  TIME_NS=false "};       //定义允许的配置项  已废弃 ！！！
    //std::string conf_total_no_bool{" TIME_NS "};                            //定义允许的配置项（不带bool值） 已废弃！！！
    std::vector<std::vector<std::string>> conf_total {
                                                        {"TIME_NS"},
                                                        {"bool"}
                                                        };
    if (argc>=2) {    
    
        std::vector<std::string> args;              
        for (int i=1;i<argc;i++) {
            args.emplace_back(argv[i]);                //转储参数
        }
       
        for(int args_l=0;args_l<args.size();){
            if (args[args_l]=="-v"||args[args_l]=="--version") {
                std::cout<<"lctime 1.0.5"<<'\n';
                return 0;
            }else if (args[args_l]=="-h"||args[args_l]=="--help") {
                std::cout<<"Usage: lctime [OPTION]..."<<'\n'
                         <<"Print the total time since the system was booted."<<'\n'
                         <<'\n'
                         <<"  -v, --version     output version information and exit"<<'\n'
                         <<"  -h, --help        display this help and exit"<<'\n'
                         <<"  -c, --conf  +configuration items or configuration items==true/false      output configuration information and exit"<<'\n';
                return 0;
            }else if (args[args_l]=="-c"||args[args_l]=="--conf") {
                
                if(args_l+1>=args.size()) {
                    std::cerr<<"lctime: option requires an argument --'-c'"<<'\n';
                    return 2;                                                                       //预防用户输入-c/--conf后没有跟参数的情况
                }else {
                    std::string conf_args=args[args_l+1];
                    if (conf_total_no_bool.find(" "+conf_args+" ")!=std::string::npos) {
                            std::cout<<"conf_args="<<lctime_conf_r(args[args_l+1])<<'\n';
                        return 0;
                    }else{
                        if(!lctime_conf_argv(conf_args,conf_total)) {
                            return 2;
                        }else{
                            bool conf_written = lctime_conf_w(args[args_l+1]);
                            if (!conf_written) {
                                std::cerr<<"lctime: failed to write configuration"<<'\n';
                                return 1;
                            }                                                                         //修改参数后写入配置文件
                            std::string conf = lctime_conf_r();
                            
                            
                        return 0;
                        }
                    }                
                }         
            }else {
                  std::cerr << "lctime: invalid option -- '" << args[args_l] << "'\n"
                            << "Try 'lctime --help' for more information.\n";
                return 3;
            }
        }
    }  
    
    std::ifstream time_last_f("/var/lib/lctime/lctime");
   
   
    std::string conf_value = lctime_conf_r("TIME_NS");
    bool conf = (conf_value == "true");
   
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