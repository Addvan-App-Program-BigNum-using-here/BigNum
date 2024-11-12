import re

def addition(add_str):
    add_str = add_str.split(' ')
    result = int(add_str[4], 16)
    tmp = int(add_str[0], 0) + int(add_str[2], 0)
    if(result != tmp):
        return false, tmp
    return true, tmp

def test_addition(f, p):
    k = open('./result/result_addition.txt', 'w')
    p.write('[Addition]\n')
    count = 0
    false_count = 0
    while True:
        addif = f.readline()
        if "----------" in addif:
            break
        count += 1
        result, tmp = addition(addif)
        if result == false:
            false_count += 1
            k.write(addif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - false_count} / 실패 횟수 : {false_count}\n")

def subtraction(sub_str):
    sub_str = sub_str.split(' ')
    result = int(sub_str[4], 16)
    tmp = int(sub_str[0], 0) - int(sub_str[2], 0)
    if(result != tmp):
        return false, tmp
    return true, tmp

def test_subtraction(f, p):
    k = open('./result/result_subtraction.txt', 'w')
    p.write('[Subtraction]\n')
    count = 0
    false_count = 0
    while True:
        subif = f.readline()
        if "----------" in subif:
            break
        count += 1
        result, tmp = subtraction(subif)
        if result == false:
            false_count += 1
            k.write(subif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - false_count} / 실패 횟수 : {false_count}\n")

def multiplicaiton(sub_str):
    sub_str = sub_str.split(' ')
    result = int(sub_str[4], 16)
    tmp = int(sub_str[0], 0) * int(sub_str[2], 0)
    if(result != tmp):
        return false, tmp
    return true, tmp

def test_multiplication(f, p):
    k = open('./result/result_multiplication.txt', 'w')
    p.write('[Multiplication]\n')
    count = 0
    false_count = 0
    while True:
        subif = f.readline()
        if "----------" in subif:
            break
        count += 1
        result, tmp = multiplicaiton(subif)
        if result == false:
            false_count += 1
            k.write(subif)
            k.write(str(hex(tmp)))
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - false_count} / 실패 횟수 : {false_count}\n")

def string_to_hex(p, string, result, base):
    hex_string = int(string, base)
    result = int(result, 16)
    if hex_string != result:
        return false, hex(hex_string)
    return true, hex(hex_string)

def test_bi_set_from_loop(f, p, k, base):
    count = 0
    false_count = 0
    p.write(f'[{base}진수]\n')
    k.write(f'[{base}진수]\n')
    while True:
        string = f.readline()
        if "----------" in string:
            break
        result = f.readline()
        result_bol, tmp = string_to_hex(p, string, result, base)
        count += 1
        if result_bol == false:
            false_count += 1
            k.write(string)
            k.write(f"올바른 값 : {tmp}\n")
            k.write(f"제시 값   : {result}")
            k.write('\n')
    p.write(f"실행 횟수 : {count} / 성공 횟수 : {count - false_count} / 실패 횟수 : {false_count}\n")

def test_bi_set_from(f, p):
    k = open('./result/result_set_from.txt', 'w')
    p.write('\n[BI SET FROM]\n')
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

def bi_test(f):
    with open('./main_result.txt', 'w') as p:
        p.write('[Test result]\n')
        while True:
            line = f.readline()
            if 'BI SET FROM' in line:
                test_bi_set_from(f, p)
            elif '[Addition]' in line:
                test_addition(f, p)
            elif '[Subtraction]' in line:
                test_subtraction(f, p)
            elif '[Multiplication]' in line:
                test_multiplication(f, p)
            elif '[TEST CASE END]' in line:
                break

        p.write('[End Test]\n')

if __name__ == '__main__':
    with open('./test.txt', 'r') as f:
        bi_test(f)
