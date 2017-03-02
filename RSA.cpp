#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int randomG();
long Inverse(long a,long b);//Extended Euclidean Algorithm
long squareM(long long int m,long e, long n);//Square and multiply algorithm for modular exponentiation
long gcd(long c, long d);
bool millerRabin(long n);//Miller-Rabin Primarlity test
long encription(long long int M,long a,long b);//encrption
long dencription(long long int M,long a,long b);//decrption

int main(){
    
    int p=0, q=0;//소수 랜덤 넘버 2개
    long n; //토션트 값
    long e,d,phi;
    printf("making prime p.... please wait\n");
    do{
        p = (int)randomG(); //prime p 생성하기
    }while(!millerRabin(p));
    printf("p = %d \n",p);
    
    printf("making prime q.... please wait\n");
    do{
        q = (int)randomG(); //prime q 생성하기
        if(p==q) // prime 큰 키를 2개 만들때 중복되면 초기화 시켜주기.
            q = 0;
    }while(!millerRabin(q));
    printf("q = %d \n",q);
    n = (p-1)*(q-1);//토션트 값 계산
    phi = p*q;
    e = 257;
//docx 문서 예제 값 테스트
//    p = 17293;
//    q = 19051;
//    e = 17;
//    n = (p-1)*(q-1);//토션트 값 계산
//    phi = p*q;
    
    
    d = Inverse(n, e);
    printf("N = %ld\n",n);
    printf("phi = %ld\n",phi);
    printf("e = %ld\n",e);
    printf("d = %ld\n",d);
    int M;
    printf("Message Input : ");
    scanf("%d",&M);
    printf("Message = %d\n",M);
    long C=encription(M, e, phi);
    long P=dencription(C, d, phi);
    printf("**Encrption\ncipher = %ld\n**Decrption\ndecrpted cipher : %ld\n",
           C,P);
           
}



int randomG(){
    //15 비트의 랜덤 수 생성
    srand((unsigned)time(NULL));
    int randNo;
    randNo = rand()%16384 + 16384; //15비트의 값을 생성시키기 위함
    return randNo;
}

long Inverse(long a,long b){//확장 유클리드 역원을 구하기 위해서 사용
    long r1,r2,q = 0,r,s,s1=1,s2=0,t,t1=0,t2=1;
    if(a>b){
        r1 = a;
        r2 = b;
    }else if(b>a){
        r1 = b;
        r2 = a;
    }
    long tmp=r1;
    while(r2){
        q = r1/r2;
        r = r1 % r2;
        s = s1 - q * s2;
        t = t1 - q * t2;
        
        r1 = r2;
        r2 = r;
        s1 = s2;
        s2 = s;
        t1 = t2;
        t2 = t;
    }
    if(r1 == 1) //역원이 있을 경우 1 + tmp 를 해줌
    {
        if(t1 < 0)
            t1 += tmp;
        return t1;
    }
    return 0;
}

long squareM(long long int m,long e, long n){ //Square and multiply algorithm for modular exponentiation
    long f = 1;
    
    while( e > 0 )
    {
        while( e%2 == 0 ) // e를 0이 될때까지 반복시킴 , 1이 되기 전까지 계속 square 됨
        {
            e /= 2;
            m = (m*m)%n;  // Square
        }
        e--;
        f = (f*m) % n;  // Square and multiply 부분 1이 발견되면 수행해줌
    }
    return f;
}

long gcd(long c, long d){ //a와 b의 최대공약수를 구하는 함수
    long r;
    long a,b;
    if(c>d){
        a=c;
        b=d;
    }else{
        a=d;
        b=c;
    }
    while(b>0)
    {
        r = a % b;
        a = b;
        b = r;
    }
    return a;
}

bool millerRabin(long n){
    long long int a,b;
    long m = n-1;
    long k = 0;
    long i,j;
    if(n == 1 || n == 2) //1과 2는 그냥 소수 처리 해줌
        return true;
    
    while(m%2 == 0){
        m=m/2;
        k++;
    }
    for(i=0; i<20; i++){
        srand((int)time(NULL));
        a = rand()%(n-1) + 2;// a는 랜덤 숫자 인데 1 < a < n-1 범위 내의 숫자를 구함
        if(gcd(a,n)!=1) // 공약수가 존재 하면 소수가 아님
        {
            return false;
        }
        b=squareM(a, m, n); // b를 스퀘어 & 멀티플라이 처리해줌
        if(b==1 || b == (n-1)){ // 처리된 b가 1거나 n-1과 같다면
            continue; //유사소수 이다.
        }else{
            for(j=0; j<k-1; j++){
                b=(b*b)%n;
                if(b==(n-1))//유사소수 이다.
                    break;
            }
            //소수가 아닌 경우
            if(b!=(n-1)){
                return false;
            }
        }
        
    }
    return true;
    
}

long encription(long long int M,long a,long b){
    return squareM(M,a,b); //Cipher = M^a mod b
}

long dencription(long long int M,long a,long b){
    return squareM(M,a,b); //Plain = C^a mod b
}
