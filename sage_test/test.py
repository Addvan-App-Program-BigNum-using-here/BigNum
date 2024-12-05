import re
import math
# from math import pow
import random
from random import randint
import sys
sys.setrecursionlimit(100000)  # 기본값인 1000보다 큰 값으로 변경

def addition(add_str):
    add_str = add_str.split(' ')
    result = int(add_str[4], 16)
    tmp = int(add_str[0], 0) + int(add_str[2], 0)
    if(result != tmp):
        return False, tmp
    return True, tmp

def test_addition(f, p):
    k = open('./result/result_addition.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 덧셈 연산]\n')
    count = 0
    False_count = 0
    while True:
        addif = f.readline()
        if not addif:
            break
        count += 1
        result, tmp = addition(addif)
        if result == False:
            False_count += 1
            k.write(addif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def subtraction(sub_str):
    sub_str = sub_str.split(' ')
    result = int(sub_str[4], 16)
    tmp = int(sub_str[0], 0) - int(sub_str[2], 0)
    if(result != tmp):
        return False, tmp
    return True, tmp

def test_subtraction(f, p):
    k = open('./result/result_subtraction.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 뺄셈 연산]\n')
    count = 0
    False_count = 0
    while True:
        subif = f.readline()
        if not subif:
            break
        count += 1
        result, tmp = subtraction(subif)
        if result == False:
            False_count += 1
            k.write(subif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def multiplicaiton(sub_str):
    sub_str = sub_str.split(' ')
    result = int(sub_str[4], 16)
    tmp = int(sub_str[0], 0) * int(sub_str[2], 0)
    if(result != tmp):
        return False, tmp
    return True, tmp

def test_multiplication(f, p):
    k = open('./result/result_multiplication.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 곱셈 연산]\n')
    count = 0
    False_count = 0
    while True:
        subif = f.readline()
        if not subif:
            break
        count += 1
        result, tmp = multiplicaiton(subif)
        if result == False:
            False_count += 1
            k.write(subif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def string_to_hex(p, string, result, base):
    hex_string = int(string, base)
    result = int(result, 16)
    if hex_string != result:
        return False, hex(hex_string)
    return True, hex(hex_string)

def test_bi_set_from_loop(f, p, k, base):
    count = 0
    False_count = 0
    p.write(f'[{base}진수]\n')
    k.write(f'[{base}진수]\n')
    while True:
        string = f.readline()
        if "----------" in string:
            break
        result = f.readline()
        result_bol, tmp = string_to_hex(p, string, result, base)
        count += 1
        if result_bol == False:
            False_count += 1
            k.write(string)
            k.write(f"올바른 값 : {tmp}\n")
            k.write(f"제시 값   : {result}")
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def test_bi_set_from(f, p):
    k = open('./result/result_set_from.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[문자열을 빅넘으로 변환하기]\n')
    while True:
        biset = f.readline()
        if '[2]' in biset:
            test_bi_set_from_loop(f, p, k, 2)
        elif '[10]' in biset:
            test_bi_set_from_loop(f, p, k, 10)
        elif '[16]' in biset:
            test_bi_set_from_loop(f, p, k, 16)
        elif "----------" in biset:
            break

def test_karachuba_multiplication(f, p):
    k = open('./result/result_multiplication_karachuba.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[카라츄바 빅넘 곱셈 연산]\n')
    count = 0
    False_count = 0
    while True:
        subif = f.readline()
        if not subif:
            break
        count += 1
        result, tmp = multiplicaiton(subif)
        if result == False:
            False_count += 1
            k.write(subif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def division(sub_str):
    sub_str = sub_str.split(' ')
    if 'DIVISION BY ZERO' in sub_str[4]:
        return True, 0, 0
    A = int(sub_str[0], 0)
    B = int(sub_str[2], 0)
    if(A > 0 and B < 0):
        B = -B
        A = -A
    q = A // B
    r = A % B
    result_q = int(sub_str[4][:-1], 16)
    result_r = int(sub_str[5], 16)
    if(result_q != q or result_r != r):
        return False, q, r
    return True, q, r

def test_division(f, p):
    k = open('./result/result_division.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 나눗셈 연산]\n')
    count = 0
    False_count = 0
    while True:
        subif = f.readline()
        if not subif:
            break
        count += 1
        result, q, r = division(subif)
        if result == False:
            False_count += 1
            k.write(subif)
            k.write(str(hex(q)))
            k.write(', ')
            k.write(str(hex(r)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def test_shift(f, p):
    k = open('./result/result_shift.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 시프트 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if "----------" in shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result = int(shiftif_tmp[4], 16)
        if shiftif_tmp[1] == '<<':
            tmp = int(shiftif_tmp[0], 0) << int(shiftif_tmp[2], 0)
        else:
            tmp = int(shiftif_tmp[0], 0) >> int(shiftif_tmp[2], 0)
        if(result != tmp):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def test_get_lower(f, p):
    k = open('./result/result_get_lower.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 and 연산]\n')
    count = 0
    False_count = 0
    while True:
        and_op = 1
        shiftif = f.readline()
        if "----------" in shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result = int(shiftif_tmp[4], 16)
        lower_len = int(shiftif_tmp[2], 0)
        if lower_len % 32 == 0:
            if lower_len == 0:
                and_op = 0
            else:
                lower_len -= 1
        elif lower_len % 32 == 31:
            lower_len -= 1
        for i in range(lower_len):
            and_op = and_op << 1 | 1
        tmp = int(shiftif_tmp[0], 0) & and_op
        if(result != tmp):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def test_cat(f, p):
    k = open('./result/result_cat.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 cat 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if "----------" in shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result = int(shiftif_tmp[4], 16)
        tmp = '0x' + shiftif_tmp[0][2:] + shiftif_tmp[2][2:]
        if(result != int(tmp, 16)):
            False_count += 1
            k.write(shiftif)
            k.write(str(tmp))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def test_squ(f, p):
    k = open('./result/result_seq.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 제곱 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if not shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result = int(shiftif_tmp[4], 16)
        tmp = int(shiftif_tmp[0], 0) * int(shiftif_tmp[2], 0)
        if(result != tmp):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")


def test_karachuba_squ(f, p):
    k = open('./result/result_seq_karachuba.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 카라츄바 제곱 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if not shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result = int(shiftif_tmp[4], 16)
        tmp = int(shiftif_tmp[0], 0) * int(shiftif_tmp[2], 0)
        if(result != tmp):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def test_exp(f, p):
    k = open('./result/result_exp.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 모듈러 지수승 연산]\n')
    count = 0
    False_count_MS = 0
    False_count_R_TO_L = 0
    False_count_L_TO_R = 0
    while True:
        shiftif = f.readline()
        if not shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result_MS = int(shiftif_tmp[6], 16)
        result_R_TO_L = int(shiftif_tmp[8], 16)
        result_L_TO_R = int(shiftif_tmp[10], 16)
        tmp = pow(int(shiftif_tmp[0], 0), int(shiftif_tmp[2], 0), int(shiftif_tmp[4], 0))
        if result_MS != tmp or result_R_TO_L != tmp or result_L_TO_R != tmp:
            if result_MS != tmp:
                False_count_MS += 1
            elif result_R_TO_L != tmp:
                False_count_R_TO_L += 1
            else:
                False_count_L_TO_R += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"[Multiplication and Squaring]\n실행 횟수 : {count} / 성공 횟수 : {count - False_count_MS} / 실패 횟수 : {False_count_MS}\n")
    p.write(f"[Right to Left]\n실행 횟수 : {count} / 성공 횟수 : {count - False_count_R_TO_L} / 실패 횟수 : {False_count_R_TO_L}\n")
    p.write(f"[Left to Right]\n실행 횟수 : {count} / 성공 횟수 : {count - False_count_L_TO_R} / 실패 횟수 : {False_count_L_TO_R}\n")

def test_barrett(f, p):
    k = open('./result/result_barrett_reduction.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 barrett reduction 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if not shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result = int(shiftif_tmp[4], 16)
        tmp = int(shiftif_tmp[0], 0) % int(shiftif_tmp[2], 0)
        if(result != tmp):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def test_gcd(f, p):
    k = open('./result/result_gcd.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 gcd 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if not shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        try:
            result = int(shiftif_tmp[7], 16)
        except (ValueError, IndexError) as e:
            print(f"Error occurred: {e}")
            print(f"shiftif_tmp: {shiftif_tmp}")
            print(f"shiftif_tmp length: {len(shiftif_tmp)}")
        tmp = math.gcd(int(shiftif_tmp[2], 16), int(shiftif_tmp[4], 16))
        if(result != tmp):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def EEA(a, b):
    if b == 0:
        return a, 1, 0  # gcd(a, 0) = a, x = 1, y = 0
    else:
        gcd, x1, y1 = EEA(b, a % b)
        x = y1
        y = x1 - (a // b) * y1
        return gcd, x, y

def test_EEA(f, p):
    k = open('./result/result_EEA.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 EEA 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if not shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        result_gcd = int(shiftif_tmp[7], 16)
        result_x = int(shiftif_tmp[9], 16)
        result_y = int(shiftif_tmp[11], 16)
        temp_gcd, temp_x, temp_y = EEA(int(shiftif_tmp[2],16), int(shiftif_tmp[4],16))
        if((result_gcd != temp_gcd) or (result_x != temp_x) or (result_y != temp_y)):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(temp_gcd)))
            k.write(str(" , "))
            k.write(str(hex(temp_x)))
            k.write(str(" , "))
            k.write(str(hex(temp_y)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def miller_rabin(n, k=10):
    if n < 2: return False
    for p in [2,3,5,7,11,13,17,19,23,29]:
        if n % p == 0: return n == p
    s, d = 0, n-1
    while d % 2 == 0:
        s += 1
        d //= 2
    for _ in range(k):
        x = pow(random.randint(2, n-1), d, n)
        if x == 1 or x == n-1:
            continue
        for _ in range(s-1):
            x = pow(x, 2, n)
            if x == n-1:
                break
        else:
            return False
    return True

def get_l(a):
    cnt = 0
    while a % 2 == 0:
        a = a >> 1
        cnt += 1
    return cnt

def is_composite(n, q, l, a):
    # pow() 함수를 사용하여 모듈러 지수 계산
    a = pow(a, q, n)
    # print(f"a^(q) = {a}")

    if a % n == 1:
        return "NOT Composite"

    for j in range(l):
        # print(f"a^(2^{j}*q) = {a}")
        if a % n == n - 1:
            return "NOT Composite"
        a = pow(a, 2, n)

    return False

def is_prime_by_miller_rabin(n, k):
    l = get_l(n-1)
    q = (n - 1) >> l
    # print(f"l = {l}, q = {q}")

    while k > 0:
        # 2와 n-2 사이의 랜덤 수 선택
        a = randint(2, n-2)
        # print(f"a = {a}")

        ret = is_composite(n, q, l, a)
        if ret == "Composite":
            return False
        k -= 1

    return True

def test_miller_rabin(f, p):
    k = open('./result/result_Miller_Rabin_fail.txt', 'w')
    t = open('./result/result_Miller_Rabin_success.txt', 'w')
    p.write('------------------------------------------------------------\n')
    p.write('[빅넘 Miller Rabin 연산]\n')
    count = 0
    False_count = 0
    while True:
        shiftif = f.readline()
        if not shiftif:
            break
        count += 1
        shiftif_tmp = shiftif.split(' ')
        value = int(shiftif_tmp[1], 16)
        result = shiftif_tmp[0]
        # mr_result = is_prime_by_miller_rabin(value, 10)
        mr_result = miller_rabin(value, 10)
        False_count += write_result(k, t, shiftif, mr_result, result)
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

def write_result(k, t, shiftif, mr_result, result):
    if mr_result == False and 'Composite' in result:
        t.write(shiftif)
        return 0
    elif mr_result == False and 'Probably_Prime' in result:
        k.write(shiftif)
        return 1
    elif mr_result == True and 'Composite' in result:
        k.write(shiftif)
        return 1
    elif mr_result == True and 'Probably_Prime' in result:
        t.write(shiftif)
        return 0

def bi_test(f, p):
    while True:
        line = f.readline()
        if not line:
            break
        if 'BI SET FROM' in line:
            test_bi_set_from(f, p)
        elif '[Shift]' in line:
            test_shift(f, p)
        elif '[Get Lower]' in line:
            test_get_lower(f, p)
        elif '[Cat]' in line:
            test_cat(f, p)

def file_open(file_name):
    try:
        f = open(file_name, 'r')
        return f
    except:
        # print(f'{file_name} 파일을 찾을 수 없습니다.')
        return None

if __name__ == '__main__':
    with open('./main_result.txt', 'w') as p:
        f = file_open('./test_vec/test.txt')
        if f != None:   bi_test(f, p)

        f = file_open('./test_vec/test_add.txt')
        if f != None:   test_addition(f, p)

        f = file_open('./test_vec/test_sub.txt')
        if f != None:   test_subtraction(f, p)

        f = file_open('./test_vec/test_mul.txt')
        if f != None:   test_multiplication(f, p)

        f = file_open('./test_vec/test_mul_karachuba.txt')
        if f != None:   test_karachuba_multiplication(f, p)

        f = file_open('./test_vec/test_div.txt')
        if f != None:   test_division(f, p)

        f = file_open('./test_vec/test_squ.txt')
        if f != None:   test_squ(f, p)

        f = file_open('./test_vec/test_squ_karachuba.txt')
        if f != None:   test_karachuba_squ(f, p)

        f = file_open('./test_vec/test_exp.txt')
        if f != None:   test_exp(f, p)

        f = file_open('./test_vec/test_barrett_reduction.txt')
        if f != None:   test_barrett(f, p)

        f = file_open('./test_vec/test_gcd.txt')
        if f != None:   test_gcd(f, p)

        f = file_open('./test_vec/test_EEA.txt')
        if f != None:   test_EEA(f, p)

        f = file_open('./test_vec/test_miller_rabin.txt')
        if f != None:   test_miller_rabin(f, p)
