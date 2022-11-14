#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QDebug>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableView>
#include <QVector>
#include <QHeaderView>
#include <QStringListModel>
#include "dbmanager.h"

class DBManager;
/*!
 * @class TableManager
 * @brief The TableManager holds attributes for all tables, initializes them,
 * and populates their contents. For use in all program functionality
 */
class TableManager {
public:

	/*!
	 * @brief Default TableManager object constructor
	 */
    TableManager();

	/*!
	 * @brief Destructor
	 */
	~TableManager() = default;

	/*!
	 * @brief Deleted copy constructor
	 */
	TableManager(const TableManager&) = delete;

	/*!
	 * @brief Deleted copy assignment
	 */
	TableManager& operator=(const TableManager&) = delete;

	// ************************* Admin Info Table Attributes ************************

	/// Number of columns in table
	const int ADMIN_TABLE_INFO_COL_COUNT = 9;
	/// positions of admin table columns by name
	enum AdminTableInfoColPositions	{ A_TEAMNAME_INFO, A_STADIUMNAME, A_SEATCAP,
									  A_LOCATION, A_CONFERENCE, A_DIVISION,
									  A_SURFACETYPE, A_ROOFTPYE, A_DATEOPEN };
	/// Names of admin table column headers
	QStringList adminTableInfoColNames = { "Team Name", "Stadium Name",
										   "Seat Cap", "Location", "Conference",
										   "Division", "Surface Type",
										   "Roof Type", "Date Open" };

	// ************************* Admin distance Table Attributes ************************

	/// Number of columns in table
	const int ADMIN_TABLE_Distance_COL_COUNT = 4;
	/// positions of admin table columns by name
	enum AdminTableDistColPositions	{ A_TEAMNAME_DISTANCE, A_BEGINSTADIUM,
									  A_ENDSTADIUM, A_DISTANCETO };
	/// Names of admin table column headers
	QStringList adminTableDistColNames = { "Team Name", "Starting Stadium",
										   "Ending Stadium", "Distance To" };

	// ************************* Admin Souvenir Table Attributes ************************

	/// Number of columns in table
	const int ADMIN_TABLE_Souvenir_COL_COUNT = 4;
	/// positions of admin table columns by name
	enum AdminTableSouvColPositions	{ AS_KEY, A_TEAMNAME_SOUVENIR, A_ITEM, A_PRICE };
	/// Names of admin table column headers
	QStringList adminTableSouvColNames = { "Key", "Team Name", "Item", "Price" };

	// ************************* Admin purchases Table Attributes ************************

	/// Names of admin table column headers
	QStringList adminTablePurchColNames = { "Key", "Team Name", "Item", "Price", "Quantity" };

						/***********
						 * Methods *
						 ***********/

	// ****************** Admin Table Table Methods ****************************
	/*!
	 * @brief Initializes admin table using model for team info list
	 * @param table; Table to be initialized
	 */
	void AdminInfoTable(QTableView* table);

	/*!
	 * \brief InitializeAdminInfoEditTable clear admin info edit table
	 * \param table; Table to be initialized
	 */
	void InitializeAdminInfoEditTable(QTableWidget* table);

	/*!
	 * \brief PopulatedAdminInfoEditTable
	 * \param table; Table to be initialized
	 */
	void PopulatedAdminInfoEditTable(QTableWidget* table);

	/*!
	 * @brief Initializes admin table using model for team distance list
	 * @param table; Table to be initialized
	 */
	void AdminDistTable(QTableView *table);

	/*!
	 * @brief Initializes admin table using model for team souvenir list
	 * @param table; Table to be initialized
	 */
	void AdminSouvTable(QTableView *table);

	/*!
	 * @brief Initializes admin table using model for team purchases list
	 * @param table; Table to be initialized
	 */
	void AdminPuchaseTable(QTableView *table, int index);

	/*!
	 * @brief Initializes admin table manually
	 * @param table; Table to be initialized
	 */
	void InitializeAdminEditTable(QTableWidget* table);

	/*!
	 * @brief Populates admin table with relevant information
	 * @param table; Table to be initialized
	 */
	void PopulateAdminEditTable(QTableWidget* table);

	/*!
	 * @brief Deletes all rows in table
	 * @param table; Table to be deleted
	 */
	void DeleteAllTableRows(QTableWidget *table);

    /*!
     * @brief Shows team info on team search [age
     * @param table; Table to be displayed
     * @param teamName; name of teams whos info will be displayed
     */
    void ShowTeamInfo(QTableView * table, QString teamName);

    /*!
     * @brief shows list of teams user can visit
     * @param table; Table to be displayed
     */
	void ShowTeamNames(QTableView * table);

	/*!
	 * \brief clearTable
	 * \param table ui pointer
	 */
	void ClearTable(QTableView * table);

    /**
     * @brief Show list of teams on a qtableview
     * @param table: qtableview to display
     * @param avilable: list of teams to display
     */
	void ShowTeams(QTableView* table, QStringList& avilable);

    /**
     * @brief Insert column of spinboxes in purchase page
     * @param table: table to insert spinboxes
     * @param min: smallest value in spinbox
     * @param max: largest value in spinbox
     * @param col: column to insert the spinbox
     */
    void InsertSpinBoxCol(QTableWidget* table, const int min, const int max, const int col);

    /**
     * @brief Create the layout for the purchase table
     * @param purchaseTable: table to set the layout
     * @param cols: number of columns in table
     * @param headers: list of headers for the columns in table
     */
    void InitializePurchaseTable(QTableWidget* purchaseTable, const int &cols, const QStringList &headers);

    /**
     * @brief Populate the purchase table with souvenir items
     * @param purchaseTable: table to display the souvenir
     * @param teamSouvenir: list of souvenirs ot display
     */
    void PopulatePurchaseTable(QTableWidget* purchaseTable, QVector<Souvenir>& teamSouvenir);
    /*!
     * \brief prepares receipt table for values
     * \param receiptTable; table to be displayed
     * \param cols; number of columns
     * \param headers; title of headers
     */
    void InitializeReceiptTable(QTableWidget* receiptTable, const int &cols, const QStringList &headers);
    /*!
     * \brief shows list of purchased souvenirs
     * \param purchaseTable; Table to be displayed
     * \param teamSouvenir; list of souvenirs to display
     */
    void PopulateReceiptTable(QTableWidget* receiptTable, QVector<Souvenir>& teamSouvenir);

    QVector<QSpinBox*>* purchaseTableSpinBoxes;

public slots:
    /**
     * @brief Function to update total price of souvenirs after user selects an item
     * @param table: table displaying the souvenirs
     * @return the total cost of souvenirs
     */
    double UpdateTotalPrice(QTableWidget* table);
};

#endif // TABLEMANAGER_H
