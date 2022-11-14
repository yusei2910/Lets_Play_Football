# Lets_Play_Football
Your agile team is to write an application that stores the given National Football League (NFL) information into an ordered or unordered map (not using the map or unordered map STL or the QT map). The underlying data structure of the map is the choice of the team. The map should store either the stadiums and/or the souvenirs. The application will allow football fans to plan their dream vacation.   

1.	Write at least 10 agile stories (including description, tasks, test scenarios, and story points) before any software development.
2.	Display all the information related to only one particular football team (team name, stadium name, seating capacity, location, conference, division, surface type, stadium roof type, date opened). Do not display information related to the other NFL teams.
3.	Display the list of the NFL teams sorted by team name.
4.	Display the list of NFL stadiums and their corresponding team name sorted by stadium name.
5.	Display the list of only the American Football Conference teams sorted by team name. The National Football Conference teams should not be displayed.
6.	Display the list of only the National Football Conference teams sorted by team name. The American Football Conference teams should not be displayed.
7.	Display the list of only the NFC North teams sorted by team name.  No other teams should be displayed.
8.	Display the list of NFL stadiums sorted by date opened (oldest to newest).  Display the team name and date opened.
9.	Display the list of only the stadiums that have an “open” stadium roof type and their corresponding team name sorted by stadium name. Display the number of stadiums have an “open” roof. If two teams use the same stadium, count the “open” roof just once.
10.	Display the list of NFL stadiums and their corresponding team names sorted by seating capacity (smallest to largest).  Be sure to display seating capacity.  Display the total capacity of all NFL teams. If two teams use the same stadium, count the capacity of the stadium only once.
11.	Display the list of NFL teams, their stadium names, their conference, and their corresponding location sorted by conference name.
12.	Display the list of NFL teams whose surface type is Bermuda Grass. Display the number of NFL teams whose surface type is Bermuda Grass.
13.	Display all the souvenir list related to only one particular football team. 

Planning a vacation:

1.	Provide the capability for a football fan to visit any other team of their choice starting at the Green Bay Packers traveling the shortest distance.  Your Agile team should implement Dijkstra’s or the A* algorithm. Display the total distance travelled.

2.	Provide the capability for a football fan to plan his/her dream vacation by allowing a football fan to choose their starting team and all the other teams they would like to visit using the order specified (determine the shortest distance).  Display the total distance travelled.

3.	Provide the capability to visit the all the teams starting at New England Patriots traveling the shortest distance. Chose the team closest to the New England Patriots and then chose the team closes to that team, etc.
a.	Display the total distance traveled

4.	Provide the capability for a football fan to plan his/her dream vacation by allowing a football fan to choose their starting team
a.	Then allow a football fan to select other teams they wish to visit
b.	Plan the trip starting with the selected team then visit each of the other teams in the most efficient order (recursively choose the team closest to the previous team)    
c.	Display the total distance traveled

5.	Determine the minimum spanning tree (MST) connecting all the NFL stadiums using Prim’s or Kruskal’s algorithm. Display the associated mileage.
6.	Perform a DFS starting at the Minnesota Vikings.  If there is a choice, always choose the shortest distance.  Display the associated mileage.

7.	Perform a BFS starting at Los Angeles Rams.  If there is a choice, always choose the shortest distance. Display the associated mileage.

8.	When taking any trip:
a.	A football fan can purchase multiple souvenirs  
b.	Your Agile team must keep track of the number of souvenirs purchased at each stadium.
c.	Display the total amount spent at each stadium and a grand total for all stadiums visited

9.	Maintenance (administrator only - requires a password to gain access)
a.	Provide the capability to add a new team and its corresponding information by having your program read from an input file given to the class (assume the team has the default souvenirs)
b.	Provide the capability to change the prices of the traditional souvenirs
c.	Provide the capability to add new traditional souvenirs
d.	Provide the capability to delete traditional souvenirs

10.	Provide the ability to modify stadium information including capacity if a team moves into a new stadium. (administrator only) (The Buffalo Bills are planning to move to a new stadium in 2022).
