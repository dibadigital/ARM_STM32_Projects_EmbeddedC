
#ifndef _PrayTimes_H
#define _PrayTimes_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* Prayer Times Calculator */
/*
User's Manual:
http://praytimes.org/manual

Calculation Formulas:
http://praytimes.org/calculation
*/

#define pi 3.14

// Time IDs
typedef enum {
		Fajr,
		Sunrise,
		Dhuhr,
		Asr,
		Sunset,
		Maghrib,
		Isha,
	
		TimesCount
}TimeID;

// Calculation Methods
typedef enum {
	Jafari, 	// Shia Ithna Ashari, Leva Research Institute, Qum
	Karachi,	// University of Islamic Sciences, Karachi 
	ISNA,   	// Islamic Society of North America (ISNA) 
	MWL,    	// Muslim World League (MWL)
	Makkah, 	// Umm al-Qura University, Makkah 
	Egypt,  	// Egyptian General Authority of Survey 
	Custom, 	// Custom Setting
	UoT, 			// Institute of Geophysics, University of Tehran 
	
	CalculationMethodsCount
}CalculationMethod;

// Juristic Methods
typedef enum {
	Shafii,    // Shafii (standard)
	Hanafi,    // Hanafi
}JuristicMethod;

// Adjusting Methods for Higher Latitudes
typedef enum {
	None,      	// No adjustment
	MidNight,  	// middle of night
	OneSeventh,	// 1/7th of night
	AngleBased,	// angle/60th of night
}AdjustingMethod;

typedef struct {
	double fajr_angle;
	bool   maghrib_is_minutes;
	double maghrib_value;	// angle or minutes
	bool   isha_is_minutes;
	double isha_value;		// angle or minutes
}MethodConfig;

typedef struct {
	int Hour;
	int Minute;
	int Second;
}Azan_Time_t;

extern Azan_Time_t praytimes[TimesCount];

/* -------------------- Interface Functions -------------------- */
void init_prayer_times(void);
/* return prayer times for a given date */
void get_prayer_times(int year, int month, int day, double _latitude, double _longitude, double _timezone, double times[]);
/* return prayer times for a given date */
//void get_prayer_times(time_t date, double latitude, double longitude, double timezone, double times[]);
/* set the calculation method  */
void set_calc_method(CalculationMethod method_id);
/* set the juristic method for Asr */
void set_asr_method(JuristicMethod method_id);
/* set adjusting method for higher latitudes */
void set_high_lats_adjust_method(AdjustingMethod method_id);
/* set the angle for calculating Fajr */
void set_fajr_angle(double angle);
/* set the angle for calculating Maghrib */
void set_maghrib_angle(double angle);
/* set the angle for calculating Isha */
void set_isha_angle(double angle);
/* set the minutes after mid-day for calculating Dhuhr */
void set_dhuhr_minutes(double minutes);
/* set the minutes after Sunset for calculating Maghrib */
void set_maghrib_minutes(double minutes);
/* set the minutes after Maghrib for calculating Isha */
void set_isha_minutes(double minutes);
///* get hours and minutes parts of a float time */
//static void get_float_time_parts(double time, int& hours, int& minutes);
///* convert float hours to 24h format */
//static string float_time_to_time24(double time);
///* convert float hours to 12h format */
//static string float_time_to_time12(double time, bool no_suffix = false);
///* convert float hours to 12h format with no suffix */
//static string float_time_to_time12ns(double time);

/* ---------------------- Time-Zone Functions ----------------------- */

///* compute local time-zone for a specific date */
//static double get_effective_timezone(time_t local_time);
///* compute local time-zone for a specific date */
//static double get_effective_timezone_ymd(int year, int month, int day);

/* ---------------------- Calculation Functions ----------------------- */

/* compute declination angle of sun and equation of time */
void sun_position(double jd, double *dd, double *eq_t);
/* compute equation of time */
double equation_of_time(double jd);
/* compute declination angle of sun */
double sun_declination(double jd);
/* compute mid-day (Dhuhr, Zawal) time */
double compute_mid_day(double _t);
/* compute time for a given angle G */
double compute_time(double g, double t);
/* compute the time of Asr */
double compute_asr(int step, double t);  // Shafii: step=1, Hanafi: step=2

/* ---------------------- Compute Prayer Times ----------------------- */

// array parameters must be at least of size TimesCount

/* compute prayer times at given julian date */
void compute_times(double times[]);
/* compute prayer times at given julian date */
void compute_day_times(double times[]);
/* adjust times in a prayer time array */
void adjust_times(double times[]);
/* adjust Fajr, Isha and Maghrib for locations in higher latitudes */
void adjust_high_lat_times(double times[]);
/* the night portion used for adjusting times in higher latitudes */
double night_portion(double angle);
/* convert hours to day portions  */
void day_portion(double times[]);

/* ---------------------- Misc Functions ----------------------- */

/* compute the difference between two times  */
static double time_diff(double time1, double time2);
//static int_to_string(int num);
///* add a leading 0 if necessary */
//static two_digits_format(int num);

/* ---------------------- Julian Date Functions ----------------------- */

/* calculate julian date from a calendar date */
double get_julian_date(int year, int month, int day);
/* convert a calendar date to julian date (second method) */
double calc_julian_date(int year, int month, int day);

/* ---------------------- Trigonometric Functions ----------------------- */

/* degree sin */
static double dsin(double d);
/* degree cos */
static double dcos(double d);
/* degree tan */
static double dtan(double d);
/* degree arcsin */
static double darcsin(double x);
/* degree arccos */
static double darccos(double x);
/* degree arctan */
static double darctan(double x);
/* degree arctan2 */
static double darctan2(double y, double x);
/* degree arccot */
static double darccot(double x);
/* degree to radian */
static double deg2rad(double d);
/* radian to degree */
static double rad2deg(double r);
/* range reduce angle in degrees. */
static double fix_angle(double a);
/* range reduce hours to 0..23 */
static double fix_hour(double a);

#endif
