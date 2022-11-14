#include "dbmanager.h"
#include <QFileDialog>
#include <QElapsedTimer>

DBManager::DBManager(QWidget *parent)
    : QWidget{parent}, QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))
{
    setDatabaseName(QDir::currentPath() + "/Data.db");

    // Print error if database does not open
    if (!open()) {
        qDebug() << "Connection to database failed";
    } else {
        qDebug() << "Connection to database succeeded";
    }
}

DBManager::~DBManager()
{
    // Output when database closes
    if (open()) {
        qDebug() << "Connection to database has been closed";
        QSqlDatabase::close();
    }
}

DBManager* DBManager::instance()
{
    // Create one and only one instance of the database
    static DBManager instance;
    return &instance;
}

bool DBManager::CheckLogin(const QString &username, const QString &password)
{
    // Prep initial login bool
    bool found = false;

    // Prep query
    query.prepare("SELECT * FROM accounts WHERE username = :username AND password = :password AND level = 'ADMIN';");

    // Bind values safely
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    // If query executes successfully, check values
    if(query.exec())
    {
        // If values are correct, login
        if(query.next())
        {
            found = true;
            qDebug() << "Login Successful";
        }
    }
    else // If query fails, print failure
    {
        qDebug() << "Login Query Failed";
    }

    return found;
}

QStringList DBManager::Parser(QString &line, const char delim)
{
	QStringList list;
	bool inString = false;
	auto marker = line.begin();
	for (auto itr = marker; itr != line.end(); itr++) {
		if (*itr == '"' && !inString) {
			inString = !inString;
			marker += 1;
		} else if (*itr == '"' && inString) {
			list.push_back(QString(marker, itr - marker));
			marker = itr + 2;
			itr = marker - 1;
			inString = !inString;
		} else if (*itr == delim && !inString) {
			list.push_back(QString(marker, itr - marker));
			marker = itr + 1;
		}
	}
	list.push_back(QString(marker));
	return list;
}

void DBManager::ImportTeams()
{
	QString filter = "CSV file (*.csv)";
	QStringList teams;
	// opens system file directory in your home path
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), filter);

	if (fileName.isEmpty()) {
		qDebug() << "No file name specified";
		return;
	}
	// needed to iterate through file...similar to fstream
	QFile file(fileName);
	// populates cities with all the cities in the DB
	GetTeams(teams);

	QElapsedTimer timer;
	timer.start();
	if (!file.open(QFile::ReadOnly)) {
		qDebug() << "DBManager::ImportTeams() : error opening the file(0)";
	} else {
		qDebug() << "file opened";
		QTextStream ss(&file);
		QStringList list;
		bool infoStarted = false;
		bool distanceStarted = false;
		bool souvenirStarted = false;

		while (!ss.atEnd()) {
			QString line = ss.readLine();
			if (line == "//information") {
				infoStarted = true;
				distanceStarted = false;
				souvenirStarted = false;
				line = ss.readLine();
			} else if (line == "//distances") {
				distanceStarted = true;
				infoStarted = false;
				souvenirStarted = false;
				line = ss.readLine();
			} else if (line == "//souvenirs") {
				souvenirStarted = true;
				infoStarted = false;
				distanceStarted = false;
				line = ss.readLine();
			}

			// parse each line in teh csv into a QStringList
			list = Parser(line, ',');

			// only insert if the city is unique
			if (!teams.contains(list[0])) {
				query.prepare("INSERT INTO teams(teamNames) VALUES(:team)");
				query.bindValue(":team", list[0]);
				if (!query.exec()) {
					qDebug() << "DBManager::ImportTeams() : error executing query(1)";
					exit(1);
				}
				// re-populates cities with updated city list
				teams.clear();
				GetTeams(teams);
			}

			// finds and stores the id associated with the city name
			query.prepare("SELECT id FROM teams WHERE teamNames = :team");
			query.bindValue(":team", list[0]);
			if (!query.exec()) {
				qDebug() << "DBManager::ImportTeams() : error executing query(2)";
				exit(2);
			}
			query.first();
			int id = query.value(0).toInt();

			if (infoStarted) {
				query.prepare("INSERT INTO information(id, stadiumName,"
							  "seatCap, location, conference, division,"
							  "surfaceType, roofType, dateOpen) VALUES(:id,"
							  ":stadiumName, :seatCap, :location, :conference,"
							  ":division, :surfaceType, :roofType, :dateOpen)");
				query.bindValue(":id", id);
				query.bindValue(":stadiumName", list[1]);
				query.bindValue(":seatCap", list[2]);
				query.bindValue(":location", list[3]);
				query.bindValue(":conference", list[4]);
				query.bindValue(":division", list[5]);
				query.bindValue(":surfaceType", list[6]);
				query.bindValue(":roofType", list[7]);
				query.bindValue(":dateOpen", list[8]);
			} else if (distanceStarted) {
				query.prepare("INSERT INTO distance(id, beginStadium, "
							  "endStadium, distanceTo) VALUES(:id, "
							  ":beginStadium, :endStadium, :distanceTo)");
				query.bindValue(":id", id);
				query.bindValue(":beginStadium", list[1]);
				query.bindValue(":endStadium", list[2]);
				query.bindValue(":distanceTo", list[3]);
			} else if (souvenirStarted) {
				query.prepare("INSERT INTO souvenir(id, items, price) "
							  "VALUES(:id, :items, :price)");
				query.bindValue(":id", id);
				query.bindValue(":items", list[1]);
				query.bindValue(":price", list[2]);
			}

			// If query does not execute, print error
			if (!query.exec()) {
				qDebug() << "DBManager::ImportTeams() : error executing query(3)";
				exit(3);
			}
		}
		query.finish();
	}
	qDebug() << "Elapse time: " << timer.elapsed() << "ms";
}

void DBManager::GetTeams(QStringList &teams)
{
	query.prepare("SELECT teamNames FROM teams");
	if (query.exec()) {
		while (query.next())
			teams.push_back(query.value(0).toString());
		query.finish();
	} else {
		qDebug() << "DBManager::GetTeams(QStringList&) : query failed";
	}
}

void DBManager::GetSouvenirs(QString teamName, QStringList &list)
{
	query.prepare("SELECT items FROM souvenir, teams WHERE teamNames = :teamName AND teams.id = souvenir.id");
	query.bindValue(":teamName", teamName);

	if (query.exec()) {
		while (query.next())
			list.push_back(query.value(0).toString());
		query.finish();
	} else {
		qDebug() << "DBManager::GetSouvenirs(QString, QStringList&) : query failed";
	}
}

int DBManager::GetNumSouvenir(QString teamName)
{

	query.prepare("SELECT COUNT(*) FROM souvenir, teams WHERE teamNames = "
				  ":teamName AND teams.id = souvenir.id");
	query.bindValue(":teamName", teamName);

	if (query.exec()) {
		query.first();
		return query.value(0).toInt();
	}
	qDebug() << "DBManager::GetNumSouvenir(QString) : query failed";
	return -1;
}

int DBManager::GetNumTeams()
{
	query.prepare("SELECT COUNT(*) FROM teams");
	if (query.exec()) {
		query.first();
		return query.value(0).toInt();
	}
	qDebug() << "DBManager::GetNumTeams() : query failed";
	return -1;
}

QString DBManager::SouvenirNameToPrice(QString team, QString souvenir)
{
	query.prepare("SELECT price FROM souvenir, teams WHERE teamNames = :team "
				  "AND teams.id = souvenir.id AND souvenir.items = :souvenir");

	query.bindValue(":team", team);
	query.bindValue(":souvenir", souvenir);

	if (query.exec()) {
		query.first();
		return query.value(0).toString();
	} else { // If query does not execute, print error
		qDebug() << "DBManager::SouvenirNameToPrice(QString, QString) : query failed";
	}
	return QString("Error");
}

void DBManager::AddInfo(QString teamName, QString stadiumName, QString seatCap,
						QString location, QString conference, QString division,
						QString surfaceType, QString roofType, QString dateOpen)
{
	// Finds and stores the id associated with the city name
	query.prepare("SELECT id FROM teams WHERE teamNames = :teamName");
	query.bindValue(":teamName", teamName);
	if (!query.exec()) {
		qDebug() << "DBManager::AddInfo(QString... ) : query failed{1}";
		return;
	}
	query.first();
	int id = query.value(0).toInt();

	// Using the obtained id, a new food item and price is INSERTED
	query.prepare("INSERT INTO information(id, stadiumName, seatCap, location, "
				  "conference, division, surfaceType, roofType, dateOpen) "
				  "VALUES(:id, :stadiumName, :seatCap, :location, :conference, "
				  ":division, :surfaceType, :roofType, :dateOpen)");
	query.bindValue(":id", id);
	query.bindValue(":stadiumName", stadiumName);
	query.bindValue(":seatCap", seatCap);
	query.bindValue(":location", location);
	query.bindValue(":conference", conference);
	query.bindValue(":division", division);
	query.bindValue(":surfaceType", surfaceType);
	query.bindValue(":roofType", roofType);
	query.bindValue(":dateOpen", dateOpen);

	if (!query.exec())
		qDebug() << "DBManager::AddInfo(QString... ) : query failed{2}";
	query.finish();
}

void DBManager::AddSouvenir(QString teamName, QString item, QString price)
{
	// Finds and stores the id associated with the city name
	query.prepare("SELECT id FROM teams WHERE teamNames = :teamName");
	query.bindValue(":teamName", teamName);
	if (!query.exec()) {
		qDebug() << "DBManager::AddSouvenir(QString, QString, QString) : query failed{1}";
		return;
	}
	query.first();
	int id = query.value(0).toInt();

	query.prepare("INSERT INTO souvenir(id, items, price) VALUES(:id, :items, :price)");
	query.bindValue(":id", id);
	query.bindValue(":items", item);
	query.bindValue(":price", price);

	if (!query.exec())
		qDebug() << "DBManager::AddSouvenir(QString, QString, QString) : query failed{2}";
	query.finish();
}

void DBManager::UpdateSouvenirPrice(QString teamName, QString item, QString price)
{
	// Prep query
	query.prepare("UPDATE souvenir SET price = :price WHERE souvenir.items = "
				  ":item AND souvenir.id = (SELECT id FROM teams WHERE "
				  "teams.teamNames = :teamName)");

	// Bind values safely
	query.bindValue(":price", price);
	query.bindValue(":item", item);
	query.bindValue(":teamName", teamName);

	// If query does not execute, print error
	if (!query.exec())
		qDebug() << "UpdateSouvenirPrice(QString, QString, QString) : query failed{2}";
	query.finish();
}

void DBManager::DeleteSouvenir(QString teamName, QString item)
{
	// Prep query
		query.prepare("DELETE FROM souvenir WHERE souvenir.id = (SELECT id FROM teams WHERE teams.teamNames = :teamName) AND souvenir.items = :item");

		// Bind values safely
		query.bindValue(":teamName", teamName);
		query.bindValue(":item", item);

		// If query does not execute, print error
		if (!query.exec())
			qDebug() << "DBManager::DeleteSouvenir(QString, QString) : query failed";
		query.finish();
}

void DBManager::UpdateInformation(int id, QString stadiumName, QString cap, QString loc, QString surfaceType, QString roofType, QString dateOpen)
{
	// Prep query
	query.prepare("UPDATE information SET stadiumName = :stadiumName, "
				  "seatCap = :cap, location = :loc, surfaceType = "
				  ":surfaceType, roofType = :roofType, dateOpen = :dateOpen "
				  "WHERE information.id = :id");

	// Bind values safely
	query.bindValue(":stadiumName", stadiumName);
	query.bindValue(":cap", cap);
	query.bindValue(":loc", loc);
	query.bindValue(":surfaceType", surfaceType);
	query.bindValue(":roofType", roofType);
	query.bindValue(":dateOpen", dateOpen);
	query.bindValue(":id", id);

	// If query does not execute, print error
	if (!query.exec())
		qDebug() << "DBManager::UpdateInformation(int, QString, QString, QString, QString, QString) : query failed";
	query.finish();
}

bool DBManager::isTeamExist(QString teamName)
{
	query.prepare("SELECT EXISTS (SELECT 1 FROM teams WHERE teamNames = :teamName)");
	query.bindValue(":teamName", teamName);
	if (!query.exec())
		qDebug() << "DBManager::isTeamExist(QString teamName) : query failed";
	query.first();
	return (bool) query.value(0).toInt();
}

bool DBManager::isSouvenirExist(QString teamName, QString item)
{
//	query.prepare("SELECT id FROM souvenir, teams WHERE souvenir.items = :item AND teams.teamNames = :teamName");
	query.prepare("SELECT EXISTS (SELECT 1 FROM souvenir, teams WHERE items = :item AND teamNames = :teamName)");
	query.bindValue(":teamName", teamName);
	query.bindValue(":item", item);
	if (!query.exec())
		qDebug() << "DBManager::isSouvenirExist(QString teamName, QString item) : query failed";
	query.first();
	return (bool) query.value(0).toInt();
}

void DBManager::AddPurchases(QVector<Souvenir> souvenirs)
{
	int id = GetNewID();
    query.prepare("INSERT INTO purchases(teamID, purchaseID, items, price, quantity) VALUES(:teamID, :id, :item, :price, :qty)");

    for (int i = 0; i < souvenirs.size(); i++)
    {
        QString qty = QString::number(souvenirs[i].purchaseQty);
        query.bindValue(":teamID", souvenirs[i].teamID);
        query.bindValue(":id", id);
        query.bindValue(":item", souvenirs[i].name);
        query.bindValue(":price", QString::number(souvenirs[i].price));
        query.bindValue(":qty", QString::number(souvenirs[i].purchaseQty));

        if (!query.exec())
            qDebug() << "DBManager::addPurchases(QVector<Souvenir> souvenirs) : query failed";
        query.finish();
    }
}

int DBManager::GetNewID()
{
    int maxID = 1;
    query.prepare("SELECT purchaseID FROM purchases ORDER BY purchaseID DESC LIMIT 1");
    if(query.exec())
    {
        if (query.first())
        {
            maxID = query.value(0).toInt();
            query.finish();
            return maxID + 1;
        }
        else
            return maxID;
    }
    qDebug() << "DBManager::getNewID() : query failed";
    return -1;
}

QString DBManager::GetTeamName(int id)
{
    QString queryString;
    queryString = "SELECT teamNames FROM teams WHERE id = :id";
    query.prepare(queryString);
    query.bindValue(":id", id);

    if(!query.exec())
    {
        qDebug() << "DBManager::getTeamName id: " << id << " failed";
    }
    else
    {
        query.first();
        return query.value(0).toString();
    }
    return "Invalid Team Name";
}

void DBManager::CreateShoppingList(QStringList teams,QVector<Souvenir>& teamSouvenirs)
{
    // Prep general query
    int teamsAsInt[teams.size()];
    for (int i = 0; i < teams.size(); i++)
		teamsAsInt[i] = GetTeamID(teams[i]);

    query.prepare("SELECT id,items, price FROM souvenir WHERE "
                  "id = :teamID");

    //variable to convert string with , to int
    QLocale c(QLocale::C);

    // Run query in a loop
    for(int index = 0; index < teams.size(); index++)
    {
        // Bind value
        query.bindValue(":teamID", teamsAsInt[index]);

        // Execute query
        if(query.exec())
        {
            // While food exists in DB for this specific city
            while(query.next())
            {
                // Create food item
                int index = query.value(0).toInt();
                QString name = query.value(1).toString();
                double price = c.toDouble(query.value(2).toString());
                Souvenir current(index,name,price);

                teamSouvenirs.push_back(current);
            }
        }
        else // If query fails, output error
        {
            qDebug() << "Query didn't execute properly";
        }
    }
}

int DBManager::GetTeamID(QString teamName)
{
    query.prepare("SELECT id FROM teams where teamNames = :team");
    query.bindValue(":team", teamName);
    if(query.exec())
    {
        query.first();
        return query.value(0).toInt();
    }
    return -1;

}

QString DBManager::GetStadiumName(int id)
{
    query.exec("SELECT stadiumName FROM information WHERE id = " + QString::number(id));
    query.first();
    return query.value(0).toString();
}
bool DBManager::comparater(generalContainer::node n1, generalContainer::node n2)
{
    return (n1.weight < n2.weight);
}
std::vector<generalContainer::node> DBManager::getAdjList(int vertex)
{
    std::vector<generalContainer::node> returnVec; // vector with all pairs of ending team and weight adj to vertex
    QString queryString = "SELECT id,distanceTo "
                          "FROM distance "
                          "WHERE beginStadium "
                          "IN(SELECT endStadium FROM distance WHERE id = :id) "
                          "AND endStadium = (SELECT beginStadium FROM distance WHERE id = :id) "
                          "AND id!=:id ";
    query.prepare(queryString);
    query.bindValue(":id",vertex);
    if(!query.exec())
    {
        qDebug() << "DBManager::getAdjList vertex: " << vertex << " failed";
    }
    else
    {
        while(query.next())
        {
            generalContainer::node tempNode;
            tempNode.end = query.value(0).toInt();
            tempNode.weight = query.value(1).toInt();
            returnVec.push_back(tempNode);
        }
    }
    std::sort(returnVec.begin(), returnVec.end(), comparater);
    return returnVec;
}
