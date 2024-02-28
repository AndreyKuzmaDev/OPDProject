import re


def del_marks(s):
    return re.sub(r'[^\w\s.]', ' ', s)


def del_exp(s):
    s = s.split('/')[0].split('\\')[0]
    return re.sub(r'\(.*\)', '', s)


def del_spaces(s):
    return s.lstrip().rstrip()
