#include <iostream>
#include "transdata.h"
using namespace std;
vector<Transdata> user_tran;
void *athread(void *ptr)
{
    int count = 0;
    int num = *(int *)ptr;
    //初始化
    while((user_tran[num].Transdata_init(num))<0)
    {
        cout << "init error "<< endl;
    }
    cout <<"My UserId is :"<< num << endl;
    //do something you want
    user_tran[num].Transdata_Recdata();
    user_tran[num].Transdata_free();
    return 0;
}
int main(int argc, char** argv)
{
    int ret;
    //申请内存  相当于注册
    for(int i = 0; i < 5 ; i++)
    {
        Transdata *p = new Transdata();
        user_tran.push_back(*p);
        user_tran[i].User_ID = i;
        cout << &user_tran[i] << endl;
        delete p;
    }
    //开启五个线程
    for(int i = 0; i < 5; i ++)
    {
        int *num_tran;
        num_tran = &user_tran[i].User_ID;
        ret = pthread_create(&user_tran[i].thread_id,NULL,athread,(void *)num_tran);
        if(ret < 0) return -1;
    }
    for(int i = 0; i < 5; i++)
    {
        pthread_join(user_tran[i].thread_id, NULL);/*等待进程t_a结束*/
    }
    return 0;
}
