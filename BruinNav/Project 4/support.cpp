#include "support.h"
#include "provided.h"
bool operator==(const GeoCoord&lhs, const GeoCoord&rhs)//need these operators for the mymap class to work as well as for the navigator, straightforward comparisons
{
	if (lhs.longitude == rhs.longitude&&lhs.latitude == rhs.latitude)
		return true;
	return false;
}
bool operator<(const GeoCoord&lhs, const GeoCoord&rhs)
{
	if (lhs.longitude < rhs.longitude)
		return true;
	if (lhs.latitudeText < rhs.latitudeText)
		return true;
	return false;
}
bool operator>(const GeoCoord&lhs, const GeoCoord&rhs)
{
	if (lhs.longitude > rhs.longitude)
		return true;
	if (lhs.latitude > rhs.latitude)
		return true;
	return false;
}
