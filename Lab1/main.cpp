/*	* By			: Muhammad Syafiq Izzat b. Hussin
	*
	* -------------------------
	* rF3.Calendar
	* -------------------------
	*
	* Description: A calendar program in C++.
	* An experimental program compiled using Microsoft Visual Studio 2013 Pro (Dreamspark)
	* The whole project/solution only compatible with stated IDE. Dev-C++ will not able to compile this due to functions limited to this *compiler like (localtime_s) but with proper porting, this program might work in Dev-C++.
	* Using external library: PDCurses for Win32
	* May contains examples from PDCurses source file with some modifications.
	* To compile this program, it require pdcurses.dll (in same folder with exe files) and pdcurses.lib as a linker library.
	*
	* LICENSE
	* This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
	* To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

	* LAST MODIFIED	: 19/3/14 16:38
	* LATEST BUILD	: 19/3/14 16:38
	*/

#include <curses.h>								// External library used to increase user experience console interface
#include <cstdlib>								// Basic header
#include <ctime>								// To get current month (during startup asking for month)
#include <vector>								// Dynamic arrays
#include <cmath>								// Some math calculation used in the program. Specifically, calculate page number.
#include <fstream>								// For save to file
#include <iomanip>								// For time format (eg. 0:0 to 00:00)
#include <algorithm>

// Creating a global PDCurses window
WINDOW *wcal, *wstatus;
#include "Calendar.h"											// Calendar.h is below the WINDOW because it need the WINDOW pointer for its function

// Global variables/constants
int max_y, max_x, selected_mon = 0, selected_year = 0;			// max_x/y is the maximum size of console window
// int filter_day = 0, filter_mon = 0, filter_year = 0;			// Variable for filters. Not use it for now.
bool event_saved = false;										// A variable to check whether the event are saved or not

// Prototypes of the functions below main function
void titlebar();
void show_calendar(bool, int, int);
char *mon_name(int);
void menu();
void statuswin();
void update_status();
void create_new_event(bool edit, int z_index = 0);
void change_mon();
void view_event();
void formatted_time(int, int, int, int);
void wformatted_time(WINDOW*, int, int, int, int);
void list_events(int day_filter = 0, int mon_filter = 0, int year_filter = 0);
void vector_erase(int);
void event_details(int);
void save_to_file();																// Lab 2
void swap(int, int);																// Lab 3
void set_month();

// The main class of the event
class my_event {
	public:
		// Added 2 properties: hour and min for time.
		int day, year, min, hour, nmon;
		char name[30], loc[30];				// Only date, name, location and description available in this class. Totally forgotten about time but it's too late to add now which the program already stable and adding "time" might take longer to finish and may cause other bug(s) appear.

		// Behaviours for LAB2
		void delete_event(int z_index) {	// z-index is the variable used for vector index. 
			vector_erase(z_index);
		}

		void print_details() {
			save_to_file();
		}

		void show_details(int z_index) {
			event_details(z_index);
		}

		void edit_event(int z_index) {
			create_new_event(true, z_index);
		}

		// These function is used to fix a bug related to pdcurses library. Find BUG001 for details
		void save_name(char *rawname) {
			strcpy_s(name, rawname);
		}

		void save_loc(char *rawloc) {
			strcpy_s(loc, rawloc);
		}

		// Disabled due to BUG002.
		/*void save_desc(char *rawdesc) {
			strcpy_s(desc, rawdesc);
		}*/
};

// Declaring data as a global object of my_event
my_event data;

// Declare vector as a global object
std::vector<my_event> myVector;

// Functions for sortings
// Based on http://stackoverflow.com/questions/1380463/sorting-a-vector-of-custom-objects
// Sort by day function
struct SortDay {
	bool operator()(const my_event& lx, const my_event& rx) const {
		return lx.day < rx.day;
	}
};

struct SortMonth {
	bool operator()(const my_event& lx, const my_event& rx) const {
		return lx.nmon < rx.nmon;
	}
};

struct SortYear {
	bool operator()(const my_event& lx, const my_event& rx) const {
		return lx.year < rx.year;
	}
};


int main(int argc, char *argv[]) {
	// Set the tiltle of the window
	system("title ");

	// Pdcurses initiation functions
	initscr();
	scrollok(stdscr, TRUE);
	raw();
	noecho();
	clear();
	keypad(stdscr, TRUE);

	// Get max height and width of the console. 
	getmaxyx(stdscr, max_y, max_x);

	// Color functions/declarations
	start_color();
	init_color(COLOR_RED, 1000, 500, 500);
	init_pair(1, 15, 9);
	init_pair(2, 0, 7);
	init_pair(3, 15, 0);
	init_pair(4, 15, 1);
	init_pair(5, 12, 0);
	init_pair(6, 10, 0);
	init_pair(7, 13, 0);
	init_pair(8, 11, 0);

	// Title of the app (with blue background)
	titlebar();

	attron(COLOR_PAIR(3));		// Function to enable coloured text based on color pair (init_pair) declared before.
	mvprintw(2, 1, "Welcome to rF3.Calendar!");
	mvprintw(3, 1, "------------------------");
	mvprintw(5, 1, "Please select the month / year...");
	mvprintw(6, 1, "Press key LEFT or RIGHT to change and ENTER to select.");
	mvprintw(7, 1, "To quit this program, press Q.");
	refresh();

	// Adding example events to the vector
	event_saved = true;
	data.save_name("Example Event");
	data.save_loc("Engineering Faculty");
	data.min = 20;
	data.hour = 11;
	srand(time_t(NULL));
	for (int o = 1; o < 31; o++) {
		data.day = rand() % (10 - 1 + 1) + 1;
		data.nmon = rand() % (12 - 1 + 1) + 1;
		data.year = rand() % (2014 - 1990 + 1) + 1990;
		myVector.push_back(data);
	}

	// Old code
	// Show the calendar and user must select one
	//show_calendar(true,14,10);

	set_month();
	menu();

	return 0;
}

// A function to create a status window at the "create new event" page
void statuswin() {
	wstatus = newwin(3, 78, 36, 1);
	box(wstatus, 0, 0);
	wrefresh(wstatus);
}

// A simpified version of function to update text in status window
void update_status(char *text, int color = 6) {
	werase(wstatus);
	wattron(wstatus, COLOR_PAIR(color));
	mvwprintw(wstatus, 0, 0, text);
	wrefresh(wstatus);
}

// A function to create the "create new event" page.
// This function is the most crucial part in the program which is handling user inputs/data
void create_new_event(bool edit, int z_index ) {
	// This will enable the input to be shown to the user when type.
	resize_term(40, 80);
	echo();

	// Variables
	char rawday, rawname, rawloc, rawhour, rawminute;	// Raw variables used for pdcurses function
	int confirmation;
	bool eventloop = false;									// Indicator for the loop
	int n_day, n_hour, n_minute;							// Day variable which is to be selected by the user

	// Loop until get a data to be saved in data object
	while (eventloop == false) {
		// Empty the console, add titlebar and redraw all.
		erase();
		titlebar();
		refresh();
		raw();

		attron(COLOR_PAIR(3));
		if (edit == true) {
			mvprintw(2, 1, "Edit Event");
			mvprintw(3, 1, "----------");
		}
		else {
			mvprintw(2, 1, "Create New Event");
			mvprintw(3, 1, "----------------");
		}

		mvprintw(20, 1, "Date\t\t:");
		mvprintw(22, 1, "Time\t\t:");
		mvprintw(24, 1, "Event name\t:");
		mvprintw(26, 1, "Location\t:");
		//mvprintw(28, 1, "Description\t:");
		
		// Disabled due to BUG002
		// A small window for description to fit in the text in nice allignment.
		/*WINDOW *wdesc;
		wdesc = newwin(3, 60, 28, 18);
		wattron(wdesc, COLOR_PAIR(3));*/

		// Show calendar, without any inputs
		mvprintw(4, 36, "Calendar");
		show_calendar(false, 14, 6);

		// Show status window (used to deliver message to the user about error, etc.)
		statuswin();

		// Update text in status window
		update_status("Please select the day for the event...\n\nTo go back, press ESC and ENTER",6);
		//refresh();

		// Do this loop until the program obtained a valid day

		do {
			mvhline(20, 18, ' ', 3);											// Clear a small portion of the console
			mvprintw(20, 21, "%s %i", mon_name(selected_mon), selected_year);	// Print the month and year
			mvgetnstr(20, 18, &rawday, 2);										// User input of the day
			if (rawday == 27) menu();												// If user pressed esc and enter, the program will go back to main menu
			n_day = atoi(&rawday);
			//n_day = day;														// A C-function to convert char * to int
			if (n_day == 0 || n_day > max_day_in_month) { update_status("Invalid day. Please enter a valid DAY in the selected month.\n\nTo go back, press ESC and ENTER.",5); }			// Inform user if the day not valid
			refresh();															// Redraw
		} while (n_day == 0 || n_day > max_day_in_month);						// Check if the day valid or not

		// Got a nice and valid n_day and print again to the screen.
		mvhline(20, 18, ' ', 60);												// Refresh the line
		mvprintw(20, 18, "%i %s %i", n_day, mon_name(selected_mon), selected_year);

		// Update to the current status.
		update_status("Please enter the hour in 24 hours...\nIf you enter some string, the result will be 0.", 6);

		// A loop that handle the time input. Runs loop until it gets the valid time.
		do {

			mvhline(22, 18, ' ', 6);											// Clear the line
			mvgetnstr(22, 18, &rawhour, 2);										// Get hour from user
			if (rawhour == 27) menu();											// If user input is ESC then go back to menu
			n_hour = atoi(&rawhour);											// Convert from char to int
			if ( !(n_hour >= 0 && n_hour < 24) ) {								// Check for valid hour
				update_status("Invalid hour. Enter a number in between 0-23. Please try again...", 5);
				continue;
			}
			else {
				mvprintw(22, 20, ":");
				update_status("Please enter the minute...\nIf you enter some string, the result will be 0.", 6);
				do {
					mvhline(22, 21, ' ', 3);
					mvgetnstr(22, 21, &rawminute, 2);
					if (rawminute == 27) menu();
					n_minute = atoi(&rawminute);								// Convert from char to int
					if (!(n_minute >= 0 && n_minute < 60)) {					// Check for valid minutes
						update_status("Invalid minute. Type a number in between 0-59. Please try again...", 5);
						continue;
					}
					else break;
				} while (1);
				break;
			}
		} while (1);

		// Print the time nicely.
		mvhline(22, 18, ' ', 6);
		formatted_time(22, 18, n_hour, n_minute);		// Print time in formatted form xx:yy

		// Update the status
		update_status("Now fill the name, location and description of the event.");

		// Get event name, location and description		// BUG001
		mvgetnstr(24, 18, &rawname, 28);				// First program bug detected here (Read below for more)
		data.save_name(&rawname);						// This function is used to copy rawname char * to another variable in normal char type. It copy the data to a more reliable variable.
														// This is because to prevent memory leak or maybe something else that causes the data in other the pointer (this is bad due to it affect other variable as well) overwritten by the typed data. Approximate more than 11 characters will cause the memory leak -- TESTED)
		mvgetnstr(26, 18, &rawloc, 28);					// Also, to prevent this bug, this function data.save_*** is called right after the user input the data.
		data.save_loc(&rawloc);							// This problem maybe related to the pdcurses library bug that cause the memory leak which affect other variables too.

		//mvwgetnstr(wdesc, 0, 0, &rawdesc, 28);		// BUG002
		//data.save_desc(&rawdesc);						// The program will crash when number of characters more than 28 chars. The cause still unknown. To fix whether to create another custom function to handle input (which is long) or decrease the numbers of characters (which is the quickest way to solve).

		refresh();

		// Clear status
		werase(wstatus);
		wrefresh(wstatus);

		// User confirmation
		mvprintw(32, 1, "Are you sure? (Y/N): ");
		refresh();

		// A loop to check only y/Y/n/N are allowed as a user input
		while (1) {
			mvhline(32, 22, ' ', 1);
			confirmation = getch();
			if (confirmation == 780 || confirmation == 110 || confirmation == 121 || confirmation == 891) { break; }
		}

		// If user type y/Y, the the loop ends
		if (confirmation == 121 || confirmation == 891){
			eventloop = true;								// To ensure that this will break the loop and will not continue anymore
			data.day = n_day;								// Save user inputs
			data.year = selected_year;
			data.nmon = selected_mon;

			// LAB-2
			data.hour = n_hour;
			data.min = n_minute;

			if (edit == true) {
				myVector.erase(myVector.begin() + z_index);
				myVector.insert(myVector.begin() + z_index, data);
				list_events();
			}
			else {
				myVector.push_back(data);
				event_saved = true;								// Tells the program about the saved data
				break;
			}
		}
	}

	if (edit == false) menu();
}

// A function that handles the changes of month and year that suit the page
void change_mon() {
	// Routine to clear and add draw the titlebar
	erase();
	titlebar();
	refresh();

	attron(COLOR_PAIR(3));
	mvprintw(2, 1, "Change month / year");
	mvprintw(3, 1, "-------------------");

	mvprintw(5, 1, "Press key LEFT or RIGHT to change and ENTER to select.");
	mvprintw(6, 1, "To quit this program, press Q.");

	show_calendar(true, 14, 10);
}

// A function to convert month number to its name
char * mon_name(int month) {
	char *month_name[13] = { "", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	return month_name[month];
}

// Show the data saved at the "create new event" page
// It will only show the data saved in the data class of myevent
void view_event() {
	erase();
	titlebar();
	refresh();

	attron(COLOR_PAIR(3));
	mvprintw(2, 1, "View saved event");
	mvprintw(3, 1, "----------------");
	mvprintw(5, 1, "Date\t\t: %i %s %i", myVector[0].day, mon_name(myVector[0].nmon), myVector[0].year);
	mvprintw(6, 1, "Event name\t: %s", myVector[0].name);
	mvprintw(7, 1, "Location\t: %s", myVector[0].loc);
	mvprintw(8, 1, "Description\t:");

	mvprintw(12, 1, "Press ESC to go back.");
	refresh();

	// Create a small window for placing description
	/*WINDOW *wdesc_2;
	wdesc_2 = newwin(3, 60, 8, 18);
	wattron(wdesc_2, COLOR_PAIR(3));
	mvwprintw(wdesc_2, 0, 0, "%s", myVector[0].desc);
	wrefresh(wdesc_2);*/
	
	int key;
	while (1) {
		key = getch();
		if (key == 27) menu();
	}
}

void wformatted_time(WINDOW *win, int y, int x, int hour, int minute) {
	bool lesshour = false, lessminute = false;
	if (hour < 10) lesshour = true;
	if (minute < 10) lessminute = true;

	if (lesshour == true && lessminute == true) mvwprintw(win, y, x, "0%i:0%i", hour, minute);
	else if (lesshour == true) mvwprintw(win, y, x, "0%i:%i", hour, minute);
	else if (lessminute == true) mvwprintw(win, y, x, "%i:0%i", hour, minute);
	else mvwprintw(win, y, x, "%i:%i", hour, minute);
}

void formatted_time(int y, int x, int hour, int minute) {
	bool lesshour = false, lessminute = false;
	if (hour < 10) lesshour = true;
	if (minute < 10) lessminute = true;

	if (lesshour == true && lessminute == true) mvprintw(y, x, "0%i:0%i", hour, minute);
	else if (lesshour == true) mvprintw(y, x, "0%i:%i", hour, minute);
	else if (lessminute == true) mvprintw(y, x, "%i:0%i", hour, minute);
	else mvprintw(y, x, "%i:%i", hour, minute);
}

// Menu function
void menu() {
	int key;

	resize_term(25, 80);
	erase();
	titlebar();
	noecho();

	attron(COLOR_PAIR(3));
	mvprintw(2, 1, "Menu");
	mvprintw(3, 1, "----");
	mvprintw(5, 1, "1) Create a new event");
	mvprintw(6, 1, "2) View or edit event");
	mvprintw(7, 1, "3) Change month / year (Selected: %s %i)", mon_name(selected_mon), selected_year);
	mvprintw(8, 1, "4) Save all events to file");
	mvprintw(9, 1, "Q) Quit program");
	//mvprintw(23, 1, "Created by Ijat (172205)");
	attron(COLOR_PAIR(8));
	mvprintw(23, 1, "(c) 2014 Ijat.my");
	attron(COLOR_PAIR(3));
	mvprintw(11, 1, "Your selection: ");
	refresh();

	// Check user input
	while (1) {
		key = getch();
		//mvprintw(23, 1, "%i", key);

		// Pressed 1
		if (key == 49) { create_new_event(false); }

		// Pressed 2
		if (key == 50) {
			if (event_saved == false) {
				attron(COLOR_PAIR(5));
				mvhline(23, 1, ' ', 78);
				mvprintw(23, 1, "No event(s) saved. Nothing to view. Please create a new event first!");
				refresh();
				attroff(COLOR_PAIR(5));
			}
			else if (event_saved == true) { //view_event(); 
				list_events();
			}
		}

		// Pressed 3
		if (key == 51) change_mon(); 

		if (key == 52) {
			if (event_saved == true) {
				attron(COLOR_PAIR(6));
				mvprintw(23, 1, "Saved to \"My Events.txt\"");
				data.print_details();
				attroff(COLOR_PAIR(6));
			}
			else {
				attron(COLOR_PAIR(5));
				mvhline(23, 1, ' ', 78);
				mvprintw(23, 1, "No event(s) to save. Please create a event first!");
				attroff(COLOR_PAIR(5));
			}
		}

		// Pressed Q
		if (key == 113 || key == 81) exit(0);
	}
}

void set_month() {
	time_t rawtime = time(NULL);
	tm now;
	localtime_s(&now, &rawtime);
	selected_mon = now.tm_mon + 1;
	selected_year = now.tm_year + 1900;
}

void show_calendar(bool input, int x_axis = 14, int y_axis = 10) {
	// Calendar
	wcal = newwin(12, 52, y_axis, x_axis);
	wbkgd(wcal, COLOR_PAIR(4));

	// Get current date and print the calendar
	time_t rawtime = time(NULL);
	tm now;
	localtime_s(&now, &rawtime);

	// Applies the changes if the user select to change the month / year
	if (selected_mon != 0 && selected_year != 0) {
		now.tm_year = selected_year - 1900;
		now.tm_mon = selected_mon - 1;
	}

	// Variables
	int nmon = 1, nyear = 1900;

	// Print out the calendar
	printCar(nyear + now.tm_year, now.tm_mon + nmon);

	// Window border
	box(wcal, 0, 0);

	// Draw the window and its contents
	wrefresh(wcal);

	// Loop to manage user inputs
	if (input == true) {
		int key;
		while (1) {
			key = getch();

			// Pressed enter
			if (key == 13) {
				selected_mon = now.tm_mon + nmon;
				selected_year = nyear + now.tm_year;
				erase();
				titlebar();
				refresh();
				menu();
			}

			// Pressed q to exit
			if (key == 113 || key == 81) {
				exit(0);
			}

			// Pressed left key and change to a month before current month
			if (key == 260) {
				nmon--;
				if (now.tm_mon + nmon == 0) {
					nmon = 12 - now.tm_mon;
					nyear--;
				}
				werase(wcal);
				printCar(nyear + now.tm_year, now.tm_mon + nmon);
				box(wcal, 0, 0);
				wrefresh(wcal);
			}

			// Pressed right key and change the month next after current month
			if (key == 261) {
				nmon++;
				if (now.tm_mon + nmon >= 13) {
					nmon = 1 - now.tm_mon;
					nyear++;
				}
				werase(wcal);
				printCar(nyear + now.tm_year, now.tm_mon + nmon);
				box(wcal, 0, 0);
				wrefresh(wcal);
			}
		}
	}
}

// Show the titlebar with the blue background
void titlebar() {
	attron(COLOR_PAIR(1));
	mvhline(0, 0, ' ', max_x);
	mvprintw(0, (max_x - 18) / 2, ".: rF3.Calendar :.");
	attroff(COLOR_PAIR(1));
}

// Function to save all events to file. Without filter day / month / year. Filtering isn't implemented yet.
void save_to_file() {
	std::ofstream the_file("My Events.txt", std::ios::app);
	for (unsigned int o = 0; o < myVector.size(); o++) the_file << "Event #" << o + 1 << "\nTitle\t\t: " << myVector[o].name << "\nDate\t\t: " << std::setw(2) << std::setfill('0') << myVector[o].day << ' ' << mon_name(myVector[o].nmon) << ' ' << myVector[o].year << "\nTime\t\t: " << std::setw(2) << std::setfill('0') << myVector[o].hour << ":" << std::setw(2) << std::setfill('0') << myVector[o].min << "\nLocation\t: " << myVector[o].loc << "\n\n";
	the_file.close();
}

// Erase events at the desired location
void vector_erase(int z_index) {
	myVector.erase(myVector.begin() + z_index);
	if (myVector.size() == 0) {
		event_saved = false;
		menu();
	}
}

// Show the events details
void event_details(int z_index) {
	erase();
	titlebar();
	refresh();
	resize_term(25, 80);

	attron(COLOR_PAIR(3));
	mvprintw(2, 1, "View Details");
	mvprintw(3, 1, "------------");

	mvprintw(5, 1, "Event Name\t: %s", myVector[z_index].name);
	mvprintw(7, 1, "Date\t\t: %i %s %i", myVector[z_index].day, mon_name(myVector[z_index].nmon), myVector[z_index].year);
	mvprintw(9, 1, "Time\t\t: ");
	formatted_time(9, 18, myVector[z_index].hour, myVector[z_index].min);
	mvprintw(11, 1, "Location\t: %s", myVector[z_index].loc);
	//mvprintw(13, 1, "Description\t: ");
	attron(COLOR_PAIR(6));
	mvprintw(17, 1, "Press ESC to go back...");
	refresh();

	/*WINDOW *wdesc;
	wdesc = newwin(3, 60, 13, 18);
	wattron(wdesc, COLOR_PAIR(3));
	mvwprintw(wdesc, 0, 0, "%s", myVector[z_index].desc);
	wrefresh(wdesc);*/

	int key;
	do {
		key = getch();
	} while (key != 27);
	list_events();
}

void list_events(int day_filter, int mon_filter, int year_filter) {
	unsigned int current_page = 1, max_page, index_page = 0;
	unsigned int mod = myVector.size() % 12;
	int key, n_keys = 0, selected_event_index;
	char type_keys[66];

	erase();
	titlebar();
	refresh();
	resize_term(40, 80);

	attron(COLOR_PAIR(3));
	mvprintw(2, 1, "List Event(s)");
	mvprintw(3, 1, "-------------");

	// Draw table
	mvvline(5, 1, 0, 29);
	mvvline(5, 6, 0, 29);
	mvvline(5, 51, 0, 29);
	mvvline(5, 70, 0, 29);
	mvvline(5, 78, 0, 29);
	mvhline(5, 1, 0, 78);
	mvhline(7, 1, 0, 78);
	mvhline(33, 1, 0, 78);

	// Print out some text
	mvprintw(6, 3, "No");
	mvprintw(6, 8, "Event Name");
	mvprintw(6, 53, "Date");
	mvprintw(6, 72, "Time");

	refresh();

	WINDOW *wID, *wNAME, *wDATE, *wTIME, *wDET, *wSTAT;

	wID = newwin(25, 4, 8, 2);
	wrefresh(wID);

	wNAME = newwin(25, 42, 8, 8);
	wrefresh(wNAME);

	wDATE = newwin(25, 18, 8, 52);
	wrefresh(wDATE);

	wTIME = newwin(25, 7, 8, 71);
	wrefresh(wTIME);

	wDET = newwin(4, 78, 36, 1);
	wattron(wDET, COLOR_PAIR(6));
	mvwprintw(wDET, 0, 0, "Press LEFT or RIGHT to change the page. Type any event number and press E to \nedit, D to delete, R to swap two events or ENTER for more details.\nOr you can press S to sort all the events. To go back, press ESC.");
	wrefresh(wDET);

	wSTAT = newwin(1, 78, 39, 1);
	wrefresh(wSTAT);

	// Declare variable for filter purpose
	std::vector<my_event> filter;
	std::vector<my_event> *list;
	my_event temp;

	// Apply filter for saved events. Copy data to the temporary local vector and print to the list.
	// Optional function. Maybe not implemented for now.
	// Since the code is identical, only the logical expression different. So it can be simpified but not for now.
	if (day_filter != 0 || mon_filter != 0 || year_filter != 0) {
		for (unsigned int z = 0; z < myVector.size(); z++) {
			if (year_filter != 0) {
				if (myVector[z].year == year_filter) {
					temp.hour = myVector[z].hour;
					temp.min = myVector[z].min;
					temp.day = myVector[z].day;
					temp.nmon = myVector[z].nmon;
					temp.year = myVector[z].year;
					temp.save_name(myVector[z].name);
					temp.save_loc(myVector[z].loc);
					//temp.save_desc(myVector[z].desc);
					filter.push_back(temp);
				}
			}
			if (mon_filter != 0) {
				if (myVector[z].nmon == mon_filter) {
					temp.hour = myVector[z].hour;
					temp.min = myVector[z].min;
					temp.day = myVector[z].day;
					temp.nmon = myVector[z].nmon;
					temp.year = myVector[z].year;
					temp.save_name(myVector[z].name);
					temp.save_loc(myVector[z].loc);
					//temp.save_desc(myVector[z].desc);
					filter.push_back(temp);
				}
			}
			if (day_filter != 0) {
				if (myVector[z].day == day_filter) {
					temp.hour = myVector[z].hour;
					temp.min = myVector[z].min;
					temp.day = myVector[z].day;
					temp.nmon = myVector[z].nmon;
					temp.year = myVector[z].year;
					temp.save_name(myVector[z].name);
					temp.save_loc(myVector[z].loc);
					//temp.save_desc(myVector[z].desc);
					filter.push_back(temp);
				}
			}
			if (day_filter != 0 && mon_filter != 0) {
				if (myVector[z].day == day_filter && myVector[z].nmon == mon_filter) {
					temp.hour = myVector[z].hour;
					temp.min = myVector[z].min;
					temp.day = myVector[z].day;
					temp.nmon = myVector[z].nmon;
					temp.year = myVector[z].year;
					temp.save_name(myVector[z].name);
					temp.save_loc(myVector[z].loc);
					//temp.save_desc(myVector[z].desc);
					filter.push_back(temp);
				}
			}
			if (year_filter != 0 && mon_filter != 0) {
				if (myVector[z].year == year_filter && myVector[z].nmon == mon_filter) {
					temp.hour = myVector[z].hour;
					temp.min = myVector[z].min;
					temp.day = myVector[z].day;
					temp.nmon = myVector[z].nmon;
					temp.year = myVector[z].year;
					temp.save_name(myVector[z].name);
					temp.save_loc(myVector[z].loc);
					//temp.save_desc(myVector[z].desc);
					filter.push_back(temp);
				}
			}
			if (day_filter != 0 && mon_filter != 0 && year_filter != 0) {
				if (myVector[z].day == day_filter && myVector[z].nmon == mon_filter && myVector[z].year == year_filter) {
					temp.hour = myVector[z].hour;
					temp.min = myVector[z].min;
					temp.day = myVector[z].day;
					temp.nmon = myVector[z].nmon;
					temp.year = myVector[z].year;
					temp.save_name(myVector[z].name);
					temp.save_loc(myVector[z].loc);
					//temp.save_desc(myVector[z].desc);
					filter.push_back(temp);
				}
			}
		}
		list = &filter;
	}
	else list = &myVector;

	if (mod != 0) max_page = (int)(ceil((*list).size() / 12)) + 1;
	else max_page = (*list).size() / 12;

	while (1) {

		werase(wID);
		werase(wNAME);
		werase(wDATE);
		werase(wTIME);
		mvprintw(34, 1, "Page: %i/%i", current_page, max_page);

		int n_line = 1;
		for (unsigned int i = index_page; i < index_page + 12; i++) {
			if (i < (*list).size()) {
				mvwprintw(wID, n_line, 1, "%i", i + 1);
				mvwprintw(wNAME, n_line, 0, "%s", (*list)[i].name);
				mvwprintw(wDATE, n_line, 1, "%i %s %i", (*list)[i].day, mon_name((*list)[i].nmon), (*list)[i].year);
				wformatted_time(wTIME, n_line, 1, (*list)[i].hour, (*list)[i].min);
				wrefresh(wID);
				wrefresh(wNAME);
				wrefresh(wDATE);
				wrefresh(wTIME);
				n_line += 2;
			}
		}

		key = getch();
		//mvprintw(39, 1, "%i", key);

		if (key == 115 || key == 83) {
			werase(wDET);
			wattron(wDET, COLOR_PAIR(6));
			mvwprintw(wDET, 0, 0, "Press 1 to sort by Day, 2 to sort by Month or 3 to sort by Year.\nPress ESC to go back.");
			wattroff(wDET, COLOR_PAIR(6));
			wrefresh(wDET);
			werase(wSTAT);
			while (1) {
				key = getch();
				if (!(key >= 49 && key <= 51) || key != 27) {
					werase(wDET);
					wattron(wDET, COLOR_PAIR(5));
					mvwprintw(wDET, 0, 0, "Invalid selection...");
					wattroff(wDET, COLOR_PAIR(5));
					wrefresh(wDET);
				}
				if (key == 49) {
					std::sort(myVector.begin(), myVector.end(), SortDay());
					break;
				}
				if (key == 50) {
					std::sort(myVector.begin(), myVector.end(), SortMonth());
					break;
				}
				if (key == 51) {
					std::sort(myVector.begin(), myVector.end(), SortYear());
					break;
				}
				if (key == 27) break;
			}
			break;
		}

		if ((key >= 48 && key <= 57) || key == 8) {
			if (key >= 48 && key <= 57) {
				if (n_keys <= 66) {
					werase(wSTAT);
					type_keys[n_keys] = char(key);
					n_keys++;

					wattron(wSTAT, COLOR_PAIR(3));
					mvwprintw(wSTAT, 0, 0, "Event No: ");
					wattroff(wSTAT, COLOR_PAIR(3));

					wattron(wSTAT, COLOR_PAIR(7));
					for (int o = 0; o < n_keys; o++) wprintw(wSTAT, "%c", type_keys[o]);		// Show the user for any typed numbers
					wrefresh(wSTAT);
					wattroff(wSTAT, COLOR_PAIR(7));
				}
			}
			if (key == 8) {
				if (n_keys > 1) {
					werase(wSTAT);
					type_keys[n_keys] = ' ';
					type_keys[n_keys - 1] = ' ';
					n_keys--;

					wattron(wSTAT, COLOR_PAIR(3));
					mvwprintw(wSTAT, 0, 0, "Event No: ");
					wattroff(wSTAT, COLOR_PAIR(3));

					wattron(wSTAT, COLOR_PAIR(7));
					for (int o = 0; o < n_keys; o++) wprintw(wSTAT, "%c", type_keys[o]);
					wrefresh(wSTAT);
					wattroff(wSTAT, COLOR_PAIR(7));
				}
				else {
					werase(wSTAT);
					wrefresh(wSTAT);
					n_keys = 0;
				}
			}
		}

		// User pressed enter/E/D (all caps)
		if (key == 13 || key == 100 || key == 68 || key == 69 || key == 101 || key == 82 || key == 114) {
			if (n_keys > 0) {
				selected_event_index = atol(&type_keys[0]);
				if (selected_event_index >= 1 && selected_event_index <= int((*list).size())) break;
				else {
					werase(wSTAT);
					wrefresh(wSTAT);
					werase(wDET);
					wattron(wDET, COLOR_PAIR(5));
					mvwprintw(wDET, 0, 0, "Invalid number or out of range.\nPlease try again...");
					wattroff(wDET, COLOR_PAIR(5));
					wrefresh(wDET);
					for (int o = 0; o <= 66; o++) type_keys[o] = ' ';						// Clear the array
					n_keys = 0;																// Reset the number of key typed
				}
			}
		}

		if (key == 27) menu();

		if (key == KEY_RIGHT) {
			if (current_page < max_page) {
				index_page += 12;
				current_page++;
				continue;
			}
		}

		if (key == KEY_LEFT) {
			if (current_page > 1) {
				index_page -= 12;
				current_page--;
				continue;
			}
		}

		if (key == KEY_F(2)) {
			mvprintw(5, 1, "%i", myVector.size());
			//refresh();
		}

		if (key == KEY_F(1)) exit(0);
	}

	if (key == 13) data.show_details(selected_event_index - 1);
	if (key == 100 || key == 68) data.delete_event(selected_event_index - 1);
	if (key == 101 || key == 69) data.edit_event(selected_event_index - 1);

	// Sorting Lab 4
//	if (key == 82 || key == 114) {
		
	//}

	// Swapping Lab 3
	if (key == 82 || key == 114) {
		werase(wDET);
		wattron(wDET, COLOR_PAIR(6));
		mvwprintw(wDET, 0, 0, "Now enter the second event number to replace.\nPress ESC to cancel.");
		wattroff(wDET, COLOR_PAIR(6));
		wrefresh(wDET);
		werase(wSTAT);
		mvwprintw(wSTAT, 0, 0, "Event No To Replace: ");
		wrefresh(wSTAT);
		for (int o = 0; o <= 66; o++) type_keys[o] = ' ';						// Clear the array
		n_keys = 0;
		while (1) {
			key = getch();
			if (key == 27) break;
			if (key >= 48 && key <= 57) {
				if (n_keys <= 66) {
					werase(wSTAT);
					type_keys[n_keys] = char(key);
					n_keys++;

					wattron(wSTAT, COLOR_PAIR(3));
					mvwprintw(wSTAT, 0, 0, "Event No To Replace: ");
					wattroff(wSTAT, COLOR_PAIR(3));

					wattron(wSTAT, COLOR_PAIR(7));
					for (int o = 0; o < n_keys; o++) wprintw(wSTAT, "%c", type_keys[o]);		// Show the user for any typed numbers
					wrefresh(wSTAT);
					wattroff(wSTAT, COLOR_PAIR(7));
				}
			}
			if (key == 8) {
				if (n_keys > 1) {
					werase(wSTAT);
					type_keys[n_keys] = ' ';
					type_keys[n_keys - 1] = ' ';
					n_keys--;

					wattron(wSTAT, COLOR_PAIR(3));
					mvwprintw(wSTAT, 0, 0, "Event No To Replace: ");
					wattroff(wSTAT, COLOR_PAIR(3));

					wattron(wSTAT, COLOR_PAIR(7));
					for (int o = 0; o < n_keys; o++) wprintw(wSTAT, "%c", type_keys[o]);
					wrefresh(wSTAT);
					wattroff(wSTAT, COLOR_PAIR(7));
				}
				else {
					werase(wSTAT);
					wrefresh(wSTAT);
					n_keys = 0;
				}
			}

			if (key == 13 || key == 100) {
				if (n_keys > 0) {
					int sec_selected_event_index;
					sec_selected_event_index = atol(&type_keys[0]);
					if (sec_selected_event_index >= 1 && sec_selected_event_index <= int((*list).size()) && selected_event_index != sec_selected_event_index) {
						/*mvwprintw(wSTAT, 0, 0, "selected: %i, second: %i",selected_event_index,sec_selected_event_index);
						wrefresh(wSTAT);
						getch();*/
						swap(selected_event_index - 1, sec_selected_event_index - 1);
						break;
					}
					else {
						werase(wSTAT);
						wrefresh(wSTAT);
						werase(wDET);
						wattron(wDET, COLOR_PAIR(5));
						mvwprintw(wDET, 0, 0, "Invalid number or out of range.\nPlease try again...");
						wattroff(wDET, COLOR_PAIR(5));
						wrefresh(wDET);
						for (int o = 0; o <= 66; o++) type_keys[o] = ' ';						// Clear the array
						n_keys = 0;																// Reset the number of key typed
						werase(wSTAT);
						mvwprintw(wSTAT, 0, 0, "Event No To Replace: ");
						wrefresh(wSTAT);
					}
				}
			}
		}

	}

	list_events();
}

// Swap position between two vector data
// z_index and x_index is just a variable name for vector[index].
void swap(int z_index, int x_index) {
	std::vector<my_event> vector_swap;
	vector_swap = myVector;
	my_event temp;

	int index;
	for (int i = 0; i < 2; i++) {
		if (i == 0) index = x_index;
		else index = z_index;

		temp.hour = vector_swap[index].hour;
		temp.min = vector_swap[index].min;
		temp.day = vector_swap[index].day;
		temp.nmon = vector_swap[index].nmon;
		temp.year = vector_swap[index].year;
		temp.save_name(vector_swap[index].name);
		temp.save_loc(vector_swap[index].loc);

		if (i == 0) index = z_index;
		else index = x_index;
		myVector.erase(myVector.begin() + index);
		myVector.insert(myVector.begin() + index, temp);
	}
}