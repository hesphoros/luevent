在使用Google test框架对lu_mm_pool模块进行单元测试和
由于:
    1.在使用原生malloc frees时page_error过多,导致程序被kill
    2.由于debug程序链接了AddressSanitizer UndefinedBehaviorSanitizer导致程序运行时间过长,未能进行有效的测试，单独剥离了lu_mm_pool模块进行测试