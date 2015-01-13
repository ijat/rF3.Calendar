// A calendar function by http://www.worldbestlearningcenter.com/index_files/cpp-example-printing-calendar.htm
// With a little modification to make it more suitable in this program
int max_day_in_month;

int getDayCode(int year){
	int fday;
	fday = (((year - 1) * 365 + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400) + 1) % 7;
	return fday;
}

int getFirstDay(int year, int month){
	int fday = getDayCode(2014);
	int num_days = 0; //number of days of the year before the current month
	int dCode = getDayCode(year);
	switch (month){
	case 12: num_days += 30;
	case 11: num_days += 31;
	case 10: num_days += 30;
	case 9: num_days += 31;
	case 8: num_days += 31;
	case 7: num_days += 30;
	case 6: num_days += 31;
	case 5: num_days += 30;
	case 4: num_days += 31;
	case 3: num_days += 28;
	case 2: num_days += 31;
	}
	if (month == 1) return(fday);
	else if ((year % 4 == 0) || (year % 400 == 0) && month>2) return (num_days + 1 + dCode) % 7;  //leap year
	else return (num_days + dCode) % 7;
}

void printCar(int year, int month){
	int fday_month = getFirstDay(year, month);
	int code[7] = { 0, 1, 2, 3, 4, 5, 6 };
	char *Day[7] = { "Sun", "Mon", "Tues", "Wed", "Thu", "Fri", "Sat" };
	char *Month[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
	int num_days, i;
	if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		num_days = 31;
	else if (month == 2)
	{
		if ((year % 4 == 0) || (year % 400 == 0)) num_days = 29;
		else num_days = 28;
	}
	else num_days = 30;
	max_day_in_month = num_days;

	mvwprintw(wcal, 1, 2, "%s", Month[month - 1]);
	mvwprintw(wcal, 1, 46, "%i", year);
	mvwprintw(wcal, 3, 2, "Su\tMo\tTu\tWe\tTh\tFr\tSa");
	mvwhline(wcal, 4, 2, '-', 48);

	int d = 1, con = 0, n = 5;

	wmove(wcal, n, 2);
	for (i = 1; i <= fday_month; i++) wprintw(wcal, "\t");

	while (d <= num_days)
	{
		wprintw(wcal, "%i\t", d);

		if (fday_month>0) {
			if (d == 7 - fday_month) {
				n++;
				wmove(wcal, n, 2);
				con = d;
			}
			else if (d % (con + 7) == 0) {
				n++;
				wmove(wcal, n, 2);
				con = d;
			}
		}
		else if (d % 7 == 0) {
			n++;
			wmove(wcal, n, 2);
		}
		d++;
	}
	n++;
	wmove(wcal, n, 2);
}