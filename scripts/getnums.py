def get_all(s):
    res = []
    num = ''
    for char in s:
        if char.isdigit():
            num = num + char
        elif (char == ',' or char == '.') and not ('.' in num):
            num += '.'
        else:
            if num != '' and num != '.':
                res.append(float(num))
                num = ''
    if num != '' and num != '.':
        res.append(float(num))

    if len(res) == 0:
        return [0]
    return res


def get_max(s):
    return max(get_all(s))


def get_average(s):
    temp = get_all(s)
    return sum(temp) / len(temp)
