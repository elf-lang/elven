//
// Note, week days and months are 0 indexed...
//

typedef enum WeekDay {
	WeekDay_Sun,
	WeekDay_Mon,
	WeekDay_Tue,
	WeekDay_Wed,
	WeekDay_Thu,
	WeekDay_Fri,
	WeekDay_Sat,
	WeekDay_COUNT,
} WeekDay;

typedef enum Month {
	Month_Jan,
	Month_Feb,
	Month_Mar,
	Month_Apr,
	Month_May,
	Month_Jun,
	Month_Jul,
	Month_Aug,
	Month_Sep,
	Month_Oct,
	Month_Nov,
	Month_Dec,
	Month_COUNT,
} Month;

typedef struct DateTime DateTime;
struct DateTime {
	u16 milli_sec;
	u16 sec;
	u16 min;
	u16 hour;
	u16 day;
	u16 month;
	u16 year;
};

typedef u64 DenseTime;

PORTABLEFUNC
DenseTime dense_time_from_date_time(DateTime dt) {
	require(dt.month     <   12, "month out of range (0–11)");
	require(dt.day       <   31, "day out of range (0–30)");
	require(dt.hour      <   24, "hour out of range (0–23)");
	require(dt.min       <   60, "minute out of range (0–59)");
	require(dt.sec       <   60, "second out of range (0–59)");
	require(dt.milli_sec < 1000, "millisecond out of range (0–999)");

	DenseTime result = 0;
	result += dt.year;       result *=   12;
	result += dt.month;      result *=   31;
	result += dt.day;        result *=   24;
	result += dt.hour;       result *=   60;
	result += dt.min;        result *=   60;
	result += dt.sec;        result *= 1000;
	result += dt.milli_sec;  result *=    1;
	return result;
}

PORTABLEFUNC
DateTime date_time_from_dense_time(DenseTime time) {
	DateTime result = {0};
	result.milli_sec  = time % 1000; time /= 1000;
	result.sec        = time %   60; time /=   60;
	result.min        = time %   60; time /=   60;
	result.hour       = time %   24; time /=   24;
	result.day        = time %   31; time /=   31;
	result.month      = time %   12; time /=   12;
	result.year       = (u16) time;
	return result;
}