#include<bits/stdc++.h>
using namespace std;
int chang[10][11],num[10];
  //chang[i][j]代表数字i的第j种变法是什么，num[i]代表数字i有多少变法 （这里没包括它自己）所以第37行得加1 
int n[100];
int ans[100];//ans最大是10的30次方 
int k,a,b,len=0;
char x;
bool find(int x,int y){
    for(int i=1;i<=num[x];i++)
        if(chang[x][i]==y) return 1;
    if(x==y) return 1;
    return 0;
}

int main(){
    while(scanf("%c",&x)&&x!=' ') n[++len]=x-'0'; //将字符型转变为真正的数字   
    cin>>k;
    for(int i=1;i<=k;i++){
        cin>>a>>b;
        chang[a][++num[a]]=b;
    }
    
    for(int i=0;i<=9;i++)
           for(int j=1;j<=num[i];j++)
            for(int k=1;k<=num[chang[i][j]];k++)
                if(!find(i,chang[chang[i][j]][k])) 
                    chang[i][++num[i]]=chang[chang[i][j]][k]; 
 

    ans[0]=1;
    for(int i=1;i<=len;i++){
        int now=n[i];
        int yu=0;
        if(num[now]){
            for(int i=0;i<=31;i++){
                ans[i]=ans[i]*(num[now]+1)+yu;
                yu=ans[i]/10;
                ans[i]%=10;
            }
        }
    }
    int flag=0;
    for(int i=31;i>=0;i--)
        if(ans[i]||flag){
            flag=1;
            cout<<ans[i];
        }
    return 0;
}