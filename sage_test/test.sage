def addition(add_str):
    add_str = add_str.split(' ')
    result = int(add_str[4], 16)
    tmp = int(add_str[0], 16) + int(add_str[2], 16)
    if(result != tmp):
        return false, tmp
    return true, tmp

with open('./test.txt', 'r') as f:
    with open('./result.txt', 'w') as p:
        if '[Addition]' in f.readline():
            while True:
                addif = f.readline()
                if addif == ' ':
                    break
                result, tmp = addition(addif)
                if result == false:
                    p.write(addif)
                    p.write(str(hex(tmp)))
                    p.write('\n')