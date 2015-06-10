#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>



unsigned char binary = 0;

unsigned long long MAX=18446744073709551615; // 2^64 -1

struct LongNumber
{
       unsigned long long size;
       unsigned long long* pointer;
};


// пишем в hex рандомно(16-ичное). копируем-перегоняем в 2-ую сч. Переворачиваем их. Обратно в hex скаладываем(если сумма, например). И результат нужно перегнать из hex в 2-ую-переворачиваем, перевернуть и опять в hex

struct LongNumber loadLongNumber(struct LongNumber a, FILE* longNumFile, const char* longNumFileName)
{
    fclose(longNumFile);
    longNumFile = fopen(longNumFileName, "rb");

    unsigned long long numOfBytes = 0;
    while (getc(longNumFile) != EOF)
    {
        ++numOfBytes;
    }

    a.size = numOfBytes / 8;//кол-во элементв в массиве, т.к 64 бита в ячейке
    if (numOfBytes % 8 != 0)
    {
        ++a.size;
    }

    a.pointer = (unsigned long long*)malloc(a.size * sizeof(unsigned long long));
    if (a.pointer == NULL)
    {
        printf("Error: Unable to allocate memory \n");
        exit(0);
    }
    memset(a.pointer, 0, a.size * sizeof(unsigned long long)); //заполнили массив нулями

    fseek(longNumFile, SEEK_SET, 0);//прыгнули на начало файла

    unsigned long long tmp = 0;//временная x64 ячейка
    unsigned char byte = 0;//байт
    unsigned long long j = 0;
    unsigned long long i=0;
    for (i = 0; i < a.size; ++i)
    {
            do
        {
                 fread(&byte, sizeof(unsigned char), 1, longNumFile);//обязательно unsigned char
             byte = (byte * 0x0202020202ULL & 0x010884422010ULL) % 1023;//для x64 ячейки выполняется переворот битов
                 tmp ^= byte;
                 tmp <<= 8 * (j % 8);//Сдвигаем на 0,8,16,32 бит
                 a.pointer[i] ^= tmp;//записываем в массив
                 tmp = 0;
             ++j;
        }

    while (j % 8 != 0 && j != numOfBytes);

        tmp = a.pointer[i];
        tmp = 0;
        printf("%llu\n",a.pointer[i]);
    }

    fclose(longNumFile);
    return a;
}

void saveLongNumber(struct LongNumber a, FILE* longNumFile)
{
     unsigned long long tmp = 0;
     unsigned char byte = 0;
     unsigned char lastElement[8];
     unsigned char lastNonZeroByte = 0;
     unsigned long long i = 0;
     unsigned char j = 0;

    for (i=0; i < a.size; ++i)
    {
            tmp = a.pointer[i];
            if (i != a.size - 1)
            {
                 for (j=0; j < 8; ++j)
                    {
                        byte = tmp;
                        byte = (byte * 0x0202020202ULL & 0x010884422010ULL) % 1023;
                        fwrite(&byte, sizeof(unsigned char), 1, longNumFile);
                        tmp >>= 8;
                    }
             }
        else  //Чтобы не было "Хвоста из нулей"
         {
            for (j = 0; j < 8; ++j)
        {
                    byte = tmp;
                    byte = (byte * 0x0202020202ULL & 0x010884422010ULL) % 1023;//Аналогично...
                    lastElement[j] = byte;
                    if (byte != 0x00)
                    {
                            lastNonZeroByte = j;
                    }
                    tmp >>= 8;
                }
                unsigned char j = 0;
        for (j = 0; j < lastNonZeroByte + 1; ++j)
                {
                    fwrite(&lastElement[j], sizeof(unsigned char), 1, longNumFile);//Побайтовая забись в файл
                }
        }
    }

    fclose(longNumFile);
}

struct LongNumber lvlUp(struct LongNumber a);

unsigned char isLessOrEqual( struct LongNumber a, struct LongNumber b);

struct LongNumber multiplyOnN(struct LongNumber res, struct LongNumber a, unsigned long long n);

unsigned char isEqual(struct LongNumber a, struct LongNumber b);


struct LongNumber sum(struct LongNumber res, struct LongNumber a, struct LongNumber b);

struct LongNumber subtract(struct LongNumber res, struct LongNumber a, struct LongNumber b);

struct LongNumber multiply(struct LongNumber res, struct LongNumber a, struct LongNumber b);

struct LongNumber divide(struct LongNumber res, struct LongNumber a, struct LongNumber b);

struct LongNumber module(struct LongNumber res,struct LongNumber a, struct LongNumber b);

struct LongNumber power(struct LongNumber res, struct LongNumber a, struct LongNumber b,struct LongNumber c);

struct LongNumber ReadTextFile(const char* file);

void WriteTextFile(const char* file, struct LongNumber number);

struct LongNumber clear(struct LongNumber number);

struct LongNumber allocate(struct LongNumber number, unsigned long long size);

struct LongNumber zero( struct LongNumber number, unsigned long long size);

struct LongNumber Normalize(struct LongNumber a);

struct LongNumber copy(struct LongNumber from);



int main(int argc, const char * argv[])
{   
    if (argc < 5)
    {
        printf("Error: Too few arguments. <first_long_number_filename> [ + (sum) | - (subtract) | * (multiply) | / (divide) | %% (module) | ^ (power) ] <second_long_number_filename> [<third_long_number_filename> (only for power operation) ] <result_long_number_filename> [ -b ]\n");
        return 0;
    }

    const char* operation = argv[2];
    if ((strlen(operation) > 1 || operation[0] == '\0') || operation[0] != '+' && operation[0] != '-' && operation[0] != '*' && operation[0] != '/' && operation[0] != '%' && operation[0] != '^')
    {
        printf("Error: Wrong operation: %s \n", operation);
        return 0;
    }

    if (operation[0] != '^')
    {
             if (argc < 5)
        {
                printf("Error: Too few arguments. <first_long_number_filename> [ + (sum) | - (subtract) | * (multiply) | / (divide) | %% (module) | ^ (power) ] <second_long_number_filename> [<third_long_number_filename> (only for power operation) ] <result_long_number_filename> [ -b ]\n");
                return 0;
            }

             if (argc > 6)
        {
                printf("Error: Too many arguments. <first_long_number_filename> [ + (sum) | - (subtract) | * (multiply) | / (divide) | %% (module) | ^ (power) ] <second_long_number_filename> [<third_long_number_filename> (only for power operation) ] <result_long_number_filename> [ -b ]\n");
             return 0;
            }
        }
    else
    {
            if (argc < 6)
        {
                printf("Error: Too few arguments. <first_long_number_filename> [ + (sum) | - (subtract) | * (multiply) | / (divide) | %% (module) | ^ (power) ] <second_long_number_filename> [<third_long_number_filename> (only for power operation) ] <result_long_number_filename> [ -b ]\n");
                return 0;
            }

            if (argc > 7)
        {
                printf("Error: Too many arguments. <first_long_number_filename> [ + (sum) | - (subtract) | * (multiply) | / (divide) | %% (module) | ^ (power) ] <second_long_number_filename> [<third_long_number_filename> (only for power operation) ] <result_long_number_filename> [ -b ]\n");
                return 0;
            }


        }

    const char* firstLongNumFileName = argv[1];
    FILE* firstLongNumFile = fopen(firstLongNumFileName, "rb");
    if (!firstLongNumFile)
    {
        printf("Error: Unable to open file: %s \n", firstLongNumFileName);
        return 0;
        }

    const char* secondLongNumFileName = argv[3];
    FILE* secondLongNumFile = fopen(secondLongNumFileName, "rb");
    if (!secondLongNumFile)
        {
        printf("Error: Unable to open file: %s \n", secondLongNumFileName);
        return 0;
        }

    const char* thirdLongNumFileName = NULL;
    FILE* thirdLongNumFile = NULL;
    if (operation[0] == '^')
    {
        thirdLongNumFileName = argv[4];
        thirdLongNumFile = fopen(thirdLongNumFileName, "rb");
        if (!thirdLongNumFile)
    {
            printf("Error: Unable to open file: %s \n", thirdLongNumFileName);
            return 0;
        }

    }

    const char* resultLongNumFileName = (operation[0] != '^') ? argv[4] : argv[5];
     FILE* resultLongNumFile = NULL;
    if(binary == 1 )
    {
       resultLongNumFile = fopen(resultLongNumFileName, "wb");
    }
    else
    {

       resultLongNumFile = fopen(resultLongNumFileName, "w+");
    }

    if (!resultLongNumFile)
    {
        printf("Error: Unable to open file: %s \n", resultLongNumFileName);
        return 0;
    }

    const char* ifBinary = (operation[0] != '^') ? argv[5] : argv[6];
    if (argc == 6 && operation[0] != '^' || argc == 7 && operation[0] == '^') 
    {
            if (strcmp(ifBinary, "-b"))
        {
                printf("Error: Invalid flag: %s \n", ifBinary);
                return 0;
            }
        binary = 1;
    }

    struct LongNumber a;
    struct LongNumber b;
    struct LongNumber c;

    if(binary == 1 )
    {
        

        a.size = 0;
        a = loadLongNumber(a, firstLongNumFile, firstLongNumFileName);



         

        b.size = 0;
        b = loadLongNumber(b, secondLongNumFile, secondLongNumFileName);

           

        c.size = 0;
        c.pointer =  NULL;

        if (operation[0] == '^')
        {
            c = loadLongNumber(c, thirdLongNumFile, thirdLongNumFileName);
        }
    }
    else
    {

        a.size = 0;
        
        a = ReadTextFile(firstLongNumFileName);

        

        b.size = 0;
        
        b = ReadTextFile(secondLongNumFileName);

          

        c.size = 0;
        c.pointer =  NULL;

        if (operation[0] == '^')
        {
            
        c = ReadTextFile(thirdLongNumFileName);;
        }
    }

   
   struct LongNumber res;
    res.size = 0;
    res.pointer = NULL;


    switch (operation[0])
    {
            case '+':
            {

                res = sum(res, a, b);
   
                break;
            }
            case '-':
            {
                res= subtract(res, a, b);
                break;
            }
        case '*':
        {
            res = multiply(res,a,b);
            break;
        }
        case '/':
        {
            res = divide(res,a,b);
            break;
        }
        case '%':
        {
            res = module(res,a,b);
            break;
        }
        case '^':
        {
            res = power(res,a,b,c);
            break;
        }
        default:
            break;
        }

   
    if(binary ==1)
        {
        saveLongNumber(res, resultLongNumFile);
        }
    else
    {
        WriteTextFile(resultLongNumFileName, res);
        
    }
   
     free(a.pointer);
     free(b.pointer);
     free(res.pointer);

        return 0;
}


struct LongNumber lvlUp(struct LongNumber a)//Используется в делении и в поиске остатка
{
    struct LongNumber res;

    res.size = a.size + 1;
    res.pointer = (unsigned long long*)malloc(res.size * sizeof(unsigned long long));
    memset(res.pointer, 0, res.size * sizeof(unsigned long long));
    unsigned long long i=0;

    for (i = res.size - 1; i > 0; --i)
    {
        res.pointer[i] = a.pointer[i - 1];// +1 нулевая ячейка размером 64 бита в начало
    }

    a.size = res.size;
    return res;
}


unsigned char isLessOrEqual(struct LongNumber a, struct LongNumber b)//Используеся в вычитании, делении по модулю... Когда подаем массив, а в нем старшие ячейки нулевые(не перевернутого числа)
{

    while (a.pointer[a.size - 1] == 0 && a.size > 1)//Обрезаем нули
    {
        --a.size;
    }

    while (b.pointer[b.size - 1] == 0 && b.size > 1) //Обрезаем нули
    {
        --b.size;
    }

    if (a.size != b.size)//Сравниваем... Если true, то 2-ой больше первого
    {
        return a.size < b.size;
    }
    unsigned long long i=0;

    for ( i= a.size - 1; i < MAX; --i)
    {
            if (a.pointer[i] != b.pointer[i])
        {
                return a.pointer[i] < b.pointer[i];
            }
    }
        return 0;
}


unsigned char isEqual (struct LongNumber a,struct LongNumber b)
{

    while (a.pointer[a.size - 1] == 0 && a.size > 1)
    {
        --a.size;
    }

    while (b.pointer[b.size - 1] == 0 && b.size > 1)
    {
        --b.size;
    }

    if (a.size != a.size)
    {
        return 0;
    }
    unsigned long long i=0;
    for (i = a.size - 1; i < MAX; --i)
    {
            if (a.pointer[i] != b.pointer[i])
        {
                return 0;
            }
    }
        return 1;
}


struct LongNumber multiplyOnN(struct LongNumber res,struct LongNumber a, unsigned long long n)//Умножение на короткое :) LittleEndian- переворачиваем число наоборот( было : 100 001 050 = 001 100 050)
 {

      res.size = a.size + 1;
    res.pointer = (unsigned long long*)malloc(res.size * sizeof(unsigned long long));
     memset(res.pointer, 0, res.size * sizeof(unsigned long long));
        unsigned long long i=0;
        __uint128_t binaryBuffer  = 1;
     for (i = 0; i < res.size - 1; ++i)
    {
            binaryBuffer *= (__uint128_t)a.pointer[i];
            binaryBuffer *= n;
            binaryBuffer += (__uint128_t)res.pointer[i];
            res.pointer[i] = binaryBuffer;
            binaryBuffer >>= 64;
            res.pointer[i + 1] = binaryBuffer;
            binaryBuffer = 1;
    }

    if (res.pointer[res.size - 1] == 0)
    {
        --res.size;
    }

        return res;
}


struct LongNumber sum(struct LongNumber res,struct LongNumber a,struct LongNumber b)
{
    if (a.size > b.size)
    {
        res.size = a.size + 1;
    }
    else
    {
        res.size = b.size + 1;
    }

        res.pointer = (unsigned long long*)malloc(res.size * sizeof(unsigned long long));
        memset(res.pointer, 0, res.size * sizeof(unsigned long long));
        unsigned long long i=0;
        __uint128_t binaryBuffer  = 0;
        for (i = 0; i < res.size - 1; ++i)
        {
            if (a.size > i && b.size > i)
              {
                binaryBuffer = (__uint128_t)res.pointer[i] + (__uint128_t)a.pointer[i] + (__uint128_t)b.pointer[i];
                res.pointer[i] = binaryBuffer;
                binaryBuffer = binaryBuffer >>64;
                res.pointer[i + 1] = binaryBuffer;
                binaryBuffer = 0;
              }
            else
                 {
                 if (a.size > i)
                     {
                    res.pointer[i] += a.pointer[i];
                      }

                     if (b.size > i)
                       {
                        res.pointer[i] += b.pointer[i];
                       }
                 }
     }

    if (res.pointer[res.size - 1] == 0)
    {
        --res.size;
    }
  
    return res;
}


struct LongNumber subtract( struct LongNumber res,struct LongNumber a, struct LongNumber b)
{

    while (a.pointer[a.size - 1] == 0 && a.size > 1)
    {
            --a.size;
    }

    while (b.pointer[b.size - 1] == 0 && b.size > 1)
    {
        --b.size;
    }

        res.size= a.size;
        res.pointer = (unsigned long long*)malloc(res.size * sizeof(unsigned long long));
        memset(res.pointer, 0, res.size * sizeof(unsigned long long));
            unsigned long long i=0;

       for (i = 0; i < res.size; ++i)
        {
            res.pointer[i] = a.pointer[i];
        }

           for (i = 0; i < b.size; ++i)
        {
                if (res.pointer[i] >= b.pointer[i])
            {
                    res.pointer[i] -= b.pointer[i];
                }
                else
            {
                    res.pointer[i] -= b.pointer[i];
                    --res.pointer[i + 1];
                }
            }

    while (res.pointer[res.size - 1] == 0 && res.size > 1) //обрезаем нули
    {
          --res.size;
        }

    return res;
}


struct LongNumber multiply(struct LongNumber res,struct LongNumber a, struct LongNumber b)
{
    res.size = a.size + b.size + 1;
    res.pointer = (unsigned long long*)malloc(res.size * sizeof(unsigned long long));
    memset(res.pointer, 0, res.size * sizeof(unsigned long long));
    unsigned long long i=0;
    unsigned long long j=0;
    __uint128_t binaryBuffer  = 1;
    unsigned long long carry = 0;
    for (i = 0; i < a.size; ++i)
    {
            for (j = 0; j < b.size; ++j)
        {
                binaryBuffer *= (__uint128_t)a.pointer[i];
                binaryBuffer *= (__uint128_t)b.pointer[j];
                binaryBuffer += carry;
                binaryBuffer += (__uint128_t)res.pointer[i + j];
                res.pointer[i + j] = binaryBuffer;
                binaryBuffer >>= 64;
                carry = binaryBuffer;
                binaryBuffer = 1;
                if (j == b.size - 1)
            {
                    res.pointer[i + j + 1] += carry;
            }
        }
            carry = 0;
    }

    while (res.pointer[res.size - 1] == 0 && res.size > 1)
    {
            --res.size;
     }

    return res;
}


struct LongNumber divide(struct LongNumber res,struct LongNumber a, struct LongNumber b)
{
   struct LongNumber CNum;
   struct LongNumber cur;
    res.size = a.size - b.size + 1;
    res.pointer = (unsigned long long*)malloc(res.size * sizeof(unsigned long long));
    memset(res.pointer, 0, res.size * sizeof(unsigned long long));

    CNum.size = 0;//отрезанный кусок от делимомого числа от конца до [i]-ой ячейки включительно
    CNum.pointer = (unsigned long long*)malloc(CNum.size * sizeof(unsigned long long));
    memset(CNum.pointer, 0, CNum.size * sizeof(unsigned long long));

    cur.size = 0;

    unsigned long long i=0;

    for (i = a.size - 1; i < MAX; --i)
    {
            CNum = lvlUp(CNum);// умножаем число на основание системы счисления(нулевая ячейка в начале массива(прям в начале))

            CNum.pointer[0] = a.pointer[i];//Берем от первого большого числа(первый раз-старшая ячейка FirstLongNum(последняя))


            unsigned long long x = 0, l = 0;//x-максимальное число,  которое дает максимально приближенное к делимому(FirstLongNum) число
            __uint128_t r = 0;//Чтобы влезло основание системы счисления
            r = --x;//r=(2^64)-1
            ++x;//x=0
            ++r;//основание СЧ (2^64)

        while (l <= r)
    {
                unsigned long long m = (l + r) >> 1;
                cur = multiplyOnN(cur, b, m);
                if (isLessOrEqual(cur,CNum))
            {
                        x = m;
                        l = m + 1;
                }
            else
        {
                r = m - 1;
            }
        }
        res.pointer[i] = x;//Формируем частное(со старших разрядов)
        cur = multiplyOnN(cur,b, x);//это произведение делителя и x (x*SecondlongNum(которое явл делителем))
        CNum = subtract(CNum,CNum, cur);//На данном шаге получаем остаток, но не возвращаем...
    }

    while (res.pointer[res.size - 1] == 0 && res.size > 1)
    {
        --res.size;
    }


    if (isEqual(CNum, b))
    {
        struct LongNumber tmp;
            tmp.size = 1;
            tmp.pointer = (unsigned long long*)malloc(tmp.size * sizeof(unsigned long long));
            tmp.pointer[0] = 1;
            res = sum(res,res,tmp);
    }



    return res;
}


struct LongNumber module(struct LongNumber res,struct LongNumber a, struct LongNumber b)
{
   struct LongNumber CNum;
   struct LongNumber cur;
    res.size = a.size - b.size + 1;
    res.pointer = (unsigned long long*)malloc(res.size * sizeof(unsigned long long));
    memset(res.pointer, 0, res.size * sizeof(unsigned long long));

    CNum.size = 0;//отрезанный кусок от делимомого числа от конца до [i]-ой ячейки включительно
    CNum.pointer = (unsigned long long*)malloc(CNum.size * sizeof(unsigned long long));
    memset(CNum.pointer, 0, CNum.size * sizeof(unsigned long long));

    cur.size = 0;

    unsigned long long i=0;

    for (i = a.size - 1; i < MAX; --i)
    {
            CNum = lvlUp(CNum);// умножаем число на основание системы счисления(нулевая ячейка в начале массива(прям в начале))

            CNum.pointer[0] = a.pointer[i];//Берем от превого большого числа(первый раз-старшая ячейка FirstLongNum(последняя))


            unsigned long long x = 0, l = 0;//x-максимальное число,  которое дает максимально приближенное к делимому(FirstLongNum) число
            __uint128_t r = 0;//Чтобы влезло основание системы счисления
            r = --x;//r=(2^64)-1
            x=0;//x=0
            ++r;//основание СЧ (2^64)

        while (l <= r)
    {
                unsigned long long m = (l + r) >> 1;
                cur = multiplyOnN(cur, b, m);
                if (isLessOrEqual(cur,CNum))
            {
                        x = m;
                        l = m + 1;
                }
            else
        {
                r = m - 1;
            }
        }
        res.pointer[i] = x;//Формируем частное(со старших разрядов)
        cur = multiplyOnN(cur,b, x);//это произведение делителя и x (x*SecondlongNum(которое явл делителем))
        CNum = subtract(CNum,CNum, cur);//На данном шаге получаем остаток, но не возвращаем...
    }
        int flag = 0;

       if(CNum.size==b.size)
       {
         for(i=0;i<b.size;i++)
        {
            if(CNum.pointer[i]!=b.pointer[i])
            {
                flag=1;

            }

        }

         if(flag==0)
         {
             CNum=zero(CNum,CNum.size);
             CNum=Normalize(CNum);
         }

       }
        return CNum;
}


struct LongNumber power(struct LongNumber res,struct LongNumber a, struct LongNumber b,struct LongNumber c)
{
    struct LongNumber cur;
    struct LongNumber decrement;
    res.pointer = NULL;
    cur.pointer = NULL;
    cur.size = 0;
    decrement.size = 1;
    decrement.pointer = (unsigned long long*)malloc(decrement.size * sizeof(unsigned long long));
    decrement.pointer[0] = 1;

    res = module(res, a, c);

    if (b.size == 1 && b.pointer[0] == 1)
    {
        return res;
    }

    cur = multiply(cur, res, res);
    res = module(res, cur, c);// Алгоритм не симметричен. В случае когда степень больше 2, мы умножаем число на остаток полученный из предыдущего!!! шага и получаем остаток по модулю в n-ую степень

    if (b.size == 1 && b.pointer[0] == 2)
    {
        return res;
    }

    while ((b.pointer[0] - 2) != 0 || b.size != 1)
    {
        cur = multiply(cur, a, res);
        res = module(res, cur, c);
        b = subtract(b, b, decrement);
    }
    return res;
}


struct LongNumber ReadTextFile(const char* file)
{
    struct LongNumber bin, number;
        char ch;

        number.size = 0;

        FILE *in = fopen(file, "r");

        fseek(in, 0, SEEK_END);
        number.size = ftell(in);
    number.size = number.size -1;
        fseek(in, SEEK_SET, 0);

        number = allocate(number, number.size);
        number = zero(number, number.size);

        bin.size = number.size / 16 + 1;

        bin = allocate(bin, bin.size);
        bin = zero(bin, bin.size);

        unsigned long long a, carry = 0, tmp, current, j, x;

        long long i = number.size-1;

        while((ch = getc(in))!=EOF)
        {
            number.pointer[i--] = ch - '0';
            if(i == -1) break;
        }
        fclose(in);

        current = 1;
        j = 0;
        x = 0;
   

        while (number.size != 1 || number.pointer[0]!=0)
        {
            carry = 0;


            for (i = number.size - 1; i >-1; i--)
            {
                tmp = carry * 10 + number.pointer[i];
                number.pointer[i] = tmp / 2;
                carry = tmp - number.pointer[i] * 2;

            }




            number = Normalize(number);

            bin.pointer[j] = ((current << x) * carry) | bin.pointer[j];

            
            x++;

            if(x == 64)
            {
                x = 0;
                j++;
            }
        }
        
        number = clear(number);

        bin = Normalize(bin);
    
        return bin;
    }


void WriteTextFile(const char* file, struct LongNumber number)
{
        long long i =0;
        FILE* out = fopen(file, "w");

        struct LongNumber decimal;

        decimal.size = number.size*17;

        decimal = allocate(decimal, decimal.size);
        decimal = zero(decimal, decimal.size);

        unsigned long long a, j = 0;
        __uint128_t tmp;
        __uint128_t number2=0;
        i = number.size-1;
        char carry =0;
        unsigned long long x=0;
       

        number2 = --x;
        number2++;

        while (number.size != 1 || number.pointer[0]!=0)
            {

            carry = 0;
                                            
            for (i = number.size - 1; i >-1; i--)
            {

                tmp = carry * number2 + (__uint128_t)number.pointer[i];
                number.pointer[i] = tmp / 10;
                carry = tmp - number.pointer[i] * 10;


            }
                                         
            carry += '0';                               
            decimal.pointer[j] = carry;
         
            j++;                          
            number = Normalize(number);
        }

        printf("\n");
        decimal = Normalize(decimal);

        i=0;
        for (i = decimal.size - 1 ; i > -1; i--)
        {
            fprintf(out, "%c" ,decimal.pointer[i]);
        }

        printf("%llu",j);
        fclose(out);
    }



    struct LongNumber clear(struct LongNumber number)
    {
        free(number.pointer);
        return number;
    }


struct LongNumber allocate(struct LongNumber number, unsigned long long size)
{
    number.pointer = (unsigned long long*)malloc(sizeof(unsigned long long)*(size));
    return number;
}

struct LongNumber zero(struct LongNumber number, unsigned long long size)
{
    memset(number.pointer, 0, size * sizeof(unsigned long long));
    return number;
}

struct LongNumber Normalize( struct LongNumber a)
{
    unsigned long long i = a.size-1;

    while ( (i>0) && (a.pointer[i]==0) )
        i--;

    a.size = i+1;

    return a;
}

struct LongNumber copy(struct LongNumber from)
{
    struct LongNumber cpy;

    cpy.size = from.size;
    cpy = allocate(cpy, cpy.size);
    memcpy(cpy.pointer, from.pointer, cpy.size * sizeof(unsigned long long));

    return cpy;
}
