import sqlite3
import csv
import sys

def ImportTeamTemplate(filename):
    # conn = sqlite3.connect('Data.db')
    # conn.execute("PRAGMA foreign_keys=1")
    # cur = conn.cursor()    

    cur.execute("CREATE TABLE IF NOT EXISTS teams(id INTEGER PRIMARY KEY AUTOINCREMENT, teamNames TEXT)")

    with open(filename, 'r') as csvFile:
        readCSV = csv.reader(csvFile, delimiter=',')
        
        for row in readCSV:
            cur.execute("INSERT INTO teams(teamNames) VALUES(?)", row)
    # conn.commit()
    # cur.close()
    # conn.close()

def CreateTable(tableName, **kwargs):
    query = "CREATE Table IF NOT EXISTS {0}(teamID INTEGER, purchaseID INTEGER, ".format(tableName)
    for i, (key, value) in enumerate(kwargs.items()):
        query += '{0} {1}, '.format(key, value)
    query += 'FOREIGN KEY(teamID) REFERENCES teams(id) ON DELETE CASCADE)'
    cur.execute(query)

def ImportFile(filename, tableName, **kwargs):
    # conn = sqlite3.connect("Data.db")
    # cur = conn.cursor()

    cur.execute('SELECT * FROM teams')
    outer = cur.fetchall()

    query = 'CREATE TABLE IF NOT EXISTS {0}(id INTEGER, '.format(tableName)
    for key, value in kwargs.items():
        query += '{0} {1}, '.format(key, value)
    query += 'FOREIGN KEY(id) REFERENCES teams(id) ON DELETE CASCADE)'

    cur.execute(query)

    with open(filename, 'r') as csvFile:
        readCSV = csv.reader(csvFile, delimiter=',')
        # cur.executemany("INSERT INTO "+table+"(startcity, endcity, distance) VALUES(?, ?, ?)", readCSV)

        line_list = list(readCSV)
        
        query2 = 'INSERT INTO {0}(id'.format(tableName)
        for args in kwargs.keys():
            query2 += ', ' + args
        query2 += ') VALUES(?'

        for i in range(len(kwargs)):
            query2 += ', ?'
        query2 += ')'
        
        count = 1
        i = 0
        for index in outer:
            while i < len(line_list):
                if index[1] == line_list[i][0]:
                    temp = [count]
                    temp.extend(line_list[i][1:])
                    cur.execute(query2, temp)
                    i += 1
                else:
                    count += 1
                    break

    # conn.commit()
    # cur.close()
    # conn.close()

def ImportAccounts(username, password, accessLevel):
    cur.execute("CREATE TABLE IF NOT EXISTS accounts(username TEXT, password TEXT, level TEXT)")
    cur.execute("INSERT INTO accounts(username, password, level) VALUES(?, ?, ?)", (username, password, accessLevel))

# def getData():
#     con = sqlite3.connect('Data.db')
 
#     with con:

#         cur = con.cursor()
#         cur.execute("SELECT * FROM Food")
        
#         for row in cur.fetchall():
#             print(row[0], row[1], row[2])



def DropTable(tableName):
    # conn = sqlite3.connect('Data.db')
    # cur = conn.cursor()

    temp = "DROP TABLE IF EXISTS {0}".format(tableName)
    cur.execute(temp)
    # conn.commit()


if __name__ == "__main__":
    conn = sqlite3.connect('Data.db')
    conn.execute("PRAGMA foreign_keys=1")
    cur = conn.cursor()

    DropTable('teams')
    DropTable('distance')
    DropTable('information')
    DropTable('souvenir')
    DropTable('accounts')
    DropTable('purchases')

    ImportTeamTemplate('teams.csv')
    ImportFile('nfl_distances.csv', 'distance', beginStadium = 'TEXT',
    endStadium = 'TEXT', distanceTo = 'TEXT')
    ImportFile('nfl_information.csv', 'information', stadiumName = 'TEXT',
    seatCap = 'TEXT', location = 'TEXT', conference = 'TEXT', division = 'TEXT',
    surfaceType = 'TEXT', roofType = 'TEXT', dateOpen = 'TEXT')
    ImportFile('nfl_team_souvenir.csv', 'souvenir', items = 'TEXT', price = 'TEXT')
    ImportAccounts('admin', 'password', 'ADMIN')
    CreateTable('purchases', items = "TEXT", price = "TEXT", quantity = "TEXT")
    
    # getData()

    conn.commit()
    cur.close()
    conn.close()
