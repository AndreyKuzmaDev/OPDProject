import requests
from bs4 import BeautifulSoup
from measurements import shorts
from getnums import *
from strformat import *
from time import time
import sqlite3


def get_data(source_id):
    print("start 'get_data'", time())
    url = 'https://www.iamcook.ru/showrecipe/' + str(source_id)
    response = requests.get(url)
    print("got response", time())
    soup = BeautifulSoup(response.text, 'lxml')
    name = soup.findAll('h1', itemprop='name')
    if len(name) == 0:
        return False
    name = name[0].text
    category = soup.findAll('a', class_='pathlink')[1].text
    descr_search = soup.findAll('span', itemprop='description')

    if len(descr_search) == 0:
        description = ''
    else:
        description = descr_search[0].text

    quotes = soup.findAll('p', itemprop='recipeIngredient')

    ingredients = []

    for quote in quotes:
        if not ('-' in quote.text):
            continue

        if '–' in quote.text:
            temp = quote.text.split('–', 1)
        else:
            temp = quote.text.split('-', 1)

        if any(ch.isdigit() for ch in temp[0]):
            return False

        element = dict()
        element['ingredient'] = del_spaces(del_exp(temp[0])).capitalize()
        element['amount'] = get_max(temp[1])

        no_marks = del_marks(temp[1])
        for i in shorts.keys():
            if i in no_marks:
                element['unit'] = shorts[i]
                break
        if not ('unit' in element.keys()):
            element['unit'] = 'grams'

        ingredients.append(element)

    output = {
        'id': source_id,
        'name': name,
        'category': category,
        'description': description,
        'url': url,
        'ingredients': ingredients
    }
    print("parsed", time())
    return output


def write(data, cur):

    if len(cur.execute(f"SELECT id FROM Recipes WHERE Name='{data['name']}'").fetchall()) != 0:
        return False

    cur.execute(f"INSERT INTO Recipes(Name, Category, Description, Link) Values ('{data['name']}', '{data['category']}', '{data['description']}', '{data['url']}')")
    recipe_id = cur.execute(f"SELECT id FROM Recipes WHERE Name='{data['name']}'").fetchall()[0][0]

    for i in data['ingredients']:
        req = f"SELECT id FROM Ingredients WHERE Name='{i['ingredient']}' AND Unit='{i['unit']}'"
        t = cur.execute(req).fetchall()
        if len(t) == 0:
            cur.execute(f"INSERT INTO Ingredients(Name, Unit) Values ('{i['ingredient']}', '{i['unit']}')")
        t = cur.execute(req).fetchall()
        ingredient_id = t[0][0]
        cur.execute(f"INSERT INTO Composition(id_recipe, id_ingredient, number) Values ('{recipe_id}', '{ingredient_id}', '{i['amount']}')")

    print("written", time())
    return True


write_try = False
success_cnt = 0
data_failed_cnt = 0
write_failed_cnt = 0
start_point = 3001
number = 10
con = sqlite3.connect('delicious.db')
cur = con.cursor()
for i in range(start_point, start_point+number):
    data = get_data(i)
    if data:
        write_try = write(data, cur)
    else:
        print(f"{i} failed to get data")
        data_failed_cnt += 1
        continue

    if write_try:
        print(f"{data['id']} done successfully!")
        success_cnt += 1
    else:
        print(f"{data['id']} can't be written")
        write_failed_cnt += 1
    con.commit()
    if (i - start_point + 1) % 25 == 0:
        print(f"""{i - start_point + 1} of {number} processed\t{success_cnt} done successfully
{data_failed_cnt} failed to get data\t{write_failed_cnt} failed to write""")
