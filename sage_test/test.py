import re
import math

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
        result = int(shiftif_tmp[7], 16)
        tmp = math.gcd(int(shiftif_tmp[2], 16), int(shiftif_tmp[4], 16))
        if(result != tmp):
            False_count += 1
            k.write(shiftif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - False_count} / 실패 횟수 : {False_count}\n")

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
        f = file_open('./test.txt')
        if f != None:   bi_test(f, p)

        f = file_open('./test_add.txt')
        if f != None:   test_addition(f, p)

        f = file_open('./test_sub.txt')
        if f != None:   test_subtraction(f, p)

        f = file_open('./test_mul.txt')
        if f != None:   test_multiplication(f, p)

        f = file_open('./test_mul_karachuba.txt')
        if f != None:   test_karachuba_multiplication(f, p)

        f = file_open('./test_div.txt')
        if f != None:   test_division(f, p)

        f = file_open('./test_squ.txt')
        if f != None:   test_squ(f, p)

        f = file_open('./test_squ_karachuba.txt')
        if f != None:   test_karachuba_squ(f, p)

        f = file_open('./test_exp.txt')
        if f != None:   test_exp(f, p)

        f = file_open('./test_barrett_reduction.txt')
        if f != None:   test_barrett(f, p)

        f = file_open('./test_gcd.txt')
        if f != None:   test_gcd(f, p)
