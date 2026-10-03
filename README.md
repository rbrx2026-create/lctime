一个用于查看通电时间的程序

1.启用并启动后台服务（仅需首次执行）
   sudo systemctl enable --now lctime_core

2.查看运行时间
   lctime

3.查看版本 
   lctime -v

4.配置文件
   配置文件在/etc/lctime/lctime.conf
   目前有TIME_NS配置项，默认为false,用于控制输出时间精度;