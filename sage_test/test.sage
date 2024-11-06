import re

def addition(add_str):
    add_str = add_str.split(' ')
    result = int(add_str[4], 16)
    tmp = int(add_str[0], 16) + int(add_str[2], 16)
    if(result != tmp):
        return false, tmp
    return true, tmp

def test_addition(f, p):
    p.write('[Addition]\n')
    while True:
        addif = f.readline()
        if addif == '\n':
            break
        result, tmp = addition(addif)
        if result == false:
            p.write(addif)
            p.write(str(hex(tmp)))
            p.write('\n')

def subtraction(sub_str):
    sub_str = sub_str.split(' ')
    result = int(sub_str[4], 16)
    tmp = int(sub_str[0], 16) - int(sub_str[2], 16)
    if(result != tmp):
        return false, tmp
    return true, tmp

def test_subtraction(f, p):
    p.write('[Subtraction]\n')
    while True:
        subif = f.readline()
        print(subif)
        if subif == '\n':
            break
        result, tmp = subtraction(subif)
        if result == false:
            p.write(subif)
            p.write(str(hex(tmp)))
            p.write('\n')

def string_to_hex(p, string, result, base):
    hex_string = int(string, base)
    result = int(result, 16)
    if hex_string != result:
        return false, hex(hex_string)
    return true, hex(hex_string)

def test_bi_set_from(f, p):
    p.write('[BI SET FROM]\n')
    while True:
        biset = f.readline()
        if '[2]' in biset or '[10]' in biset or '[16]' in biset:
            p.write(biset)
            base = re.findall(r'\d+', biset)[0]
            string = f.readline()
            result = f.readline()
            result_bol, tmp = string_to_hex(p, string, result, int(base))
            if result_bol == false:
                p.write(string)
                p.write(f"올바른 값 : {tmp}\n")
                p.write(f"제시 값   : {result}")
                p.write('\n')
        elif biset == '\n':
            break

def bi_test(f):
    with open('./result.txt', 'w') as p:
        p.write('[Test result]\n')
        while True:
            line = f.readline()
            if 'BI SET FROM' in line:
                test_bi_set_from(f, p)
            elif '[Addition]' in line:
                test_addition(f, p)
            elif '[Subtraction]' in line:
                test_subtraction(f, p)
            elif '[TEST CASE END]' in line:
                break

        p.write('[End Test]\n')

if __name__ == '__main__':
    with open('./test.txt', 'r') as f:
        bi_test(f)
