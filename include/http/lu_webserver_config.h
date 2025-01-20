#ifndef LU_EVENT_CONFIG_H
#define LU_EVENT_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

typedef struct lu_webserver_config_s{
    int port;// 端口
    char *host;//主机
    int log_write_mode;//日志写入方式
    //触发组合模式
    int trig_mode;
    //listenfd触发模式
    int listen_trigmode;
    //connfd触发模式
    int conn_trigmode;
    //优雅关闭链接
    int opt_linger;
    //数据库连接池数量
    int sql_num;
    //线程池内的线程数量
    int thread_num;
    //是否关闭日志
    int close_log;
    //并发模型选择
    int actor_model;

}lu_webserver_config_t;


lu_webserver_config_t* lu_new_webserver_config(){
    static lu_webserver_config_t* config;
    config->port = 9006;
    config->log_write_mode = 0;// 默认为同步写入
    config->trig_mode = 0;// 默认为LT模式 默认为水平触发(LD) + connfd使用水平触发
    config->listen_trigmode = 0;
    config->conn_trigmode = 0;
    config->opt_linger = 0;
    config->sql_num = 8;//数据库连接池的数量
    config->thread_num = 8;//线程池内的线程数量 
    config->close_log = 0;//默认开启日志
    config->actor_model = 0;// 设置并发模型为proactor模型，即处理器模型
}

 static lu_webserver_config_t* lu_paser_webserver_config(int argc, char*argv[]){
    static lu_webserver_config_t* config;
    int opt;
    const char *str  = "p:l:t:lm:n:c:a:";
   while ((opt = getopt(argc, argv, str)) != -1)
    {
       switch (opt)
        {
        case 'p':
        {
            config->port = atoi(optarg);
            break;
        }
        case 'l':
        {
            config->log_write_mode = atoi(optarg);
            break;
        }
        case 'm':
        {
            config->trig_mode = atoi(optarg);
            break;
        }
        case 'o':
        {
            config->opt_linger = atoi(optarg);
            break;
        }
        case 's':
        {
            config->sql_num = atoi(optarg);
            break;
        }
        case 't':
        {
            config->thread_num = atoi(optarg);
            break;
        }
        case 'c':
        {
            config->close_log = atoi(optarg);
            break;
        }
        case 'a':
        {
            config->actor_model = atoi(optarg);
            break;
        }
        default:
            break;
        } 
    }
    return config;
    
}

#endif /*LU_EVENT_CONFIG_H*/