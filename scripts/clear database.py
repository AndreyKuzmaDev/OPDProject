import sqlite3

con = sqlite3.connect('delicious.db')
cur = con.cursor()

cur.execute("DELETE FROM Ingredients")
cur.execute("DELETE FROM Recipes")
cur.execute("DELETE FROM Composition")

con.commit()
