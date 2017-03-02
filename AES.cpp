#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <bitset>
#include <array>

using namespace std;
unsigned char key_[16];
unsigned char plain_[16];
unsigned char result[16];
unsigned char RC[10];
unsigned char Rkey[10][16];
uint8_t S_box[256];
//기본키 셋팅, 원문
//알씨 셋팅
unsigned short irre_poly4 = 0x165;
//0x65은 x^8 + x^6 + x^5 + x^2 + 1 -> 1 01100101

void key_plain();
//파일에서 key,plain text 불러오기
unsigned char addRoundKey(unsigned char in[],int round);
//arroundkey
unsigned char shiftRow(unsigned char in[]);
//shiftRow
unsigned char mixColumns(unsigned char in[]);
//mixcolumns

unsigned char inShiftRow(unsigned char in[]);
//Inverse shiftRow
unsigned char inMixColumns(unsigned char in[]);
//Inverse mixcolumns
unsigned char inInitialize_aes_sbox(unsigned char num);
//Inverse sbox

unsigned char key_expension();
//key expension 만들기
unsigned char initialize_aes_sbox(unsigned char num);
//s-box 만들기
void Rinitialize_aes_sbox(unsigned char S_box[256]);
//reverse s-box 만들기
void initialize_RC(unsigned char RC[10]);
//RC배열 생성하기
void substitution(unsigned char);

unsigned char galois_plus(unsigned char a, unsigned char b);
unsigned char galois_multiple(unsigned short a,unsigned short b);
unsigned char galois_inverse(unsigned short a);

int main(){
    initialize_RC(RC);
    key_plain();
    printf("\n\n<----------------ENCRYPTION--------------->\n\n");
    
    key_expension();
    
    printf("\nRound 0\n");
    addRoundKey(plain_, 0);
    
    for(int cou = 1; cou <= 9; cou++){
    
    printf("\nRound %d\n",cou);
    for(int i = 0; i < 16; i++){
        plain_[i] = initialize_aes_sbox(plain_[i]);
    }
    printf("SB:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    
    shiftRow(plain_);
    printf("SR:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    
    mixColumns(plain_);
    printf("MC:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    addRoundKey(plain_, cou);
    
    }
    
    printf("\nRound 10\n");
    for(int i = 0; i < 16; i++){
        plain_[i] = initialize_aes_sbox(plain_[i]);
    }
    printf("SB:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    
    shiftRow(plain_);
    printf("SR:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    addRoundKey(plain_, 10);
    
    printf("\nCIPHER:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    
    int fd = 0;
    ssize_t nwrite = 0;
    if((fd = open("cipher.bin",O_WRONLY|O_CREAT|O_TRUNC,0x777)) == -1){
        fputs("파일 에러 ! ", stderr);
    }
    if((nwrite = write(fd,plain_,16)) == -1){
        fputs("쓰기 에러 ! ", stderr);
    }
    close(fd);
    
    
    printf("<--------- DECRYPTION --------->\n");
    
    printf("\nRound 0\n");
    addRoundKey(plain_, 10);
    
    
    for(int cou = 9; cou>0; cou--){
    printf("\nRound %d\n",10-cou);
    inShiftRow(plain_);
    printf("SR:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");

    for(int i = 0; i < 16; i++){
        plain_[i] = inInitialize_aes_sbox(plain_[i]);
    }
    printf("SB:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    addRoundKey(plain_, cou);
    
    inMixColumns(plain_);
    printf("MC:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    }
    
    printf("\nRound 10\n");
    inShiftRow(plain_);
    printf("SR:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    for(int i = 0; i < 16; i++){
        plain_[i] = inInitialize_aes_sbox(plain_[i]);
    }
    printf("SB:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    addRoundKey(plain_, 0);
    
    printf("\nDECRYPTED:");
    for(int i = 0; i<16; i++)
        printf(" %02X",plain_[i]);
    printf("\n");
    
    fd = 0;
    nwrite = 0;
    if((fd = open("decrypt.bin",O_WRONLY|O_CREAT|O_TRUNC,0x777)) == -1){
        fputs("파일 에러 ! ", stderr);
    }
    if((nwrite = write(fd,plain_,16)) == -1){
        fputs("쓰기 에러 ! ", stderr);
    }
    close(fd);
    
    
    
    return 0;
}

unsigned char galois_plus(unsigned char a, unsigned char b){
    unsigned char res;
    res = a ^ b;
    printf("%02X",res);
    return res;
}

unsigned char galois_multiple(unsigned short a,unsigned short b){
    unsigned short there = 0;
    //곱하기를 XOR 사용해야함
    if(a == 0 || b == 0){
        //printf("%d",0);
        return 0;
    }else{
        for(int k = 7; k >= 0; k--){
            if(b & (1 << k)){
                there ^= (a << k);
                {
//                                printf("%d : ",k);
//                                for(int j = 14; j >= 0; j--){
//                                    if(a << k& (1 << j))
//                                        printf("1");
//                                    else
//                                        printf("0");
//                                }
//                                printf("\n");
                } //print each multiple -> b
            }
        }
        {
//                printf("0 : ");
//                for(int j = 14; j >= 0; j--){
//                    if(a & (1 << j))
//                        printf("1");
//                    else
//                        printf("0");
//                }
//                printf("\n");
        }//print last multiple -> b
        {
//                for(int j = 14; j >= 0; j--){
//                        if(temp & (1 << j))
//                            printf("1");
//                        else
//                            printf("0");
//                    }
//                printf("\n");
        }//print result multiple -> b
        
        for(int i = 14; i >= 7; i--){
            //곱했을때 총길이가 최대 15글자수 14 ~ 0까지
            if(there & (1 << i)){
                {
                    //            for(int j = 14; j >= 0; j--){
                    //                //8번째 자리에서 1이 걸리면 XOR
                    //                if(there & (1 << j))
                    //                    printf("1");
                    //                else
                    //                    printf("0");
                    //            }
                    //            printf("\n");
                }//print
                there ^= (irre_poly4 << (i-8));
//                there ^= (0x11b << (i-8));
                //9번째 자리면 그냥 XOR 해주면됨
            }
        }
        {
//            for(int j = 7; j >= 0; j--){
//                if(there & (1 << j))
//                    printf("1");
//                else
//                    printf("0");
//            }
//            printf("\n");
            //    printf("\n%d",temp);
        }//print
    }
    return there;
}

unsigned char galois_inverse(unsigned short a){
    //extended euclidean algorithm
    //irre_poly4
    //0x65은 x^8 + x^6 + x^5 + x^2 + 1 -> 1 01100101
    if(a == 0)
        return 0;
    unsigned short A1,A2,A3,B1,B2,B3,T1,T2,T3,Q = 0;
    A1 = 1; A2 = 0;
    A3 = irre_poly4;
//    A3 = 0x11b;
    B1 = 0; B2 = 1; B3 = a;
    int count = 0;
    
    
    
//    printf("A1 : %d A2 : %d A3 : %d\n",A1,A2,A3);
//    printf("B1 : %d B2 : %d B3 : %d\n",B1,B2,B3);
    while(1){
        
        
//        printpol(A1);
//        printpol(A2);
//        printpol(A3);
//        printpol(B1);
//        printpol(B2);
//        printpol(B3);
//        printf("\n");
//        
        
        
        if(B3 == 1)
            break;
        
        for(int j = 7; j >= 0; j--){
            if(B3 & (1<<j)){
                count = j;
                break;
            }
        }
        unsigned short temp3 = A3;
        Q = 0;
        for(int i = 8; i >= 0; i--){
            if(temp3 & (1 << i)){
                temp3 ^= (B3 << (i-count));
                Q ^= 1 << (i-count);
            }
            if((i-count) == 0)
                break;
        }// 1 round
        
        
        T1 = A1 ^ galois_multiple(Q,B1);
        T2 = A2 ^ galois_multiple(Q,B2);
        T3 = temp3;
        A1 = B1; A2 = B2; A3 = B3;
        B1 = T1; B2 = T2; B3 = T3;
        
    }
    return B2;
}

unsigned char key_expension(){
    unsigned char lastCol[4];
    for(int i = 0; i < 16; i ++)
        Rkey[0][i] = key_[i];
    //round 0의 키
    printf("KEY EXPANSION\n");
    for(int j = 1; j <= 10; j++){
        //맨마지막 줄의 첫번째 칸을 마지막칸으로 옮기고 땡김
        
        lastCol[0] = Rkey[j-1][13];
        lastCol[1] = Rkey[j-1][14];
        lastCol[2] = Rkey[j-1][15];
        lastCol[3] = Rkey[j-1][12];
        
        
        //Sbox 구간
        lastCol[0] = initialize_aes_sbox(lastCol[0]);
        lastCol[1] = initialize_aes_sbox(lastCol[1]);
        lastCol[2] = initialize_aes_sbox(lastCol[2]);
        lastCol[3] = initialize_aes_sbox(lastCol[3]);
        
        //RC는 0부터 9로 설정해 놓음
        lastCol[0] ^= RC[j-1];
        
        for(int p = 0; p < 4; p++){
            Rkey[j][p] = Rkey[j-1][p] ^ lastCol[p];
            Rkey[j][p+4] = Rkey[j][p] ^ Rkey[j-1][p+4];
            Rkey[j][p+8] = Rkey[j][p+4] ^ Rkey[j-1][p+8];;
            Rkey[j][p+12] = Rkey[j][p+8] ^ Rkey[j-1][p+12];;
        }
    }
    
    for(int i = 0; i < 11; i++){
        printf("ROUND %D:",i);
        for(int j = 0; j < 16; j++)
            printf(" %02X",Rkey[i][j]);
        printf("\n");
    }
    
    unsigned char res;
    return res;
}
//key expension 만들기

unsigned char addRoundKey(unsigned char in[],int round){
    for(int i = 0; i<16; i++)
        in[i] ^= Rkey[round][i];
    printf("AR:");
    for(int i = 0; i<16; i++)
        printf(" %02X",in[i]);
    printf("\n");
    return 0;
}
//arroundkey
unsigned char shiftRow(unsigned char in[]){
    //줄을 수정하는게 아니라 칸을 수정하는것
    //0123 05 이런식으로
    //4567 49
    unsigned char temp = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < i; j++){
            temp = in[i + 0];
            in[i + 0] = in[i + 4];
            in[i + 4] = in[i + 8];
            in[i + 8] = in[i + 12];
            in[i + 12] = temp;
        }
    }
    return 0;
}
//shiftRow

unsigned char inShiftRow(unsigned char in[]){
    //줄을 수정하는게 아니라 칸을 수정하는것
    //0123 0 이런식으로
    //4567 41
    unsigned char temp = 0;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < i; j++){
            temp = in[i + 12];
            in[i + 12] = in[i + 8];
            in[i + 8] = in[i + 4];
            in[i + 4] = in[i + 0];
            in[i + 0] = temp;
        }
    }
    return 0;
}

unsigned char mixColumns(unsigned char in[]){
    unsigned char shifted_Row[4][4] = {0};
    unsigned char mixColumnBox[4][4]={
        {0x02,0x03,0x01,0x01},
        {0x01,0x02,0x03,0x01},
        {0x01,0x01,0x02,0x03},
        {0x03,0x01,0x01,0x02}
    };
    int st = 0;
    for(int i = 0; i<4; i++){
        for(int j = 0; j<4; j++){
            shifted_Row[i][j] = in[st++];
        }
    }
    st = 0;
    for (int row = 0; row < 4; row++)
    {
        unsigned char temp[4] = {0,};
        for (int col = 0; col < 4; col++)
        {
            for (int inner = 0; inner < 4; inner++)
            {
                temp[col] ^= galois_multiple(shifted_Row[row][inner],mixColumnBox[col][inner]);
            }
        }
        
        in[st++] = temp[0];            //  가장 마지막에 돌아가는 i의 값의 변화는 mixed_Column 배열의 열의 값만 바뀌는 것이다.
        in[st++] = temp[1];
        in[st++] = temp[2];
        in[st++] = temp[3];
        
    }

    
    return 0;
}
//mixcolumns

unsigned char inMixColumns(unsigned char in[]){
    unsigned char shifted_Row[4][4] = {0};
    unsigned char mixColumnBox[4][4]={
        {0x0E,0x0B,0x0D,0x09},
        {0x09,0x0E,0x0B,0x0D},
        {0x0D,0x09,0x0E,0x0B},
        {0x0B,0x0D,0x09,0x0E}
    };
    int st = 0;
    for(int i = 0; i<4; i++){
        for(int j = 0; j<4; j++){
            shifted_Row[i][j] = in[st++];
        }
    }
    st = 0;
    for (int row = 0; row < 4; row++)
    {
        unsigned char temp[4] = {0,};
        for (int col = 0; col < 4; col++)
        {
            for (int inner = 0; inner < 4; inner++)
            {
                temp[col] ^= galois_multiple(shifted_Row[row][inner],mixColumnBox[col][inner]);
            }
        }
        
        in[st++] = temp[0];            //  가장 마지막에 돌아가는 i의 값의 변화는 mixed_Column 배열의 열의 값만 바뀌는 것이다.
        in[st++] = temp[1];
        in[st++] = temp[2];
        in[st++] = temp[3];
        
    }
    
    
    return 0;
}
//mixcolumns

void key_plain(){
    int fd = 0;
    ssize_t nread = 0;
    if((fd = open("key.bin",O_RDONLY)) == -1){
        fputs("파일 에러 ! ", stderr);
    }
    if((nread = read(fd,key_,16)) == -1){
        fputs("읽기 에러 ! ", stderr);
    }
    close(fd);
    
    nread = 0;
    if((fd = open("plain.bin",O_RDONLY)) == -1){
        fputs("파일 에러 ! ", stderr);
    }
    if((nread = read(fd,plain_,16)) == -1){
        fputs("읽기 에러 ! ", stderr);
    }
    close(fd);
    
    printf("RC: ");
    for(int i = 0; i < 10; i++)
        printf("%02X ",RC[i]);
    printf("\n");
    printf("PLAIN: ");
    for(int i = 0; i<16; i++)
        printf("%02X ",plain_[i]);
    printf("\nKEY: ");
    for(int i = 0; i<16; i++)
        printf("%02X ",key_[i]);
}

void initialize_RC(unsigned char RC[10]){
    RC[0] = 0x01;
    for(int i = 1; i < 10; i ++){
        RC[i] = (RC[i-1] << 1) ^ (RC[i-1] & 0x80 ? 0x65 : 0);
        //x를 곱해서 *2 를 만듦
    }
}

unsigned char initialize_aes_sbox(unsigned char num){
    int box8[8][8] = {
        {1,0,0,0,1,1,1,1},
        {1,1,0,0,0,1,1,1},
        {1,1,1,0,0,0,1,1},
        {1,1,1,1,0,0,0,1},
        {1,1,1,1,1,0,0,0},
        {0,1,1,1,1,1,0,0},
        {0,0,1,1,1,1,1,0},
        {0,0,0,1,1,1,1,1}
    };
    int col8[8]={
        1,1,0,0,0,1,1,0
    };
    
    unsigned char inverGF = 0;
    unsigned char res = 0;
    int inverGFrow[8] = {0};
    int result[8] = {0};
    inverGF = galois_inverse(num);
    
    
//    printf(": %02X\n",inverGF);
    for(int i = 7; i >= 0; i--){
        //숫자를 배열에 하나씩 집어넣기
        if(inverGF & (1<<i))
            inverGFrow[i] = 1;
        else
            inverGFrow[i] = 0;
    }
//    printf("0 : ");
//                    for(int j = 7; j >= 0; j--){
//                        if(inverGF & (1 << j))
//                            printf("1");
//                        else
//                            printf("0");
//                    }
//                    printf("\n");
//    for(int j =7; j>=0; j--)
//        printf("%d",inverGFrow[j]);
//    printf("\n");
    
    
    for(int row = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){
            result[row] ^= box8[row][col] * inverGFrow[col];
            //곱하는 부분
        }
    }
    
    for(int row = 0; row < 8; row++){
        result[row] ^= col8[row];
        //XOR 하는 부분
    }
    
    for(int k = 7; k >= 0; k--){
        if(result[k] == 1)
        {
            res ^= (1 << k);
            //배열을 숫자로 다시 합치기
        }
    }
    
//    printf("%02X",res);
    //결과를 출력해
    return res;
}

unsigned char inInitialize_aes_sbox(unsigned char num){
    //inverse는 암호화 된 숫자를 다시 집어넣고 행렬계산 이후
    //갈로아 곱의 역 함수에 다시 넣어야함
    int box8[8][8] = {
        {0,0,1,0,0,1,0,1},
        {1,0,0,1,0,0,1,0},
        {0,1,0,0,1,0,0,1},
        {1,0,1,0,0,1,0,0},
        {0,1,0,1,0,0,1,0},
        {0,0,1,0,1,0,0,1},
        {1,0,0,1,0,1,0,0},
        {0,1,0,0,1,0,1,0}
    };
    int col8[8]={
        1,0,1,0,0,0,0,0
    };
    
    unsigned char inverGF = 0;
    unsigned char res = 0;
    int inverGFrow[8] = {0};
    int result[8] = {0};
    inverGF = num;
    
    
    //    printf(": %02X\n",inverGF);
    for(int i = 7; i >= 0; i--){
        //숫자를 배열에 하나씩 집어넣기
        if(inverGF & (1<<i))
            inverGFrow[i] = 1;
        else
            inverGFrow[i] = 0;
    }
    //    printf("0 : ");
    //                    for(int j = 7; j >= 0; j--){
    //                        if(inverGF & (1 << j))
    //                            printf("1");
    //                        else
    //                            printf("0");
    //                    }
    //                    printf("\n");
    //    for(int j =7; j>=0; j--)
    //        printf("%d",inverGFrow[j]);
    //    printf("\n");
    
    
    for(int row = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){
            result[row] ^= box8[row][col] * inverGFrow[col];
            //곱하는 부분
        }
    }
    
    for(int row = 0; row < 8; row++){
        result[row] ^= col8[row];
        //XOR 하는 부분
    }
    
    for(int k = 7; k >= 0; k--){
        if(result[k] == 1)
        {
            res ^= (1 << k);
            //배열을 숫자로 다시 합치기
        }
    }
    res = galois_inverse(res);
    //    printf("%02X",res);
    //결과를 출력해
    return res;
}
