#include "tablemanager.h"
#include "dbmanager.h"

TableManager::TableManager()
{
    purchaseTableSpinBoxes = new QVector<QSpinBox*>;
}

void TableManager::AdminInfoTable(QTableView *table)
{
	QSqlQueryModel *model = new QSqlQueryModel;

	model->setQuery("SELECT teamNames, stadiumName, seatCap, location, "
					"conference, division, surfaceType, roofType, dateOpen "
					"FROM information, teams WHERE teams.id = information.id "
					"ORDER BY teams.id");

	if (!model->query().exec())
		qDebug() << "TableManager::InitializeAdminInfoTable() : query failed";

	model->setHeaderData(0, Qt::Horizontal, adminTableInfoColNames[0], Qt::DisplayRole);
	model->setHeaderData(1, Qt::Horizontal, adminTableInfoColNames[1], Qt::DisplayRole);
	model->setHeaderData(2, Qt::Horizontal, adminTableInfoColNames[2], Qt::DisplayRole);
	model->setHeaderData(3, Qt::Horizontal, adminTableInfoColNames[3], Qt::DisplayRole);
	model->setHeaderData(4, Qt::Horizontal, adminTableInfoColNames[4], Qt::DisplayRole);
	model->setHeaderData(5, Qt::Horizontal, adminTableInfoColNames[5], Qt::DisplayRole);
	model->setHeaderData(6, Qt::Horizontal, adminTableInfoColNames[6], Qt::DisplayRole);
	model->setHeaderData(7, Qt::Horizontal, adminTableInfoColNames[7], Qt::DisplayRole);
	model->setHeaderData(8, Qt::Horizontal, adminTableInfoColNames[8], Qt::DisplayRole);

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setModel(model);
}



void TableManager::AdminDistTable(QTableView *table)
{
	QSqlQueryModel *model = new QSqlQueryModel;

	model->setQuery("SELECT teamNames, beginStadium, endStadium, distanceTo "
					"FROM distance, teams WHERE teams.id = distance.id "
					"ORDER BY teams.id");

	if (!model->query().exec())
		qDebug() << "TableManager::InitializeAdminDistTable() : query failed";

	model->setHeaderData(0, Qt::Horizontal, adminTableDistColNames[0], Qt::DisplayRole);
	model->setHeaderData(1, Qt::Horizontal, adminTableDistColNames[1], Qt::DisplayRole);
	model->setHeaderData(2, Qt::Horizontal, adminTableDistColNames[2], Qt::DisplayRole);
	model->setHeaderData(3, Qt::Horizontal, adminTableDistColNames[3], Qt::DisplayRole);

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setModel(model);
}

void TableManager::AdminSouvTable(QTableView *table)
{
	QSqlQueryModel *model = new QSqlQueryModel;

	model->setQuery("SELECT teamNames, items, price FROM souvenir, teams "
					"WHERE teams.id = souvenir.id ORDER BY teams.id");

	if (!model->query().exec())
		qDebug() << "TableManager::InitializeAdminSouvTable() : query failed";

	model->setHeaderData(0, Qt::Horizontal, adminTableSouvColNames[1], Qt::DisplayRole);
	model->setHeaderData(1, Qt::Horizontal, adminTableSouvColNames[2], Qt::DisplayRole);
	model->setHeaderData(2, Qt::Horizontal, adminTableSouvColNames[3], Qt::DisplayRole);

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setModel(model);
}

void TableManager::AdminPuchaseTable(QTableView *table, int index)
{
	if (index < 0)
		return;
	QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT teamNames, items, price, quantity FROM teams, purchases "
                        "WHERE purchaseID = '" + QString::number(index + 1) + "' "
                        "AND teams.id = teamID");

	if (!model->query().exec())
		qDebug() << "TableManager::AdminPuchaseTable(QTableView *table, int index) : query failed";

	model->setHeaderData(0, Qt::Horizontal, adminTablePurchColNames[1], Qt::DisplayRole);
	model->setHeaderData(1, Qt::Horizontal, adminTablePurchColNames[2], Qt::DisplayRole);
	model->setHeaderData(2, Qt::Horizontal, adminTablePurchColNames[3], Qt::DisplayRole);
	model->setHeaderData(3, Qt::Horizontal, adminTablePurchColNames[4], Qt::DisplayRole);

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->verticalHeader()->hide();

	table->setModel(model);
}

void TableManager::InitializeAdminEditTable(QTableWidget *table)
{
	table->clearContents();
	table->setColumnCount(ADMIN_TABLE_Souvenir_COL_COUNT);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->setHorizontalHeaderLabels(adminTableSouvColNames);
	// TODO - Might be a good idea to set column widths here
	table->setEditTriggers(QTableView::DoubleClicked);
	table->setSelectionBehavior(QTableView::SelectRows);
	table->hideColumn(AS_KEY);
	table->verticalHeader()->hide();

	DeleteAllTableRows(table);
}

#define db DBManager::instance()
void TableManager::PopulateAdminEditTable(QTableWidget *table)
{
	QTableWidgetItem* priceItem;
	QTableWidgetItem *item;
	QString currentName;
	QString previousName;
	QStringList teams;
	QStringList souvenirs;

	db->GetTeams(teams);

	// For the length of the city list
	for(int i = 0; i < teams.size(); i++)
	{
		int numSouvenir = db->GetNumSouvenir(teams[i]);
		souvenirs.clear();
		db->GetSouvenirs(teams[i], souvenirs);

		// Iterate through each city's food list
		for(int j = 0; j < numSouvenir; j++)
		{
			item = new QTableWidgetItem(db->SouvenirNameToPrice(teams[i], souvenirs[j]));
			// Generate item price tablewidget item
			priceItem = item;

			// If list is not empty
			if(table->rowCount() != 0)
			{
				// Check to see if there's a match between this row's city name and the previous row's city name
				currentName = table->item(table->rowCount() -1, AS_KEY)->data(0).toString();
				previousName = teams[i];

				// Add a row to the end
				table->insertRow(table->rowCount());

				bool match = currentName == previousName;

				// If the row names do not match, insert the city name into the name column
				if(!match)
				{
					item = new QTableWidgetItem(teams[i]);
					item->setFlags(item->flags() & ~Qt::ItemIsEditable);
					// Insert city name into city name column
					table->setItem(table->rowCount() - 1, A_TEAMNAME_SOUVENIR, item);
				}
				else // Else, insert blank name
				{
					item = new QTableWidgetItem("");
					item->setFlags(item->flags() & ~Qt::ItemIsEditable);
					table->setItem(table->rowCount() - 1, A_TEAMNAME_SOUVENIR, item);
				}
			} // END if purchase table not empty
			else // if purchase table empty
			{
				// Add new row
				table->insertRow(table->rowCount());

				item = new QTableWidgetItem(teams[i]);
				item->setFlags(item->flags() & ~Qt::ItemIsEditable);
				// Insert city name into city name column
				table->setItem(table->rowCount() - 1, A_TEAMNAME_SOUVENIR, item);
			}

			item = new QTableWidgetItem(teams[i]);
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			// Insert city name into key column
			table->setItem(table->rowCount() - 1, AS_KEY, item);

			item = new QTableWidgetItem(souvenirs[j]);
			item->setFlags(item->flags() & ~Qt::ItemIsEditable);
			// Add food name
			table->setItem(table->rowCount() - 1, A_ITEM, item);
			// Add food price
			table->setItem(table->rowCount() - 1, A_PRICE, priceItem);

		} // END for iterate through food list
	} // END for iterate through city list
}
#undef db

// Deletes contents of passed in table
void TableManager::DeleteAllTableRows(QTableWidget *table)
{
	const int ROW_COUNT = table->rowCount();

	for(int index = 0; index < ROW_COUNT; index++)
	{
		table->removeRow(0);
	}
}

void TableManager::ShowTeamInfo(QTableView *table, QString teamName)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString query = "SELECT * FROM information WHERE id = (SELECT teams.id FROM teams WHERE teams.teamNames = '" + teamName + "')";
    model->setQuery(query);

    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Stadium"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Capacity"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Location"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Conference"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Division"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Surface Type"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Roof Type"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Date Opened"));
    table->verticalHeader()->hide();

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->hideColumn(0);
    table->setModel(model);
}
void TableManager::ShowTeamNames(QTableView *table)
{
    QSqlQueryModel * model = new QSqlQueryModel;
    QString query = "SELECT teamNames FROM teams";
    model->setQuery(query);
    model->setHeaderData(0,Qt::Horizontal,QObject::tr("Teams"));

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setModel(model);
}

void TableManager::ShowTeams(QTableView* table, QStringList& available)
{
    QStringListModel* model = new QStringListModel;
    model->setStringList(available);
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QTableView::NoEditTriggers);
    table->setModel(model);
}

void TableManager::InitializePurchaseTable(QTableWidget* purchaseTable, const int &cols, const QStringList &headers)
{
    purchaseTable->clearContents();
    purchaseTable->setColumnCount(cols);
    purchaseTable->setHorizontalHeaderLabels(headers);
    purchaseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    purchaseTable->setEditTriggers(QTableView::NoEditTriggers);
    //purchaseTable->hideColumn(P_KEY);
    purchaseTable->verticalHeader()->hide();

    DeleteAllTableRows(purchaseTable);
}

void TableManager::PopulatePurchaseTable(QTableWidget* purchaseTable, QVector<Souvenir>& teamSouvenirs)
{
    QTableWidgetItem* priceItem;
    QString currentName;
    QString previousName;

    // For the length of the city list
    //for(int teamIndex = 0; teamIndex < teamSouvenirs.size(); teamIndex++)
    //{
     //   int foodListSize = DBManager::instance
                //teamSouvenirs[teamIndex].size();
      //  qDebug() << foodListSize;

        // Iterate through each city's food list
    for(int index = 0; index < teamSouvenirs.size(); index++)
    {
            // Generate food price tablewidgetitem
        priceItem = new QTableWidgetItem(QString::number(teamSouvenirs[index].price, 'f', 2));

            // If list is not empty
        if(purchaseTable->rowCount() != 0)
        {
                // Check to see if there's a match between this row's city name and the previous row's city name
			currentName = DBManager::instance()->GetTeamName(teamSouvenirs[index-1].teamID);
                    //purchaseTable->item(purchaseTable->rowCount() -1, 0)->data(0).toString();
			previousName = DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID);

                // Add a row to the end
            purchaseTable->insertRow(purchaseTable->rowCount());


            bool match = currentName == previousName;

                // If the row names do not match, insert the city name into the name column
            if(!match)
            {
                // Insert city name into city name column
				purchaseTable->setItem(purchaseTable->rowCount() - 1, 0, new QTableWidgetItem(DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID)));
            }
            else // Else, insert blank name
            {
                purchaseTable->setItem(purchaseTable->rowCount() - 1, 0, new QTableWidgetItem(""));
            }
        } // END if purchase table not empty
        else // if purchase table empty
        {
                // Add new row
            purchaseTable->insertRow(purchaseTable->rowCount());

                // Insert city name into city name column
			purchaseTable->setItem(purchaseTable->rowCount() - 1, 0, new QTableWidgetItem(DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID)));
        }

            // Insert city name into key column
            //purchaseTable->setItem(purchaseTable->rowCount() - 1, P_KEY, new QTableWidgetItem(cities->at(cityIndex).GetName()));
            // Add food name
        purchaseTable->setItem(purchaseTable->rowCount() - 1, 1, new QTableWidgetItem(teamSouvenirs[index].name));
            // Add food price
        purchaseTable->setItem(purchaseTable->rowCount() - 1, 2, priceItem);

		QString teamname = DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID);
        QString souName = teamSouvenirs[index].name;
        QString price = QString::number(teamSouvenirs[index].price);
    } // END for iterate through food list
    InsertSpinBoxCol(purchaseTable,0,100,3);
}

void TableManager::InitializeReceiptTable(QTableWidget* receiptTable, const int &cols, const QStringList &headers)
{
    receiptTable->clearContents();
    receiptTable->setColumnCount(cols);
    receiptTable->setHorizontalHeaderLabels(headers);
    receiptTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    receiptTable->setEditTriggers(QTableView::NoEditTriggers);
    receiptTable->verticalHeader()->hide();

    DeleteAllTableRows(receiptTable);
}

void TableManager::PopulateReceiptTable(QTableWidget* receiptTable, QVector<Souvenir>& teamSouvenirs)
{
    QTableWidgetItem* priceItem;
    QTableWidgetItem* qtyItem;
    QTableWidgetItem* totalItem;
    QTableWidgetItem* teamTotal;
    QTableWidgetItem* grandTotal;
    double teamCounter;
    double totalCounter;
    QString currentName;
    QString previousName;
    int souvListSize = teamSouvenirs.size();

    for (int i = 0; i < souvListSize; i++) // remove any souvenirs with qty == 0
    {
        qtyItem = new QTableWidgetItem(QString::number(teamSouvenirs.at(i).purchaseQty));
        if (qtyItem->text().toDouble() == 0)
        {
            teamSouvenirs.removeAt(i);
            i--;
            souvListSize--;
        }
    }

    for(int index = 0; index < teamSouvenirs.size(); index++) // Iterate through full souvenir list
    {     
        priceItem = new QTableWidgetItem(QString::number(teamSouvenirs[index].price, 'f', 2)); // generate price tablewidgetitem
        qtyItem = new QTableWidgetItem(QString::number(teamSouvenirs[index].purchaseQty)); // generate qty tablewidgetitem
        totalItem = new QTableWidgetItem(QString::number((teamSouvenirs[index].price) * (teamSouvenirs[index].purchaseQty), 'f', 2)); // generate total tablewidgetitem

        if(receiptTable->rowCount() != 0) // If list is not empty
        {         
			currentName = DBManager::instance()->GetTeamName(teamSouvenirs[index-1].teamID); // Check to see if there's a match between this row's city name and the previous row's city name
			previousName = DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID);

            receiptTable->insertRow(receiptTable->rowCount()); // Add a row to the end

            bool match = currentName == previousName; // If the row names do not match, insert the city name into the name column
            if(!match)
            {
                teamTotal = new QTableWidgetItem(QString::number(teamCounter, 'f', 2)); // insert team total into table
                receiptTable->setItem(receiptTable->rowCount() - 1, 4, teamTotal);
                receiptTable->setItem(receiptTable->rowCount() - 1, 3, new QTableWidgetItem("Team Total:"));
                receiptTable->insertRow(receiptTable->rowCount());

                teamCounter = 0; //start team counter over
                // Insert city name into city name column
				receiptTable->setItem(receiptTable->rowCount() - 1, 0, new QTableWidgetItem(DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID)));
            }
            else // Else, insert blank name
            {
                receiptTable->setItem(receiptTable->rowCount() - 1, 0, new QTableWidgetItem(""));
            }
        } // END if purchase table not empty
        else // if purchase table empty
        {    
            receiptTable->insertRow(receiptTable->rowCount()); // Add new row
                // Insert city name into city name column
			receiptTable->setItem(receiptTable->rowCount() - 1, 0, new QTableWidgetItem(DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID)));
        }
        if (teamSouvenirs[index].purchaseQty != 0)
        {
            receiptTable->setItem(receiptTable->rowCount() - 1, 1, new QTableWidgetItem(teamSouvenirs[index].name)); // Add food name
            receiptTable->setItem(receiptTable->rowCount() - 1, 2, priceItem); // Add food price
            receiptTable->setItem(receiptTable->rowCount() - 1, 3, qtyItem); // Add food qty
            receiptTable->setItem(receiptTable->rowCount() - 1, 4, totalItem); // add total

			QString teamname = DBManager::instance()->GetTeamName(teamSouvenirs[index].teamID);
            QString souName = teamSouvenirs[index].name;
            QString price = QString::number(teamSouvenirs[index].price);
        }

        teamCounter = teamCounter + (teamSouvenirs[index].price) * (teamSouvenirs[index].purchaseQty); // add to team total
        totalCounter = totalCounter + (teamSouvenirs[index].price) * (teamSouvenirs[index].purchaseQty); // add to grand total
    } // END for iterate through food list

    // generate final team total
    receiptTable->insertRow(receiptTable->rowCount());
    teamTotal = new QTableWidgetItem(QString::number(teamCounter, 'f', 2));
    receiptTable->setItem(receiptTable->rowCount() - 1, 4, teamTotal);
    receiptTable->setItem(receiptTable->rowCount() - 1, 3, new QTableWidgetItem("Team Total:"));

    // generate grand total
    receiptTable->insertRow(receiptTable->rowCount());
    grandTotal = new QTableWidgetItem(QString::number(totalCounter, 'f', 2));
    receiptTable->setItem(receiptTable->rowCount() - 1, 4, grandTotal);
    receiptTable->setItem(receiptTable->rowCount() - 1, 3, new QTableWidgetItem("Grand Total:"));
}


void TableManager::InsertSpinBoxCol(QTableWidget* table, const int min, const int max, const int col)
{
    QSpinBox *sBox;

    purchaseTableSpinBoxes->clear();

    for(int row = 0; row < table->rowCount(); row++)
    {
        QString objectName = QString::number(row);

        sBox = new QSpinBox(table);
        //sBox->setObjectName(objectName);
        sBox->setRange(min, max);
        table->setCellWidget(row, col, sBox);
        purchaseTableSpinBoxes->append(sBox);
    }
}

void TableManager::ClearTable(QTableView *table)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    table->setModel(model);
}

double TableManager::UpdateTotalPrice(QTableWidget* table)
{
    double totalCost = 0;
    for (int i = 0; i < purchaseTableSpinBoxes->size(); i++)
        totalCost+= table->item(i,2)->data(0).toDouble() * (purchaseTableSpinBoxes->at(i)->value());
    return totalCost;
}
