
#include "PrayTimes.h"

/* ------------------------ Variables ------------------------ */

static const char* TimeName[] =
{
	"Fajr",
	"Sunrise",
	"Dhuhr",
	"Asr",
	"Sunset",
	"Maghrib",
	"Isha",
};

CalculationMethod calc_method;		// caculation method
JuristicMethod asr_juristic;			// Juristic method for Asr
AdjustingMethod adjust_high_lats;	// adjusting method for higher latitudes
MethodConfig method_params[CalculationMethodsCount];

double dhuhr_minutes;		// minutes after mid-day for Dhuhr

double latitude;
double longitude;
double timezone;
double julian_date;

Azan_Time_t praytimes[TimesCount];

/* --------------------- Technical Settings -------------------- */

	static const int NUM_ITERATIONS = 1;		// number of iterations needed to compute times

// ---------------
/* -------------------- Interface Functions -------------------- */
void init_prayer_times(void)
{
	calc_method = UoT;
	asr_juristic = Shafii;
	adjust_high_lats = MidNight;
	dhuhr_minutes = 0;
	
	method_params[Jafari].fajr_angle  = 16.0;
	method_params[Jafari].isha_is_minutes = false;
	method_params[Jafari].isha_value = 4.0;
	method_params[Jafari].maghrib_is_minutes = false;
	method_params[Jafari].maghrib_value = 14.0;
	
	method_params[Karachi].fajr_angle  = 18.0;
	method_params[Karachi].isha_is_minutes = true;
	method_params[Karachi].isha_value = 0.0;
	method_params[Karachi].maghrib_is_minutes = false;
	method_params[Karachi].maghrib_value = 18.0;
		
	method_params[ISNA].fajr_angle  = 15.0;
	method_params[ISNA].isha_is_minutes = true;
	method_params[ISNA].isha_value = 0.0;
	method_params[ISNA].maghrib_is_minutes = false;
	method_params[ISNA].maghrib_value = 15.0;
	
	method_params[MWL].fajr_angle  = 18.0;
	method_params[MWL].isha_is_minutes = true;
	method_params[MWL].isha_value = 0.0;
	method_params[MWL].maghrib_is_minutes = false;
	method_params[MWL].maghrib_value = 17.0;
	
	method_params[Makkah].fajr_angle  = 18.5;
	method_params[Makkah].isha_is_minutes = true;
	method_params[Makkah].isha_value = 0.0;
	method_params[Makkah].maghrib_is_minutes = true;
	method_params[Makkah].maghrib_value = 90.0;
	
	method_params[Egypt].fajr_angle  = 19.5;
	method_params[Egypt].isha_is_minutes = true;
	method_params[Egypt].isha_value = 0.0;
	method_params[Egypt].maghrib_is_minutes = false;
	method_params[Egypt].maghrib_value = 17.5;
	
	method_params[Custom].fajr_angle  = 18.0;
	method_params[Custom].isha_is_minutes = true;
	method_params[Custom].isha_value = 0.0;
	method_params[Custom].maghrib_is_minutes = false;
	method_params[Custom].maghrib_value = 17.0;
	
	method_params[UoT].fajr_angle  = 17.7;
	method_params[UoT].isha_is_minutes = true;
	method_params[UoT].isha_value = 0.0;
	method_params[UoT].maghrib_is_minutes = false;
	method_params[UoT].maghrib_value = 14.0;
}

/* return prayer times for a given date */
void get_prayer_times(int year, int month, int day, double _latitude, double _longitude, double _timezone, double times[])
{
	latitude = _latitude;
	longitude = _longitude;
	timezone = _timezone;
	julian_date = get_julian_date(year, month, day) - longitude / (double) (15 * 24);
	compute_day_times(times);
}

/* return prayer times for a given date */
//void get_prayer_times(time_t date, double latitude, double longitude, double timezone, double times[])
//{
//	struct tm* t = localtime(&date);
//	get_prayer_times_ymd(1900 + t->tm_year, t->tm_mon + 1, t->tm_mday, latitude, longitude, timezone, times);
//}

/* set the calculation method  */
void set_calc_method(CalculationMethod method_id)
{
	calc_method = method_id;
}

/* set the juristic method for Asr */
void set_asr_method(JuristicMethod method_id)
{
	asr_juristic = method_id;
}

/* set adjusting method for higher latitudes */
void set_high_lats_adjust_method(AdjustingMethod method_id)
{
	adjust_high_lats = method_id;
}

/* set the angle for calculating Fajr */
void set_fajr_angle(double angle)
{
	method_params[Custom].fajr_angle = angle;
	calc_method = Custom;
}

/* set the angle for calculating Maghrib */
void set_maghrib_angle(double angle)
{
	method_params[Custom].maghrib_is_minutes = false;
	method_params[Custom].maghrib_value = angle;
	calc_method = Custom;
}

/* set the angle for calculating Isha */
void set_isha_angle(double angle)
{
	method_params[Custom].isha_is_minutes = false;
	method_params[Custom].isha_value = angle;
	calc_method = Custom;
}

/* set the minutes after mid-day for calculating Dhuhr */
void set_dhuhr_minutes(double minutes)
{
	dhuhr_minutes = minutes;
}

/* set the minutes after Sunset for calculating Maghrib */
void set_maghrib_minutes(double minutes)
{
	method_params[Custom].maghrib_is_minutes = true;
	method_params[Custom].maghrib_value = minutes;
	calc_method = Custom;
}

/* set the minutes after Maghrib for calculating Isha */
void set_isha_minutes(double minutes)
{
	method_params[Custom].isha_is_minutes = true;
	method_params[Custom].isha_value = minutes;
	calc_method = Custom;
}

///* get hours and minutes parts of a float time */
//static void get_float_time_parts(double time, int& hours, int& minutes)
//{
//	time = fix_hour(time + 0.5 / 60);		// add 0.5 minutes to round
//	hours = floor(time);
//	minutes = floor((time - hours) * 60);
//}

///* convert float hours to 24h format */
//static string float_time_to_time24(double time)
//{
//	if (isnan(time))
//		return std::string();
//	int hours, minutes;
//	get_float_time_parts(time, hours, minutes);
//	return two_digits_format(hours) + ':' + two_digits_format(minutes);
//}

///* convert float hours to 12h format */
//static string float_time_to_time12(double time, bool no_suffix = false)
//{
//	if (isnan(time))
//		return std::string();
//	int hours, minutes;
//	get_float_time_parts(time, hours, minutes);
//	const char* suffix = hours >= 12 ? " PM" : " AM";
//	hours = (hours + 12 - 1) % 12 + 1;
//	return int_to_string(hours) + ':' + two_digits_format(minutes) + (no_suffix ? "" : suffix);
//}

///* convert float hours to 12h format with no suffix */
//static string float_time_to_time12ns(double time)
//{
//	return float_time_to_time12(time, true);
//}

/* ---------------------- Time-Zone Functions ----------------------- */

///* compute local time-zone for a specific date */
//static double get_effective_timezone(time_t local_time)
//{
//	tm* tmp = localtime(&local_time);
//	tmp->tm_isdst = 0;
//	time_t local = mktime(tmp);
//	tmp = gmtime(&local_time);
//	tmp->tm_isdst = 0;
//	time_t gmt = mktime(tmp);
//	return (local - gmt) / 3600.0;
//}

///* compute local time-zone for a specific date */
//static double get_effective_timezone_ymd(int year, int month, int day)
//{
//	tm date = { 0 };
//	date.tm_year = year - 1900;
//	date.tm_mon = month - 1;
//	date.tm_mday = day;
//	date.tm_isdst = -1;		// determine it yourself from system
//	time_t local = mktime(&date);		// seconds since midnight Jan 1, 1970
//	return get_effective_timezone(local);
//}

/* ---------------------- Calculation Functions ----------------------- */

/* References: */
/* http://www.ummah.net/astronomy/saltime   */
/* http://aa.usno.navy.mil/faq/docs/SunApprox.html */

/* compute declination angle of sun and equation of time */
void sun_position(double jd, double *dd, double *eq_t)
{
	double d = jd - 2451545.0;
	double g = fix_angle(357.529 + 0.98560028 * d);
	double q = fix_angle(280.459 + 0.98564736 * d);
	double l = fix_angle(q + 1.915 * dsin(g) + 0.020 * dsin(2 * g));

	// double r = 1.00014 - 0.01671 * dcos(g) - 0.00014 * dcos(2 * g);
	double e = 23.439 - 0.00000036 * d;

	*dd = darcsin(dsin(e) * dsin(l));
	double ra = darctan2(dcos(e) * dsin(l), dcos(l)) / 15.0;
	ra = fix_hour(ra);
	*eq_t = q / 15.0 - ra;
}

/* compute equation of time */
double equation_of_time(double jd)
{
	double dd, eq_t;
	sun_position(jd, &dd, &eq_t);
	return eq_t;
}

/* compute declination angle of sun */
double sun_declination(double jd)
{
	double dd, eq_t;
	sun_position(jd, &dd, &eq_t);
	return dd;
}

/* compute mid-day (Dhuhr, Zawal) time */
double compute_mid_day(double _t)
{
	double t = equation_of_time(julian_date + _t);
	double z = fix_hour(12 - t);
	return z;
}

/* compute time for a given angle G */
double compute_time(double g, double t)
{
	double d = sun_declination(julian_date + t);
	double z = compute_mid_day(t);
	double v = 1.0 / 15.0 * darccos((-dsin(g) - dsin(d) * dsin(latitude)) / (dcos(d) * dcos(latitude)));
	return z + (g > 90.0 ? - v :  v);
}

/* compute the time of Asr */
double compute_asr(int step, double t)  // Shafii: step=1, Hanafi: step=2
{
	double d = sun_declination(julian_date + t);
	double g = -darccot(step + dtan(fabs(latitude - d)));
	return compute_time(g, t);
}

/* ---------------------- Compute Prayer Times ----------------------- */

// array parameters must be at least of size TimesCount

/* compute prayer times at given julian date */
void compute_times(double times[])
{
	day_portion(times);

	times[Fajr]    = compute_time(180.0 - method_params[calc_method].fajr_angle, times[Fajr]);
	times[Sunrise] = compute_time(180.0 - 0.833, times[Sunrise]);
	times[Dhuhr]   = compute_mid_day(times[Dhuhr]);
	times[Asr]     = compute_asr(1 + asr_juristic, times[Asr]);
	times[Sunset]  = compute_time(0.833, times[Sunset]);
	times[Maghrib] = compute_time(method_params[calc_method].maghrib_value, times[Maghrib]);
	times[Isha]    = compute_time(method_params[calc_method].isha_value, times[Isha]);
}


/* compute prayer times at given julian date */
void compute_day_times(double times[])
{
	double default_times[] = { 5, 6, 12, 13, 18, 18, 18 };		// default times
	for (int i = 0; i < TimesCount; ++i)
		times[i] = default_times[i];

	for (int i = 0; i < NUM_ITERATIONS; ++i)
		compute_times(times);

	adjust_times(times);
}


/* adjust times in a prayer time array */
void adjust_times(double times[])
{
	for (int i = 0; i < TimesCount; ++i)
		times[i] += timezone - longitude / 15.0;
	times[Dhuhr] += dhuhr_minutes / 60.0;		// Dhuhr
	if (method_params[calc_method].maghrib_is_minutes)		// Maghrib
		times[Maghrib] = times[Sunset] + method_params[calc_method].maghrib_value / 60.0;
	if (method_params[calc_method].isha_is_minutes)		// Isha
		times[Isha] = times[Maghrib] + method_params[calc_method].isha_value / 60.0;

	if (adjust_high_lats != None)
		adjust_high_lat_times(times);
}

/* adjust Fajr, Isha and Maghrib for locations in higher latitudes */
void adjust_high_lat_times(double times[])
{
	double night_time = time_diff(times[Sunset], times[Sunrise]);		// sunset to sunrise

	// Adjust Fajr
	double fajr_diff = night_portion(method_params[calc_method].fajr_angle) * night_time;
	if (isnan(times[Fajr]) || time_diff(times[Fajr], times[Sunrise]) > fajr_diff)
		times[Fajr] = times[Sunrise] - fajr_diff;

	// Adjust Isha
	double isha_angle = method_params[calc_method].isha_is_minutes ? 18.0 : method_params[calc_method].isha_value;
	double isha_diff = night_portion(isha_angle) * night_time;
	if (isnan(times[Isha]) || time_diff(times[Sunset], times[Isha]) > isha_diff)
		times[Isha] = times[Sunset] + isha_diff;

	// Adjust Maghrib
	double maghrib_angle = method_params[calc_method].maghrib_is_minutes ? 4.0 : method_params[calc_method].maghrib_value;
	double maghrib_diff = night_portion(maghrib_angle) * night_time;
	if (isnan(times[Maghrib]) || time_diff(times[Sunset], times[Maghrib]) > maghrib_diff)
		times[Maghrib] = times[Sunset] + maghrib_diff;
}


/* the night portion used for adjusting times in higher latitudes */
double night_portion(double angle)
{
	switch (adjust_high_lats)
	{
		case AngleBased:
			return angle / 60.0;
		case MidNight:
			return 1.0 / 2.0;
		case OneSeventh:
			return 1.0 / 7.0;
		default:
			// Just to return something!
			// In original library nothing was returned
			// Maybe I should throw an exception
			// It must be impossible to reach here
			return 0;
	}
}

/* convert hours to day portions  */
void day_portion(double times[])
{
	for (int i = 0; i < TimesCount; ++i)
		times[i] /= 24.0;
}

/* ---------------------- Misc Functions ----------------------- */

/* compute the difference between two times  */
static double time_diff(double time1, double time2)
{
	return fix_hour(time2 - time1);
}

//static string int_to_string(int num)
//{
//	char tmp[16];
//	tmp[0] = '\0';
//	sprintf(tmp, "%d", num);
//	return string(tmp);
//}

///* add a leading 0 if necessary */
//static string two_digits_format(int num)
//{
//	char tmp[16];
//	tmp[0] = '\0';
//	sprintf(tmp, "%2.2d", num);
//	return string(tmp);
//}

/* ---------------------- Julian Date Functions ----------------------- */

/* calculate julian date from a calendar date */
double get_julian_date(int year, int month, int day)
{
	if (month <= 2)
	{
		year -= 1;
		month += 12;
	}

	double a = floor(year / 100.0);
	double b = 2 - a + floor(a / 4.0);

	return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + b - 1524.5;
}

/* convert a calendar date to julian date (second method) */
double calc_julian_date(int year, int month, int day)
{
	double j1970 = 2440588.0;
	struct tm date = { 0 };
	date.tm_year = year - 1900;
	date.tm_mon = month - 1;
	date.tm_mday = day;
	date.tm_isdst = -1;		// determine it yourself from system
	time_t ms = mktime(&date);		// seconds since midnight Jan 1, 1970
	double days = floor(ms / (double) (60 * 60 * 24));
	return j1970 + days - 0.5;
}

/* ---------------------- Trigonometric Functions ----------------------- */

/* degree sin */
static double dsin(double d)
{
	return sin(deg2rad(d));
}

/* degree cos */
static double dcos(double d)
{
	return cos(deg2rad(d));
}

/* degree tan */
static double dtan(double d)
{
	return tan(deg2rad(d));
}

/* degree arcsin */
static double darcsin(double x)
{
	return rad2deg(asin(x));
}

/* degree arccos */
static double darccos(double x)
{
	return rad2deg(acos(x));
}

/* degree arctan */
static double darctan(double x)
{
	return rad2deg(atan(x));
}

/* degree arctan2 */
static double darctan2(double y, double x)
{
	return rad2deg(atan2(y, x));
}

/* degree arccot */
static double darccot(double x)
{
	return rad2deg(atan(1.0 / x));
}

/* degree to radian */
static double deg2rad(double d)
{
	return d * pi / 180.0;
}

/* radian to degree */
static double rad2deg(double r)
{
	return r * 180.0 / pi;
}

/* range reduce angle in degrees. */
static double fix_angle(double a)
{
	a = a - 360.0 * floor(a / 360.0);
	a = a < 0.0 ? a + 360.0 : a;
	return a;
}

/* range reduce hours to 0..23 */
static double fix_hour(double a)
{
	a = a - 24.0 * floor(a / 24.0);
	a = a < 0.0 ? a + 24.0 : a;
	return a;
}
