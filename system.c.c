// 메모리 설정을 위한 구조체 선언 구역
typedef struct {
    char address[10];
    char value[10];
} Memory;
Memory memoryTable[] = {
    {"3030","003600"},
    {"3600","103000"},
    {"6390","00C303"},
    {"C303","003030"}
};
// ------------------------------
// 변수 선언 구역
char input[100];
char binary[400];
char opcode[9], nixbpe[7], disp[21];
int N,I,X,B,P,E;
int format;
int f3_dispValue[4];
int f4_dispValue[3];
char SIC_SICXE[10];
char addr_mode[25];
int Base = 0x6000; // 예시 006000
int PC = 0x3000; // 예시 003000
int Index = 0x90; // 예시 000090
int TA = 0; // Target Address
int dispValue = 0;

// ------------------------------
// 함수 선언 구역
void hexToBinary(char h, char *out, int idx); 
// HEX → Binary
void opcode_nixbpe_disp(); // opcode, nixbpe, disp 분리
void PrintValue(); // 값 출력
void Format(); // 포맷 결정
void Find_TA(); // 타겟 어드레스 계산
void Find_SIC_SICXE(); // SIC, SIC/XE 판별
char* getMemoryValue(int address); // 메모리 테이블에서 값 가져오기

// ------------------------------
// 메인 함수
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(){
    printf("HEX 입력: ");
    scanf("%s",input);

    int j=0;
    for(int i=0;input[i]!='\0';i++){
        hexToBinary(input[i],binary,j);
        j+=4;
    }
    binary[j]='\0';
    opcode_nixbpe_disp();
    Format();
    Find_SIC_SICXE();
    Find_TA();
    PrintValue();
    return 0;
}
//------------------------------
// 함수 정의 구역
void hexToBinary(char h, char *out, int idx){
    int num;
    if(h>='0'&&h<='9') num=h-'0';
    else if(h>='A'&&h<='F') num=h-'A'+10;
    else num=0;
    for(int k=3;k>=0;k--){
        out[idx+(3-k)]=((num>>k)&1)+'0';
    }
} // HEX → Binary
void opcode_nixbpe_disp(){
    dispValue = 0;

    // 먼저 6비트 opcode와 2비트 (n,i)를 확인
    strncpy(opcode,binary,6);
    opcode[6]='\0';
    N=(binary[6]-'0');
    I=(binary[7]-'0');

    // SIC 모드인지 확인 (N=0, I=0)
    if(N==0 && I==0){
        // SIC 형식: opcode(8비트) + x(1비트) + address(15비트) = 24비트
        strncpy(opcode,binary,8);
        opcode[8]='\0';
        X=binary[8]-'0';
        strncpy(disp,binary+9,15);
        disp[15]='\0';
        B=0; P=0; E=0;
        // nixbpe는 출력용으로만 설정
        sprintf(nixbpe,"%d%d%d000",N,I,X);
    } else {
        // SIC/XE 형식
        strncpy(opcode,binary,6);
        opcode[6]='\0';
        strncpy(nixbpe,binary+6,6);
        nixbpe[6]='\0';
        X=nixbpe[2]-'0';
        B=nixbpe[3]-'0';
        P=nixbpe[4]-'0';
        E=nixbpe[5]-'0';

        if(E==0){
            strncpy(disp,binary+12,12);
            disp[12]='\0';
        }else{
            strncpy(disp,binary+12,20);
            disp[20]='\0';
        }
    }
    // SIC 모드 (15비트)인 경우
    if (N == 0 && I == 0) {
        if (disp[0] == '1') dispValue += 16384;
        if (disp[1] == '1') dispValue += 8192;
        if (disp[2] == '1') dispValue += 4096;
        if (disp[3] == '1') dispValue += 2048;
        if (disp[4] == '1') dispValue += 1024;
        if (disp[5] == '1') dispValue += 512;
        if (disp[6] == '1') dispValue += 256;
        if (disp[7] == '1') dispValue += 128;
        if (disp[8] == '1') dispValue += 64;
        if (disp[9] == '1') dispValue += 32;
        if (disp[10] == '1') dispValue += 16;
        if (disp[11] == '1') dispValue += 8;
        if (disp[12] == '1') dispValue += 4;
        if (disp[13] == '1') dispValue += 2;
        if (disp[14] == '1') dispValue += 1;
    }
    // Format 4 (20비트)인 경우
    else if (E == 1) {
        if (disp[0] == '1') dispValue += 524288;
        if (disp[1] == '1') dispValue += 262144;
        if (disp[2] == '1') dispValue += 131072;
        if (disp[3] == '1') dispValue += 65536;
        if (disp[4] == '1') dispValue += 32768;
        if (disp[5] == '1') dispValue += 16384;
        if (disp[6] == '1') dispValue += 8192;
        if (disp[7] == '1') dispValue += 4096;
        if (disp[8] == '1') dispValue += 2048;
        if (disp[9] == '1') dispValue += 1024;
        if (disp[10] == '1') dispValue += 512;
        if (disp[11] == '1') dispValue += 256;
        if (disp[12] == '1') dispValue += 128;
        if (disp[13] == '1') dispValue += 64;
        if (disp[14] == '1') dispValue += 32;
        if (disp[15] == '1') dispValue += 16;
        if (disp[16] == '1') dispValue += 8;
        if (disp[17] == '1') dispValue += 4;
        if (disp[18] == '1') dispValue += 2;
        if (disp[19] == '1') dispValue += 1;
    } else {
        // Format 3 (12비트)인 경우
        if (disp[0] == '1') dispValue += 2048;
        if (disp[1] == '1') dispValue += 1024;
        if (disp[2] == '1') dispValue += 512;
        if (disp[3] == '1') dispValue += 256;
        if (disp[4] == '1') dispValue += 128;
        if (disp[5] == '1') dispValue += 64;
        if (disp[6] == '1') dispValue += 32;
        if (disp[7] == '1') dispValue += 16;
        if (disp[8] == '1') dispValue += 8;
        if (disp[9] == '1') dispValue += 4;
        if (disp[10] == '1') dispValue += 2;
        if (disp[11] == '1') dispValue += 1;
    }
        
} // opcode, nixbpe, disp 분리
void PrintValue(){
    printf("Binary: %s\n",binary);
    printf("Opcode: %s\n",opcode);
    printf("nixbpe: %s\n",nixbpe);
    printf("N: %d, I: %d, X: %d, B: %d, P: %d, E: %d\n",N,I,X,B,P,E);
    printf("disp: %s\n",disp);
    printf("disp value: %d\n",dispValue);
    printf("Addressing mode: %s\n",addr_mode);
    printf("Format: %d\n",format);
    printf("SIC/SICXE: %s\n",SIC_SICXE);
    printf("Target Address: %04X\n",TA);
    if (N == 0 && I == 1) {
        printf("");
    } else {
        printf("Memory Value at TA: %s\n",getMemoryValue(TA));
    }

} // 출력을 위한 함수
void Format(){
    if(E==0) format=3;
    else format=4;
} // E에 따른 포멧이 결정 되기에 E로 포맷 결정하는 함수 제작
void Find_TA(){
    if (N == 0 && I == 0) {
        format = 3;
        strcpy(SIC_SICXE, "SIC");
        strcpy(addr_mode, "Simple addressing");
        TA = dispValue;
        if (X == 1) TA += Index;
    }
    else if (N == 1 && I == 1) {
        strcpy(addr_mode, "Simple addressing");
        strcpy(SIC_SICXE, "SIC/XE");
        if (B == 1) {
            TA = Base + dispValue;
        } else if (P == 1) {
            TA = PC + dispValue;
        } else {
            TA = dispValue;
        }
        if (X == 1) TA += Index;
    } else if (N == 1 && I == 0) {
        strcpy(addr_mode, "Indirect addressing");
        if (B == 1) {
            // 2의 보수 처리
            if (disp[0] == '1') {
                TA = Base - (4096 - dispValue);
            } else {
                TA = Base + dispValue;
            }
        } else if (P == 1) {
            // 2의 보수 처리
            if (disp[0] == '1') {
                TA = PC - (4096 - dispValue);
            } else {
                TA = PC + dispValue;
            }
        } else {
            TA = dispValue;
        }
        if (X == 1) TA += Index;
    } else if (N == 0 && I == 1) {
        strcpy(addr_mode, "Immediate addressing");
        TA = dispValue;
    };
} // 여러가지의 경우의 수에 따른 TA를 계산하는 함수 또한 음수를 계산 하기에 2의 보수 처리도 같이 진행
void Find_SIC_SICXE(){
    if(N==0 && I==0) strcpy(SIC_SICXE,"SIC");
    else strcpy(SIC_SICXE,"SIC/XE");
} // N과 I에 따른 SIC, SIC/XE 판별 함수
char* getMemoryValue(int address){
    char addrHex[10];
    sprintf(addrHex, "%04X", address);

    for(int i = 0; i < sizeof(memoryTable)/sizeof(Memory); i++){
        if(strcmp(memoryTable[i].address, addrHex) == 0){
            return memoryTable[i].value;
        }
    }
    return "Not Found";
} // 메모리 테이블에서 값 가져오기 함수
//------------------------------