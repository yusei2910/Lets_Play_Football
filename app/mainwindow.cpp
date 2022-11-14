#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"
#include "tablemanager.h"
#include "layout.h"
#include <functional>
#include <qnamespace.h>
#include "graph.h"
#include "graphDFS.h"
#include "mstGraph.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	DBManager::instance();
	table = new TableManager;
    Layout::instance();
	InitializeLayout();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::SetAdminPurchaseDropDown()
{
	ui->comboBox_admin_receipts->clear();
	QSqlQuery query;
	query.prepare("SELECT DISTINCT purchaseID FROM purchases");
	if (!query.exec())
		qDebug() << "MainWindow::SetAdminPurchaseDropDown(): query failed";
	while (query.next())
		ui->comboBox_admin_receipts->addItem(query.value(0).toString());
}

void MainWindow::InitializeLayout() // sets default pages on program restart
{
	on_pushButton_pages_home_clicked();
	on_comboBox_edit_activated(0);
	ui->pushButton_pages_home->setDisabled(true);
    ui->comboBox_list_sort->addItems(sort);
    ui->comboBox_list_filterteams->addItems(filterTeams);
	ui->comboBox_list_filterstadiums->addItems(filterStadiums);
	SetResources();
}

void MainWindow::SetResources() // imports and assigns layout elements
{
	/*----Fonts----*/
	Layout::instance()->ImportResources();

	QFont mainFont = QFont("OldSansBlack", 16); // main font
	ui->centralwidget->setFont(mainFont);

    QFont homeButtons = QFont("OLD SPORT 02 ATHLETIC NCV", 32); // main page button font
	ui->pushButton_pages_home->setFont(homeButtons);
	ui->pushButton_pages_view->setFont(homeButtons);
	ui->pushButton_pages_plan->setFont(homeButtons);
	ui->pushButton_pages_admin->setFont(homeButtons);
	ui->pushButton_pages_exit->setFont(homeButtons);

	QFont buttons = QFont("OLD SPORT 02 ATHLETIC NCV", 20); // button font
	ui->pushButton_admin_edit->setFont(buttons);
	ui->pushButton_admin_import->setFont(buttons);
	ui->pushButton_admin_receipts->setFont(buttons);
	ui->pushButton_edit_add->setFont(buttons);
	ui->pushButton_edit_cancel->setFont(buttons);
	ui->pushButton_edit_confirm->setFont(buttons);
	ui->pushButton_edit_delete->setFont(buttons);
	ui->pushButton_import->setFont(buttons);
	ui->pushButton_login->setFont(buttons);
	ui->pushButton_plan_add->setFont(buttons);
	ui->pushButton_plan_continue->setFont(buttons);
	ui->pushButton_plan_custom->setFont(buttons);
	ui->pushButton_plan_packers->setFont(buttons);
	ui->pushButton_plan_patriots->setFont(buttons);
	ui->pushButton_plan_remove->setFont(buttons);
	ui->pushButton_plan_sort->setFont(buttons);
	ui->pushButton_pos_cancel->setFont(buttons);
	ui->pushButton_pos_continue->setFont(buttons);
	ui->pushButton_receipt_continue->setFont(buttons);
	ui->pushButton_view_list->setFont(buttons);
	ui->pushButton_view_search->setFont(buttons);

	QFont tables = QFont("MADE TOMMY", 16); // table font
	ui->tableView_edit->setFont(tables);
	ui->tableView_import->setFont(tables);
	ui->tableView_import_2->setFont(tables);
	ui->tableView_import_3->setFont(tables);
	ui->tableView_list->setFont(tables);
    ui->tableView_plan_custom->setFont(tables);
    ui->tableView_plan_route->setFont(tables);
    ui->tableView_pos_trip->setFont(tables);
	ui->tableWidget_receipt->setFont(tables);
	ui->tableView_search_info->setFont(tables);
	ui->tableView_search_souvenirs->setFont(tables);
    ui->tableView_search_teams->setFont(tables);
	ui->tableWidget_edit->setFont(tables);
	ui->tableWidget_pos_purchase->setFont(tables);
	ui->lineEdit_edit_souvenir_name->setFont(tables);
	ui->lineEdit_edit_souvenir_price->setFont(tables);
	ui->lineEdit_edit_souvenir_team->setFont(tables);
	ui->lineEdit_edit_stadium_capacity->setFont(tables);
	ui->lineEdit_edit_stadium_dateopen->setFont(tables);
	ui->lineEdit_edit_stadium_location->setFont(tables);
	ui->lineEdit_edit_stadium_name->setFont(tables);
	ui->lineEdit_edit_stadium_roof->setFont(tables);
	ui->lineEdit_edit_stadium_surface->setFont(tables);
	ui->lineEdit_login_password->setFont(tables);
	ui->lineEdit_login_username->setFont(tables);
	/*----End Fonts----*/
}
void MainWindow::ClearButtons() // resets most program states
{
    ui->pushButton_pages_home->setDisabled(false); // home pages
	ui->pushButton_pages_view->setDisabled(false);
	ui->pushButton_pages_plan->setDisabled(false);
	ui->pushButton_pages_admin->setDisabled(false);

    ui->comboBox_list_sort->setCurrentIndex(NOSORT); // view page
	ui->comboBox_list_filterteams->setCurrentIndex(ALLTEAMS);
	ui->comboBox_list_filterstadiums->setCurrentIndex(ALLSTADIUMS);
	ClearViewLabels();

    ui->pushButton_plan_sort->setVisible(false); // trip planning buttons
	ui->gridWidget_plan_custom->setVisible(false);
	ui->tableView_plan_custom->setVisible(false);
	ui->pushButton_plan_continue->setDisabled(true);
	ui->pushButton_plan_packers->setDisabled(false);
	ui->pushButton_plan_patriots->setDisabled(false);
	ui->pushButton_plan_custom->setDisabled(false);

    ui->formWidget_edit_souvenir->setEnabled(false); // admin buttons
	ui->formWidget_edit_stadium->setEnabled(false);
	ui->pushButton_edit_confirm->setDisabled(true);
	ui->pushButton_edit_cancel->setDisabled(true);
	ui->pushButton_edit_delete->setDisabled(true);
	ui->pushButton_edit_add->setDisabled(false);
	ui->comboBox_edit->setDisabled(false);
	ui->tabWidget_IMPORT->setCurrentIndex(IMPORT);
	ui->comboBox_edit->setCurrentIndex(EDITSOUV);

    ui->lineEdit_edit_souvenir_name->clear(); // line edits
	ui->lineEdit_edit_souvenir_price->clear();
	ui->lineEdit_edit_souvenir_team->clear();
	ui->lineEdit_login_password->clear();
	ui->lineEdit_login_username->clear();
	ui->lineEdit_edit_stadium_capacity->clear();
	ui->lineEdit_edit_stadium_location->clear();
	ui->lineEdit_edit_stadium_name->clear();
	ui->lineEdit_edit_stadium_roof->clear();
	ui->lineEdit_edit_stadium_surface->clear();
	ui->lineEdit_edit_stadium_dateopen->clear();

    ui->label_pos_cost->setText("Total Cost: $0"); // labels
}

void MainWindow::ClearViewLabels()
{
	ui->label_list_totalcapacity->hide();
	ui->label_list_totalgrass->hide();
	ui->label_list_totalroofs->hide();
}

long MainWindow::CalculateDistance(QStringList teams) // calculates trip distance for unsorted custom trips
{
	long temp = 0;
	for (int i = 0; i < teams.size() - 1; i++) // gets dijkstra distance between each city on list progressively
	{
		QString start = teams[i];
		QString end = teams[i + 1];

		Graph<QString> graph;
		graph.GenerateGraph();
		std::vector<QString> vect(graph.Vertices());
		std::vector<QString> dijkstra;
		int costsD[graph.Size()];
		int parentD[graph.Size()];
		graph.DijkstraPathFinder(start,
									 dijkstra, costsD, parentD);

		temp += costsD[graph.FindVertex(end)]; // totals up distance
	}
	return temp;
}

void MainWindow::CreateReceipt(QVector<Souvenir>& souvenirs) // adds quantity values to purchased items
{
    for(int souvIndex = 0; souvIndex < souvenirs.size(); souvIndex++) // loops through the list of souvenirs
	{
        souvenirs.operator[](souvIndex).purchaseQty = table->purchaseTableSpinBoxes->at(souvIndex)->value(); // Add food to item
	}
}

void MainWindow::LaRams()
{
	ClearButtons();
	//ui->pushButton_plan_rams->setDisabled(true);

	//clearTable
	table->ClearTable(ui->tableView_plan_route);  //reset table
	ui->label_plan_distance->setText("Distance"); // reset label

	//Create bfs obj
	bfs bfsObj;
	bfsObj.AddEdges();
	bfsObj.bfsAlgo(19); // starting at La Rams (id: 19)
	//table->showBFSTrip(ui->tableView_plan_route,bfsObj);
	ui->label_plan_bfs->setText("LA Rams Distance(BFS): " +
	QLocale(QLocale::English).toString(bfsObj.GetTotalDistance()) + " miles");
//	ui->label_plan_bfs->setText(QString("LA Rams Distance(BFS): %1").arg(bfsObj.GetTotalDistance()));

	//ui->pushButton_plan_continue->setDisabled(false);
}

void MainWindow::PopulateStadiumInfo(int sortIndex, int teamFilterIndex, int stadiumsFilterIndex)
{

	if (sortIndex < 0 || teamFilterIndex < 0 || stadiumsFilterIndex < 0)
		return;

	ClearViewLabels();

	int capacity = 0;
	int openRoofCount = 0;

	QString sort[] = {"None","teamNames", "conference","stadiumName", "dateOpen", "seatCap"};

	QSqlQuery query;
	QString queryString = "SELECT (SELECT teams.teamNames FROM teams WHERE "
						  "teams.id = information.id) as teamNames, "
						  "stadiumName,seatCap,conference,division,surfaceType,"
						  "roofType,dateOpen FROM information";

	switch(teamFilterIndex)
	{
	case 1: queryString+= " WHERE division LIKE '%" + filterTeams[teamFilterIndex] + "%'";
			break;
	case 2: queryString+= " WHERE division LIKE '%" + filterTeams[teamFilterIndex] + "%'";
			break;
	case 3: queryString+= " WHERE division = '" + filterTeams[teamFilterIndex] + "'";
			break;
	case 4: queryString+= " WHERE surfaceType = '" + filterTeams[teamFilterIndex] + "'";
			break;
	default: break;
	}




	 if (stadiumsFilterIndex == OPENROOF)
	 {
		if (teamFilterIndex==0)
			queryString+= " WHERE roofType = 'Open' ";
		else
			queryString+= " AND roofType = 'Open'";
	 }

	 QString queryStringWithoutOrder = queryString;
	 if (sortIndex != 0)
		queryString += " ORDER BY " + sort[sortIndex] + " ASC";


	query.exec(queryString);


	QSqlQueryModel* model = new QSqlQueryModel;
	model->setQuery(query);
	model->setHeaderData(0, Qt::Horizontal, QObject::tr("Team Name"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("Stadium Name"));
	model->setHeaderData(2, Qt::Horizontal, QObject::tr("Seat Cap"));
	model->setHeaderData(3, Qt::Horizontal, QObject::tr("Conference"));
	model->setHeaderData(4, Qt::Horizontal, QObject::tr("Division"));
	model->setHeaderData(5, Qt::Horizontal, QObject::tr("Surface Type"));
	model->setHeaderData(6, Qt::Horizontal, QObject::tr("Roof Type"));
	model->setHeaderData(7, Qt::Horizontal, QObject::tr("Date Open"));

	ui->tableView_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableView_list->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_list->verticalHeader()->hide();

	ui->tableView_list->setModel(model);

	QLocale c(QLocale::C);  // to set the string with "," (ex: 12,500) into int
	query.exec(queryStringWithoutOrder + " GROUP BY stadiumName");
	while(query.next())
	{

		capacity += c.toInt(query.value(2).toString());
		if (query.value(6).toString() == "Open")
			openRoofCount++;
	}

	if (sortIndex == CAPACITY)
	{
		ui->label_list_totalcapacity->setText("Total Capacity: " + QString("%L1").arg(capacity));
		ui->label_list_totalcapacity->show();
	}

	if (teamFilterIndex == BERMUDAGRASS)
	{
		ui->label_list_totalgrass->setText("Total Bermuda Grass Teams: " + QString::number(ui->tableView_list->model()->rowCount()));
		ui->label_list_totalgrass->show();
	}

	if (stadiumsFilterIndex == OPENROOF)
	{
		ui->label_list_totalroofs->setText("Total Open Roof Stadiums: " + QString::number(openRoofCount));
		ui->label_list_totalroofs->show();
	}


}

void MainWindow::PopulateTeams()
{
	QStringList teamList;
	DBManager::instance()->GetTeams(teamList);
	table->ShowTeams(ui->tableView_search_teams, teamList);
}

void MainWindow::PopulateSouvenirs(QString team)
{
	QSqlQueryModel *model = new QSqlQueryModel;
	model->setQuery("SELECT items, price FROM souvenir WHERE id = (SELECT teams.id FROM teams WHERE teams.teamNames = '" + team + "')");

	model->setHeaderData(0, Qt::Horizontal, QObject::tr("Souvenir"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("Price"));
    ui->tableView_search_souvenirs->verticalHeader()->hide();

	ui->tableView_search_souvenirs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableView_search_souvenirs->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableView_search_souvenirs->setModel(model);
}

void MainWindow::RecursiveAlgo(QString start, QStringList& selectedList, QStringList& availableList, long& distance)
{
	if (availableList.size() == 0)
		return;

	Graph<QString> graph;
	graph.GenerateGraph();
	std::vector<QString> vect(graph.Vertices());
	std::vector<QString> dijkstra;
	int costsD[graph.Size()];
	int parentD[graph.Size()];
	graph.DijkstraPathFinder(start,
								 dijkstra, costsD, parentD);

	int smallestIndex = 0;
	int shortestPath = INT_MAX;
	for (int i = 0 ; i < (int) vect.size(); i++) {
		std::vector<QString> path = graph.ReturnPath(start, vect[i], parentD);
		if (shortestPath > costsD[graph.FindVertex(vect[i])])
		{
			if(!selectedList.contains(vect[i]) && availableList.contains(vect[i]))
			{
				shortestPath = costsD[graph.FindVertex(vect[i])];
				smallestIndex = i;
			}
		}
	   // qDebug() << "Arizona Cardinals to " << vect[i] << "...\nPath: ";
	   // qDebug() << path;
	   // qDebug() << "\nTotal Distance: " << costsD[graph.findVertex(vect[i])] << "\n\n";
	}

	distance+= costsD[graph.FindVertex(vect[smallestIndex])];
	selectedList.push_back(vect[smallestIndex]);
	for (int i = 0; i < availableList.size(); i++)
	{
		if(availableList[i] == vect[smallestIndex])
		{
			availableList.removeAt(i);
		}
	}

	RecursiveAlgo(vect[smallestIndex],selectedList,availableList,distance);
}

bool MainWindow::isValid(QString cur, QString prev)
{
	bool isNum;
	cur.toDouble(&isNum);
	QRegExp regNum("[0-9]{0,255}[.]{1}[0-9]{0,2}");
	QRegExp regStr("[A-Za-z_ ]{0,255}");
	if (regNum.exactMatch(prev)) {
		if (regNum.exactMatch(cur)) {
			return true;
		} else {
			return false;
		}
	} else if (regStr.exactMatch(prev)) {
		return false;
//		if (regStr.exactMatch(cur)) {
//			return true;
//		} else {
//			return false;
//		}
	}
	qDebug() << "Something went wrong: MainWindow::isValid(QString, QString)";
	assert(false);
}

QString MainWindow::toUpperCase(const QString &str)
{
	QStringList parts = str.split(" ", QString::SkipEmptyParts);
	for (int i = 0; i < parts.size(); i++)
		parts[i].replace(0, 1, parts[i][0].toUpper());

	return parts.join(" ");
}


/*----NAVIGATION----*/
void MainWindow::on_pushButton_pages_home_clicked()
{
    ui->stackedWidget_pages->setCurrentIndex(HOME);
	ClearButtons();
    ui->pushButton_pages_home->setDisabled(true);
}

void MainWindow::on_pushButton_pages_view_clicked()
{
    ui->stackedWidget_pages->setCurrentIndex(VIEW);
	ClearButtons();
    on_pushButton_view_search_clicked();
    ui->pushButton_pages_view->setDisabled(true);
}
    void MainWindow::on_pushButton_view_search_clicked()
    {
        ui->stackedWidget_view_pages->setCurrentIndex(SEARCH);
        ui->pushButton_view_search->setDisabled(true);
        ui->pushButton_view_list->setDisabled(false);

		PopulateSouvenirs("");
        table->ShowTeamInfo(ui->tableView_search_info,"");
        ui->tableView_search_info->hideColumn(0);
		PopulateTeams();
    }

    void MainWindow::on_pushButton_view_list_clicked()
    {
        ClearButtons();
        ui->pushButton_pages_view->setDisabled(true);

        ui->stackedWidget_view_pages->setCurrentIndex(LIST);
        ui->pushButton_view_search->setDisabled(false);
        ui->pushButton_view_list->setDisabled(true);

		PopulateStadiumInfo(0,0,0);
    }

void MainWindow::on_pushButton_pages_plan_clicked()
{
    ui->stackedWidget_pages->setCurrentIndex(PLAN);
	ClearButtons();
    ui->pushButton_pages_plan->setDisabled(true);
	table->ClearTable(ui->tableView_plan_custom);
	table->ClearTable(ui->tableView_plan_route);

    on_pushButton_plan_packers_clicked();

    //sets Total Dist for LA rams
	LaRams();
	QSqlQuery query;
	dfs::GraphDFS<QString> graphDFS;
	graphDFS.GenerateGraph();
	std::vector<QString> temp;
	int distanceDFS = graphDFS.dfs("Minnesota Vikings", temp);
    ui->label_plan_dfs->setText("Vikings Trip Distance(DFS): " +
								QLocale(QLocale::English).toString(distanceDFS) + " miles");

	mstGraph graph;
    vector<mstEdge> mstEdges;
	graph.GetMST(mstEdges);
	int distance = graph.GetMSTdistance();
    ui->label_plan_mst->setText("Total Distance (MST): "+ QLocale(QLocale::English).toString(distance) +" miles");
}

    void MainWindow::on_pushButton_plan_continue_clicked()
    {
        if(selectedTeams.size() < 2)
            return;
        ui->stackedWidget_pages->setCurrentIndex(POS);
        QVector<Souvenir> tempCart;
        DBManager::instance()->CreateShoppingList(selectedTeams,tempCart);

		table->ClearTable(ui->tableWidget_pos_purchase);
        QStringList headers;
        headers.append("Team");
        headers.append("Souvenir");
        headers.append("Price");
        headers.append("Quantity");
        table->InitializePurchaseTable(ui->tableWidget_pos_purchase,4,headers);
        table->PopulatePurchaseTable(ui->tableWidget_pos_purchase,tempCart);
		table->ShowTeams(ui->tableView_pos_trip,selectedTeams);

        for (int i = 0; i < table->purchaseTableSpinBoxes->size(); i++)
        {
			connect(table->purchaseTableSpinBoxes->at(i), SIGNAL(valueChanged(int)), this, SLOT(UpdateCartTotal()));
        }

        for (int i = 0; i < tempCart.size(); i++)
        {
            souvenirList.insert({tempCart[i].teamID, tempCart[i]});
        }
}

    void MainWindow::on_pushButton_pos_cancel_clicked()
    {
        on_pushButton_pages_home_clicked();
    }

    void MainWindow::on_pushButton_pos_continue_clicked()
    {
        ui->stackedWidget_pages->setCurrentIndex(RECEIPT);
        QVector<Souvenir> tempCart;
        DBManager::instance()->CreateShoppingList(selectedTeams,tempCart);
        CreateReceipt(tempCart);

		table->ClearTable(ui->tableWidget_receipt);
        QStringList headers;
        headers.append("Team");
        headers.append("Souvenir");
        headers.append("Price");
        headers.append("Quantity");
        headers.append("Total");
        table->InitializeReceiptTable(ui->tableWidget_receipt,5,headers);
        table->PopulateReceiptTable(ui->tableWidget_receipt,tempCart);

		DBManager::instance()->AddPurchases(tempCart);
        for (int i = 0; i < table->purchaseTableSpinBoxes->size(); i++)
        {
			connect(table->purchaseTableSpinBoxes->at(i), SIGNAL(valueChanged(int)), this, SLOT(UpdateCartTotal()));
        }
    }

    void MainWindow::on_pushButton_receipt_continue_clicked()
    {
        on_pushButton_pages_home_clicked();
    }

void MainWindow::on_pushButton_pages_admin_clicked()
{
    ui->stackedWidget_pages->setCurrentIndex(LOGIN);
	ClearButtons();
	on_pushButton_admin_import_clicked();
    ui->pushButton_pages_admin->setDisabled(true);
	connect(ui->lineEdit_login_password, SIGNAL(returnPressed()), ui->pushButton_login, SLOT(click()));
}

    void MainWindow::on_pushButton_login_clicked()
    {
        // Check login credentials
		if(DBManager::instance()->CheckLogin(ui->lineEdit_login_username->text(), ui->lineEdit_login_password->text()))
        {
            // Change index to admin section
            ui->stackedWidget_pages->setCurrentIndex(ADMIN);
			table->AdminInfoTable(ui->tableView_import);
			table->AdminDistTable(ui->tableView_import_2);
			table->AdminSouvTable(ui->tableView_import_3);
			disconnect(ui->lineEdit_login_password, SIGNAL(returnPressed()), ui->pushButton_login, SLOT(click()));
        }
        else
        {
            // Notify user if username and password are incorrect
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Username and/or password is incorrect."));
        }

        // Clear username and password fields
        ui->lineEdit_login_username->clear();
        ui->lineEdit_login_password->clear();
    }

    void MainWindow::on_pushButton_admin_import_clicked()
    {
        table->AdminInfoTable(ui->tableView_import);
        table->AdminDistTable(ui->tableView_import_2);
        table->AdminSouvTable(ui->tableView_import_3);
        ui->stackedWidget_admin_pages->setCurrentIndex(IMPORT);
        ui->pushButton_admin_import->setDisabled(true);
        ui->pushButton_admin_edit->setDisabled(false);
        ui->pushButton_admin_receipts->setDisabled(false);
    }

		void MainWindow::on_pushButton_import_clicked()
		{
			DBManager::instance()->ImportTeams();
			table->AdminInfoTable(ui->tableView_import);
			table->AdminDistTable(ui->tableView_import_2);
			table->AdminSouvTable(ui->tableView_import_3);
		}

    void MainWindow::on_pushButton_admin_edit_clicked()
    {

        disconnect(this, &MainWindow::EmittedSignal, this, &MainWindow::UpdateTable);
        disconnect(ui->tableWidget_edit, &QTableWidget::cellChanged, nullptr, nullptr);

        ui->stackedWidget_admin_pages->setCurrentIndex(EDIT);
        on_comboBox_edit_activated(EDITSOUV);
        ui->pushButton_admin_import->setDisabled(false);
        ui->pushButton_admin_edit->setDisabled(true);
        ui->pushButton_admin_receipts->setDisabled(false);
		table->InitializeAdminEditTable(ui->tableWidget_edit);
		table->PopulateAdminEditTable(ui->tableWidget_edit);
    }

	void MainWindow::on_pushButton_admin_receipts_clicked()
	{
		SetAdminPurchaseDropDown();
		ui->stackedWidget_admin_pages->setCurrentIndex(RECEIPTS);
		ui->pushButton_admin_import->setDisabled(false);
		ui->pushButton_admin_edit->setDisabled(false);
		ui->pushButton_admin_receipts->setDisabled(true);
	}

    void MainWindow::on_comboBox_edit_activated(int index)
    {
        if (index == EDITSOUV)
        {
            ui->comboBox_edit->setCurrentIndex(EDITSOUV);
			ui->stackedWidget_edit->setCurrentIndex(EDITSOUV);
            ui->formWidget_edit_souvenir->setVisible(true);
			ui->formWidget_edit_stadium->setVisible(false);
			table->InitializeAdminEditTable(ui->tableWidget_edit);
			table->PopulateAdminEditTable(ui->tableWidget_edit);
            ui->pushButton_edit_add->setVisible(true);
            ui->pushButton_edit_delete->setVisible(true);
            ui->pushButton_edit_add->clicked();
        }
        else
        {
			ui->stackedWidget_edit->setCurrentIndex(EDITSTAD);
            ui->formWidget_edit_souvenir->setVisible(false);
            ui->formWidget_edit_stadium->setVisible(true);
			ui->tableView_edit->verticalHeader()->hide();
			table->AdminInfoTable(ui->tableView_edit);
			ui->pushButton_edit_add->setDisabled(true);
            ui->pushButton_edit_add->setVisible(false);
            ui->pushButton_edit_delete->setVisible(false);
            ui->pushButton_edit_cancel->setVisible(true);
            ui->pushButton_edit_confirm->setVisible(true);

        }
    }

void MainWindow::on_pushButton_pages_exit_clicked() // exit button
{
    QApplication::quit();
}
/*----END NAVIGATION----*/

/*----HELPER FUNCTIONS----*/

void MainWindow::on_pushButton_edit_add_clicked() // admin add button
{
    ui->formWidget_edit_souvenir->setEnabled(true);
    ui->formWidget_edit_stadium->setDisabled(false);
    ui->pushButton_edit_add->setDisabled(true);
    ui->pushButton_edit_delete->setDisabled(true);
    ui->pushButton_edit_cancel->setEnabled(true);
    ui->comboBox_edit->setDisabled(true);

	if (ui->stackedWidget_edit->currentIndex() == EDITSOUV) {
		ui->lineEdit_edit_souvenir_team->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_ ]{0,60}"), this));
		ui->lineEdit_edit_souvenir_name->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_ ]{0,60}"), this));
		ui->lineEdit_edit_souvenir_price->setValidator(new QRegExpValidator(QRegExp("[0-9]{0,60}[.]{1}[0-9]{0,2}"), this));
	}
    // code + error checking

    ui->pushButton_edit_confirm->setEnabled(true);

}

void MainWindow::on_pushButton_edit_confirm_clicked()
{
	disconnect(this, &MainWindow::EmittedSignal, this, &MainWindow::UpdateTable);
	disconnect(ui->tableWidget_edit, &QTableWidget::cellChanged, nullptr, nullptr);

	bool ok = false;
	if (ui->stackedWidget_edit->currentIndex() == EDITSOUV) {
		QString teamName = ui->lineEdit_edit_souvenir_team->text();
		QString item = ui->lineEdit_edit_souvenir_name->text();
		QString price = ui->lineEdit_edit_souvenir_price->text();

		int temp = (price.size() - 1) - price.indexOf('.');

		if (!price.contains(".") && !price.isEmpty())
			price += ".00";
		else if (temp == 1)
			price += "0";
		else if (temp == 0 && !price.isEmpty())
			price += "00";

#define db() DBManager::instance()
		if (!db()->isTeamExist(teamName) ||
			db()->isSouvenirExist(teamName, item) || item.isEmpty() ||
				price.isEmpty()) {
			QMessageBox::warning(this, tr("Notice"),
					tr("There was an error with your query.\nPlease try again."));
		} else {
			db()->AddSouvenir(teamName, item, price);
			table->InitializeAdminEditTable(ui->tableWidget_edit);
			table->PopulateAdminEditTable(ui->tableWidget_edit);
		}
#undef db
	} else if (ui->stackedWidget_edit->currentIndex() == EDITSTAD) {
		ui->pushButton_edit_add->setDisabled(true);

		QString stadiumName = toUpperCase(ui->lineEdit_edit_stadium_name->text());
		int cap = ui->lineEdit_edit_stadium_capacity->text().replace(",", "").toInt(&ok);
		QString loc = toUpperCase(ui->lineEdit_edit_stadium_location->text());
		QString surface = toUpperCase(ui->lineEdit_edit_stadium_surface->text());
		QString roofType = toUpperCase(ui->lineEdit_edit_stadium_roof->text());
		QString dateOpen = ui->lineEdit_edit_stadium_dateopen->text();
		QModelIndexList selection = ui->tableView_edit->selectionModel()->selectedRows();

		int id = selection[0].row() + 1;

		QString capacity = QLocale(QLocale::English).toString(cap);

		if (stadiumName.isEmpty() || loc.isEmpty() ||
				surface.isEmpty() || roofType.isEmpty() || dateOpen.isEmpty() || !ok) {
			QMessageBox::warning(this, tr("Notice"),
					tr("There was an error with your query.\nPlease try again."));
		} else {
			DBManager::instance()->UpdateInformation(id, stadiumName, capacity, loc, surface, roofType, dateOpen);
			table->AdminInfoTable(ui->tableView_edit);
		}
	}
	ClearButtons();
	ui->pushButton_pages_admin->setDisabled(true);
}

void MainWindow::on_pushButton_edit_cancel_clicked()
{
	ClearButtons();
	ui->pushButton_pages_admin->setDisabled(true);
}

void MainWindow::on_pushButton_plan_packers_clicked()
{
	ClearButtons();
	ui->label_plan_distance->setText("Trip Distance: ");

	//disable Continue
	ui->pushButton_plan_continue->setDisabled(true);

	ui->pushButton_pages_plan->setDisabled(true);
	ui->pushButton_plan_packers->setDisabled(true);
	ui->gridWidget_plan_custom->setVisible(true);
	ui->tableView_plan_custom->setVisible(true);
	//clears the routes table
	table->ClearTable(ui->tableView_plan_route);
	availableTeams.clear();
	selectedTeams.clear();

	DBManager::instance()->GetTeams(availableTeams);
	availableTeams.removeAll("Green Bay Packers");

	table->ShowTeams(ui->tableView_plan_custom, availableTeams);
}

void MainWindow::on_pushButton_plan_patriots_clicked()
{
	ClearButtons();
	ui->label_plan_distance->setText("Trip Distance: ");// sets default label

	ui->pushButton_pages_plan->setDisabled(true);
	ui->pushButton_plan_patriots->setDisabled(true);

	availableTeams.clear(); //clears avaialableTeams
	selectedTeams.clear();  //Clears selected teams
	DBManager::instance()->GetTeams(selectedTeams); // all teams names

	long totalDistance = 0; // initialize total distance

	QString startingTeam = "New England Patriots";
	selectedTeams.removeAll("New England Patriots");
	QStringList sortedList; // temp list
	sortedList.push_back(startingTeam);

	RecursiveAlgo(startingTeam, sortedList, selectedTeams,totalDistance);

	selectedTeams = sortedList;
	table->ShowTeams(ui->tableView_plan_route, selectedTeams);
	ui->label_plan_distance->setText("Trip Distance: " + QString::number(totalDistance) + " miles");

	ui->pushButton_plan_continue->setDisabled(false);
}

void MainWindow::on_pushButton_plan_custom_clicked()
{
	ClearButtons();
	ui->label_plan_distance->setText("Trip Distance: ");

	availableTeams.clear();
	selectedTeams.clear();

	DBManager::instance()->GetTeams(availableTeams);

	ui->pushButton_pages_plan->setDisabled(true);
	ui->pushButton_plan_sort->setVisible(true);
	ui->gridWidget_plan_custom->setVisible(true);
	ui->tableView_plan_custom->setVisible(true);
	ui->pushButton_plan_custom->setDisabled(true);

	table->ShowTeamNames(ui->tableView_plan_custom);
	table->ShowTeams(ui->tableView_plan_route,selectedTeams);

}

/*----END HELPER FUNCTIONS----*/

void MainWindow::on_tableView_search_teams_doubleClicked(const QModelIndex &index)
{
	PopulateSouvenirs(index.data().toString());
	table->ShowTeamInfo(ui->tableView_search_info, index.data().toString());
}

void MainWindow::on_comboBox_list_sort_currentIndexChanged(int index)
{
	PopulateStadiumInfo(index, ui->comboBox_list_filterteams->currentIndex(), ui->comboBox_list_filterstadiums->currentIndex());
}

void MainWindow::on_comboBox_list_filterteams_currentIndexChanged(int index)
{
	PopulateStadiumInfo(ui->comboBox_list_sort->currentIndex(), index, ui->comboBox_list_filterstadiums->currentIndex());
}

void MainWindow::on_comboBox_list_filterstadiums_currentIndexChanged(int index)
{
	PopulateStadiumInfo(ui->comboBox_list_sort->currentIndex(), ui->comboBox_list_filterteams->currentIndex(), index);
}

void MainWindow::on_tableWidget_edit_cellClicked(int row, int col)
{
	on_pushButton_edit_cancel_clicked();
	static int rowStat;
	static int colStat;
	rowStat = row;
	colStat = col;

	ui->pushButton_edit_delete->setDisabled(false);
	connect(ui->pushButton_edit_delete, &QPushButton::clicked, [this]() {
		emit this->EmittedDelSignal(rowStat, colStat);
	});
	connect(this, &MainWindow::EmittedDelSignal, this, &MainWindow::ProcessDelete);
}

#define table(row, column) ui->tableWidget_edit->item(row, column)->text()
void MainWindow::ProcessDelete(int row, int /*col*/)
{
	disconnect(this, &MainWindow::EmittedSignal, this, &MainWindow::UpdateTable);
	disconnect(ui->tableWidget_edit, &QTableWidget::cellChanged, nullptr, nullptr);
	disconnect(ui->pushButton_edit_delete, &QPushButton::clicked, nullptr, nullptr);
	disconnect(this, &MainWindow::EmittedDelSignal, this, &MainWindow::ProcessDelete);

	QString teamName;
	QString item = table(row, table->A_ITEM);
	for (int i = row; i >= 0; i--) {
		if (table(i, table->A_TEAMNAME_SOUVENIR) != QString{}) {
			teamName = table(i, table->A_TEAMNAME_SOUVENIR);
			break;
		}
	}

	DBManager::instance()->DeleteSouvenir(teamName, item);
	table->InitializeAdminEditTable(ui->tableWidget_edit);
	table->PopulateAdminEditTable(ui->tableWidget_edit);
	ui->pushButton_edit_delete->setDisabled(true);
}
#undef table

void MainWindow::on_tableWidget_edit_doubleClicked(const QModelIndex &index)
{
	static QString temp;
	temp = index.data().toString();

	connect(ui->tableWidget_edit, &QTableWidget::cellChanged, [this](int row, int column) {
		emit this->EmittedSignal(row, column, temp);
	});

	connect(this, &MainWindow::EmittedSignal, this, &MainWindow::UpdateTable);
}

#define table(row, column) ui->tableWidget_edit->item(row, column)->text()
void MainWindow::UpdateTable(int row, int column, QString prev)
{
	disconnect(this, &MainWindow::EmittedSignal, this, &MainWindow::UpdateTable);
	disconnect(ui->tableWidget_edit, &QTableWidget::cellChanged, nullptr, nullptr);

	QString entry = table(row, column);

	if (!isValid(entry, prev)) {
		ui->tableWidget_edit->item(row, column)->setText(prev);
		QMessageBox::warning(this, tr("Warning"),
							 tr("That query was not valid, please try again"));
	} else {
		QString teamName;
		QString item = table(row, column - 1);
		for (int i = row; i >= 0; i--) {
			if (table(i, 1) != QString{}) {
				teamName = table(i, 1);
				break;
			}
		}

		DBManager::instance()->UpdateSouvenirPrice(teamName,
						   table(row, column - 1), table(row, column));
	}
}
#undef table

void MainWindow::on_tableView_edit_doubleClicked(const QModelIndex &index)
{
	ui->lineEdit_edit_stadium_name->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_ '&]{0,60}"), this));
	// ^(?=.)(\d{1,3}(,\d{3})*)?(\.\d+)?$ (commas required)
	// ^(\\d+|\\d{1,3}(,\\d{3})*)(\\.\\d+)?$ (commas not required)
	ui->lineEdit_edit_stadium_capacity->setValidator(new QRegExpValidator(QRegExp("[0-9]{0,60}"), this));
	ui->lineEdit_edit_stadium_location->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_ ]{0,60}[,]{1}[A-Za-z_ ]{0,60}"), this));
	ui->lineEdit_edit_stadium_surface->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_ -]{0,60}"), this));
	ui->lineEdit_edit_stadium_roof->setValidator(new QRegExpValidator(QRegExp("[A-Za-z_ ]{0,60}"), this));
	ui->lineEdit_edit_stadium_dateopen->setValidator(new QRegExpValidator(QRegExp("[0-9]{0,4}"), this));
	ui->formWidget_edit_stadium->setDisabled(false);
	ui->pushButton_edit_confirm->setDisabled(false);
	ui->pushButton_edit_cancel->setDisabled(false);
	ui->pushButton_edit_delete->setDisabled(true);
	QSqlQuery query;
	query.prepare("SELECT stadiumName, seatCap, location, surfaceType, roofType, dateOpen FROM information WHERE information.id = :id");
	query.bindValue(":id", index.row() + 1);

	if (query.exec()) {
		query.first();
		ui->lineEdit_edit_stadium_name->setText(query.value(0).toString());
		ui->lineEdit_edit_stadium_capacity->setText(query.value(1).toString());
		ui->lineEdit_edit_stadium_location->setText(query.value(2).toString());
		ui->lineEdit_edit_stadium_surface->setText(query.value(3).toString());
		ui->lineEdit_edit_stadium_roof->setText(query.value(4).toString());
		ui->lineEdit_edit_stadium_dateopen->setText(query.value(5).toString());
	} else {
		qDebug() << "MainWindow::on_tableView_edit_doubleClicked(const QModelIndex&) : query failed";
	}
}

void MainWindow::on_pushButton_plan_add_clicked()
{
    if(!(ui->pushButton_plan_packers->isEnabled()))
    {
        //disables the plan continue
        ui->pushButton_plan_continue->setDisabled(false);
        if(ui->tableView_plan_custom->currentIndex().row() >= 0 && ui->tableView_plan_custom->currentIndex().row() < availableTeams.size() && selectedTeams.size() <=1)
        {
            QString selectedString;

            selectedString = availableTeams[ui->tableView_plan_custom->currentIndex().row()];

            availableTeams.removeAt(ui->tableView_plan_custom->currentIndex().row());

            QStringList selected;
            selected.push_back("Green Bay Packers");
            selectedTeams.push_back(selectedString);
			long totalDist = 0;
			RecursiveAlgo("Green Bay Packers",selected,selectedTeams,totalDist);
            selectedTeams = selected;
            ui->label_plan_distance->setText("Trip Distance: " + QString::number(totalDist) + " miles");
            //selectedTeams.clear
        }
    }

    else if (ui->tableView_plan_custom->currentIndex().row() >= 0 && ui->tableView_plan_custom->currentIndex().row() < availableTeams.size())
    {
        QString selectedString = availableTeams[ui->tableView_plan_custom->currentIndex().row()];
        availableTeams.removeAt(ui->tableView_plan_custom->currentIndex().row());
        selectedTeams.push_back(selectedString);
		long customDistance = CalculateDistance(selectedTeams);
        ui->label_plan_distance->setText("Trip Distance: " + QString::number(customDistance) + " miles");
    }
	table->ShowTeams(ui->tableView_plan_custom, availableTeams);
	table->ShowTeams(ui->tableView_plan_route, selectedTeams);

    if(selectedTeams.size() < 2)
        ui->pushButton_plan_continue->setDisabled(true);
    else
        ui->pushButton_plan_continue->setDisabled(false);
}

void MainWindow::on_pushButton_plan_remove_clicked()
{
    if(!(ui->pushButton_plan_packers->isEnabled()))
    {
        ui->pushButton_plan_continue->setDisabled(true);
        selectedTeams.clear();
        availableTeams.clear();
		table->ClearTable(ui->tableView_plan_route);
        DBManager::instance()->GetTeams(availableTeams);
        availableTeams.removeAll("Green Bay Packers");
        ui->pushButton_plan_add->setDisabled(false);
        ui->label_plan_distance->setText("Trip Distance: ");
    }
    if (ui->tableView_plan_route->currentIndex().row() >= 0 && ui->tableView_plan_route->currentIndex().row() < availableTeams.size())
    {
        QString selectedString = selectedTeams[ui->tableView_plan_route->currentIndex().row()];
        selectedTeams.removeAt(ui->tableView_plan_route->currentIndex().row());
        availableTeams.push_back(selectedString);
		long customDistance = CalculateDistance(selectedTeams);
        ui->label_plan_distance->setText("Trip Distance: " + QString::number(customDistance) + " miles");
    }
	table->ShowTeams(ui->tableView_plan_custom, availableTeams);
	table->ShowTeams(ui->tableView_plan_route, selectedTeams);

    if(selectedTeams.size() < 2)
        ui->pushButton_plan_continue->setDisabled(true);

}

void MainWindow::on_pushButton_plan_sort_clicked()
{
    if (selectedTeams.size() > 1)
    {
        long totalDistance = 0;

        QString startingTeam = selectedTeams[0];
        selectedTeams.removeFirst();
        QStringList sortedList;
        sortedList.push_back(startingTeam);

		RecursiveAlgo(startingTeam, sortedList, selectedTeams,totalDistance);

        selectedTeams = sortedList;
		table->ShowTeams(ui->tableView_plan_route, selectedTeams);
        ui->label_plan_distance->setText("Trip Distance: " + QString::number(totalDistance) + " miles");
    }
}

void MainWindow::on_comboBox_admin_receipts_currentIndexChanged(int index)
{
	table->AdminPuchaseTable(ui->tableView_admin_receipts, index);
}

void MainWindow::UpdateCartTotal()
{
	double total = table->UpdateTotalPrice(ui->tableWidget_pos_purchase);
	QString totalString = QString::number(total,'f',2);
	ui->label_pos_cost->setText("Total Cost: $" + totalString);
}
