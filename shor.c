/*
 * Shorのアルゴリズム
 * 想定入力: N = p*q (p, qは素数)
 * 1. 正の合成数Nと互いに素な数a(1 < a < N)をランダムに選択
 * 2. a^r = 1 (mod N)を満たす最小のr(位数)を見つける  // ここで量子コンピュータを使う
 * 3. rが偶数なら次へ進む，奇数なら1に戻る
 * 4. x ≡ a^(r/2) (mod N)を定義する
 * 5. x + 1 not= 0 (mod N)ならば次へ進む，満たさなければ1へ戻る
 * 6. Nを作っている2つの素数は，{p, q} = {gcd(x+1, N), gcd(x-1, N)}
 * 
 * aがマイナスになってはいけないがなってしまうことがある
 * mod_pow関数で弾けるようにしてある
*/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
 
#define DEV_RANDOM "/dev/urandom"
#define BUF 32
 
int get_random (char * const buf, const int buflen, const int len);
int isPrimeNum(unsigned int n);
unsigned long long mod_pow(unsigned long long x, unsigned long long n, unsigned long long m);
int gcd(int x, int y);
 
int main()
{
    int N;  // 素因数分解される数を入れる
    char buf[BUF];  // 乱数を入れる配列
    int len = 20;  // 乱数を読み込む文字列の長さ
    unsigned int a = 0;
    int r = 1;
    int x = 0;
    int p = 0;  // Nの素因数
    int q = 0;  // Nの素因数
 
    do {
        printf("素因数分解する値を入力: ");
        scanf("%d", &N);
    }while(N < 2);  // 2より大きい数のみの入力を許可
 
    // 素数ならプログラム終了
    if(isPrimeNum(N)) {
        printf("%dは素数です\n", N);
        exit(EXIT_SUCCESS);
    }
 
    for(int i = 0; ; i++) {
        if(i == 10*N) {
            printf("%d回ループを行いましたが素因数分解できませんでした\n", i);
            exit(EXIT_SUCCESS);
        }
        r = 1;
        // 1. 正の合成数Nと互いに素な数a(1 < a < N)をランダムに選択
        printf("1.を実行\n");
        do {
            if(get_random(buf, BUF, len) == -1) continue;  // 乱数を取得し配列bufに入れる/失敗したら-1
            int random = 0;
            for(int j = 0; j < BUF; j++) {
                random += (unsigned int)buf[j];  // 乱数をint型にして合計値を取る
            }
            random = 2*random - random;
            a = random%N;  // N未満の乱数値を生成
        }while(a <= 1 || gcd(N, a) != 1);
        printf("a = %d\n", a);
 
        // 2. a^r = 1 (mod N)を満たす最小のr(位数)を見つける  // ここで量子コンピュータを使う
        printf("2.を実行\n");
        while(mod_pow(a, r, N) != 1) r++;
        printf("r = %d\n", r);
 
        // 3. rが偶数なら次へ進む，奇数なら1に戻る
        printf("3.を実行\n");
        if((r%2) != 0) {
            printf("rが奇数なので1.へ戻る\n");
            continue;
        }
 
        // 4. x ≡ a^(r/2) (mod N)を定義する
        printf("4.を実行\n");
        x = mod_pow(a, r/2, N);
        printf("x = %d\n", x);
 
        // 5. x + 1 not= 0 (mod N)ならば次へ進む，満たさなければ1へ戻る
        printf("5.を実行\n");
        int y = (x + 1)%N;
        printf("%d + 1 = %d (mod %d)\n", x, y, N);
        if(y == 0) {
            printf("あと一歩\n");
            printf("---------------------------------\n");
            continue;
        }
        break;
    }
    printf("6.を実行\n");
    p = gcd(x+1, N);  // x+1とNの最大公約数がp
    q = gcd(x-1, N);  // x-1とNの最大公約数がq
 
    printf("N = %d = p*q = %d*%d\n", N, p, q);
 
    exit(EXIT_SUCCESS);
}
 
// /dev/urandomから乱数を取得する関数
// 引用:https://kaworu.jpn.org/security/%E4%B9%B1%E6%95%B0
int get_random (char * const buf, const int buflen, const int len)
{
    if (len > buflen) {
        warnx ("buffer size is small (%d / %d)", buflen, len);
        return -1;
    }
    int fd = open(DEV_RANDOM, O_RDONLY);
    if (fd == -1) {
        warn ("can not open %s", DEV_RANDOM);
        return -1;
    }
    int r = read (fd, buf, len);
    if (r < 0) {
        warn ("can not read");
        return -1;
    }
 
    if (r != len) {
        warnx ("can not read(%d != %d)", r, len);
        return -1;
    }
 
    (void) close (fd);
    return 0;
}
 
// 素数判定をするプログラム
// 素数でない時0を返す
// 素数の時1を返す
int isPrimeNum(unsigned int n)
{
    // 入力が2未満の時素数ではない
    if(n < 2) return 0;
 
    for(int i = 2; i < n; i++) {
        if(n%i == 0) return 0;  // 割り切れる場合は素数ではない
    }
 
    return 1;
}
 
// x^n (mod m)を計算する関数
unsigned long long mod_pow(unsigned long long x, unsigned long long n, unsigned long long m)
{
    unsigned long long result = 1;
    printf("%llu^%llu (mod %llu): ", x, n, m);
    if(x > m) return 1;
    while (n > 0){
        if (n % 2 == 1) result = (result * x) % m;
        x = (x * x) % m;
        n = n / 2;
        //printf("n=%llu\n", n);
    }
    printf("%llu\n", result);
 
    return result;
}
 
// ユークリッドの互除法で最大公約数を求める関数
int gcd(int x, int y)
{
    if(y == 0){
        return x;
    }else{
        return gcd(y, x%y);
    }
}