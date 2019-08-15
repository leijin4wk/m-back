## 项目介绍


### 这是一个用c语言自己实现的https服务器


整个项目使用了一些优秀的第三方代码（做了必要的裁剪），比如Cjson 和优先级队列，线程池，以及hashmap


整个项目的依赖:


apt-get install libssl-dev


apt-get install libzdb-dev

时至今日基本完成以下:


1.ssl连接的读写:


2.客户端连接超时部分的代码编写（还有瑕疵):

 
3.epool的应用


4.so动态模块的加载和执行（模仿java web servlet)


5.静态文件的读写（mmap实现）（因为不知道sendfile系统调用在ssl下的处理，只能退而求其次）

 
接下来要完成的内容：


1.完成session功能。


2.检查是否有内存泄漏的问题。


                                        
                                                                        
