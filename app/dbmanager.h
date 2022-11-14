#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQueryModel>
#include <QDir>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <bfsNamespace.h>

/*!
 * \brief The Souvenir struct
 */
struct Souvenir
{
    int teamID;
    QString name;
    double price;
    int purchaseQty = 0;
    Souvenir() = default;
    Souvenir(int id,QString name,double price):teamID(id),name(name),price(price){}
    Souvenir(const Souvenir&) = default;
};

/*!
 * @class DBManager
 * @brief The DBManager class manages core methods used to access the data
 * stored in the database
 */
class DBManager: public QWidget, public QSqlDatabase {
    Q_OBJECT
public:
	/*!
	 * @brief Creates one instance of the database
	 * @return Pointer the the instance of the database class
	 */
	static DBManager* instance();

	/*!
	 * @brief Deleted copy constructor
	 */
	DBManager(const DBManager&) = delete;

	/*!
	 * @brief Deleted copy assignment
	 */
	DBManager& operator=(const DBManager&) = delete;

    /*!
     * @brief Compares user login to values in the database
     * @return bool representing successful login
     */
	bool CheckLogin(const QString &username, const QString &password);

	/*!
	 * \brief ImportTeams to db
	 */
	void ImportTeams();

	/*!
	 * \brief GetTeams from db
	 * \param teams; list of teams
	 */
	void GetTeams(QStringList &teams);

	/*!
	 * \brief GetSouvenirs from db
	 * \param teamName; teams that souvenir belongs to
	 * \param list; list of souvenir's
	 */
	void GetSouvenirs(QString teamName, QStringList &list);

	/*!
	 * \brief GetNumSouvenir from dn
	 * \param teamName
	 * \return number of souvenir's
	 */
	int GetNumSouvenir(QString teamName);

	/*!
	 * \brief GetNumTeams from db
	 * \return number of teams in db
	 */
	int GetNumTeams();

	/*!
	 * \brief SouvenirNameToPrice from db
	 * \param team; name
	 * \param souvenir; name
	 * \return price of souvenir
	 */
	QString SouvenirNameToPrice(QString team, QString souvenir);

	/*!
	 * \brief AddInfo to db
	 * \param teamName
	 * \param stadiumName
	 * \param seatCap
	 * \param location
	 * \param conference
	 * \param division
	 * \param surfaceType
	 * \param roofType
	 * \param dateOpen
	 */
	void AddInfo(QString teamName, QString stadiumName, QString seatCap,
				 QString location, QString conference, QString division,
				 QString surfaceType, QString roofType, QString dateOpen);

	/*!
	 * \brief AddSouvenir to db
	 * \param teamName
	 * \param item
	 * \param price
	 */
	void AddSouvenir(QString teamName, QString item, QString price);

	/*!
	 * \brief UpdateSouvenirPrice on db
	 * \param teamName
	 * \param item
	 * \param price
	 */
	void UpdateSouvenirPrice(QString teamName, QString item, QString price);

	/*!
	 * \brief DeleteSouvenir from db
	 * \param teamName
	 * \param item
	 */
	void DeleteSouvenir(QString teamName, QString item);

	/*!
	 * \brief UpdateInformation on db
	 * \param id; linked to team name
	 * \param stadiumName
	 * \param cap; capacity
	 * \param loc; location
	 * \param surfaceType
	 * \param roofType
	 * \param dateOpen
	 */
	void UpdateInformation(int id, QString stadiumName, QString cap,
						   QString loc, QString surfaceType, QString roofType,
						   QString dateOpen);

	/*!
	 * \brief isTeamExist in db
	 * \param teamName
	 * \return bool
	 */
	bool isTeamExist(QString teamName);

	/*!
	 * \brief isSouvenirExist
	 * \param teamName
	 * \param item
	 * \return bool
	 */
	bool isSouvenirExist(QString teamName, QString item);

    /*!
	 * \brief function adds souvenirs from recent purchase into the 'purchases' database table
	 * \param souvenirs; collection of all souvenirs
     */
	void AddPurchases(QVector<Souvenir> souvenirs);

	/*!
	 * \brief getNewID
	 * \return a new id from db
	 */
	int GetNewID();

	/*!
	 * \brief getTeamName
	 * \param id; linked to team name
	 * \return name of team
	 */
	QString GetTeamName(int id);

	/*!
	 * \brief getStadiumName
	 * \param id; team linker
	 * \return stadium name
	 */
	QString GetStadiumName(int id);

	/*!
	 * \brief getTeamID
	 * \param teamName
	 * \return id in db linked to a team name
	 */
	int GetTeamID(QString teamName);

	/*!
	 * \brief CreateShoppingList
	 * \param teams
	 * \param teamSouvenirs; list of souvenir for a team
	 */
    void CreateShoppingList(QStringList teams, QVector<Souvenir>& teamSouvenirs);

    /*!
     * @brief function gets all adjacent cities and distances based on the vertex input and
     * returns it all as a vector of type node
     *
     * @param vertex; reperesents team id
     * @return vector<node>; which contains all the adj teams and their weights
     */
    std::vector<generalContainer::node> getAdjList(int vertex);

    /*!
     * @brief function takes in an id and returns the team name which corresponds with that id
     * @param id; int that represents the teams id
     * @return QString; the name of the team based on the team
     */
    static bool comparater(generalContainer::node n1, generalContainer::node n2);
private:
	QSqlQuery query;	/// Generic reusable query
	/*!
	 * @brief DBManager (private)
	 * @param parent; Linked to QWidget class
	 */
	explicit DBManager(QWidget *parent = nullptr);

	/*!
	 * @brief Destructor
	 */
	~DBManager();
	/*!
	 * \brief parser helper function
	 * \param line; string of char
	 * \param delim
	 * \return QStringList of strings separated by delim
	 */
	QStringList Parser(QString &line, const char delim);
};


#endif // DBMANAGER_H
