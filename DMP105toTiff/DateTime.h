#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>


class DateTime
{
public:
	std::string localDateTimeLong()
	{
		char *s, *p;

		getLocalTime();
		s = asctime( ptime );
		p = s + strlen( s ) - 1;
		if ( *p == 0x0A )
			*p = 0;
		return s;
	}

	std::string tiffDateTime()
	{
		getLocalTime();
		std::stringstream sstr;
		sstr << setfill('0')
			<< setw(4) << ptime->tm_year+1900 << ":" << setw(2) << ptime->tm_mon+1 << ":" << setw(2) << ptime->tm_mday
			<< " " << setw(2) << ptime->tm_hour << ":" << setw(2) << ptime->tm_min << ":" << setw(2) << ptime->tm_sec;
		return sstr.str();
	}

private:
	time_t clock;
	struct tm *ptime;

	void getLocalTime()
	{
		time( &clock );
		ptime = localtime( &clock );
	}

};
