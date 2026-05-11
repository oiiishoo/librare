#include <iostream>


int case1(int data) {

    return data-1;
}
int case2(int data) {

    return 2+data;
}

int main()
{
    int i = 1;
    int (*a[2])(int);
    a[0] = case1;
    a[1] = case2;
    std::cout << "Case " <<(a[i](0)) << "Hello World!\n";
} 
