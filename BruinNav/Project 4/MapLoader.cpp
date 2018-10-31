#include "provided.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	std::vector<StreetSegment*> m_StrSegVec;
	StreetSegment* makeNewStreetSegement(std::string name, std::string coords);
};
StreetSegment* ::MapLoaderImpl::makeNewStreetSegement(std::string name, std::string coords) //helper function
{//passed in a string with the name of the segment, and a string of lat lon of start and end of segement
	int count1 = 0;
	StreetSegment* temp = new StreetSegment;
	temp->streetName = name;
	for (int k = 0; k < coords.size(); k++)//for the lat of start segment
	{
		if (coords[k] == ',')//iterate till we get to the comma
			break;
		count1++;
	}

	std::string a = coords.substr(0, count1);//take that substring for lat of start
	int count2 = 0;
	int add1 = 1;
	if (coords[count1+1] == ' ')//for the start of the next substring we have to move over one position for the , and an optional one for a ' ' character
		add1++;
	for (int k = count1+add1; k < coords.size(); k++)//count how many characters to include in the next substr for the longitude
	{
		if (coords[k] == ' ')//start and end coords are separated by a space 
			break;
		count2++;
	}

	std::string b = coords.substr(count1+add1, count2);
//	cerr << b;
	int count3 = 0;
	int add2 = 1;
	for (int k = count2+count1+add1+add2; k < coords.size(); k++)//count how many for the ending latitude
	{
		if (coords[k] == ',')
			break;
		count3++;
	}
	std::string c = coords.substr(count2+count1 +add1+add2, count3);
//	cerr << c;
	int add3 = 1;
	if (coords[count2 + count1 + add1 + add2 + count3 + 1] == ' ')
		add3++;
	std::string d = coords.substr(count2 + count1 + count3 + add1 + add2 + add3);
	//cerr << d;
	GeoCoord st(a, b);
	GeoCoord en(c, d);
	GeoSegment seg(st, en);
	temp->segment = seg;
	return temp;
	
}
MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
	for (int k = 0; k < m_StrSegVec.size(); k++)
		delete m_StrSegVec[k];
}

bool MapLoaderImpl::load(string mapFile)
{
	int pos = 0;
	ifstream infile(mapFile);
	std::string s;
	std::string t;
	while (getline(infile, s))//this gets the first line which is a street name
	{
		getline(infile, t);//the next line ahs the start end lat long coords
		m_StrSegVec.push_back(makeNewStreetSegement(s, t));//create the new segment
		getline(infile, s);//this is the number of attractions on the segment
		int iter = stoi(s);
		for (int k = 0; k < iter; k++)//for as many attractions as there are
		{
			int count1 = 0;
			getline(infile, s);//get string of the segment 
			for (int k = 0; k < s.size(); k++)
			{
				if (s[k] == '|')//up until this character is the attraction name
					break;
				count1++;
			}
			string tname = s.substr(0, count1);
			int count2 = 0;
			int add1 = 1;
			if (s[count1 + 1] == ' ')
				add1++;
			for (int k = count1 + add1; k < s.size(); k++)//count the number of characters in hte latitude
			{
				if (s[k] == ',')//latitude ends at the comma
					break;
				count2++;
			}
			int add2 = 1;
			if (s[count1 + add1 + count2 + 1] == ' ')//if there is a space then we have to move over an extra space in the string
				add2++;
			string tlat = s.substr(count1 + add1, count2);//take the two substr of lat and longitude
			string tlon = s.substr(count1 + add1 + count2 + add2);
			Attraction atemp;
			atemp.name = tname;
			GeoCoord gtemp(tlat, tlon);
			atemp.geocoordinates = gtemp;
			m_StrSegVec[pos]->attractions.push_back(atemp);//add that attraction onto the street segment
			//cerr << "here";
		}
		pos++;
	}
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_StrSegVec.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{if(segNum>=getNumSegments()||segNum<0)
	return false;
else
{
	seg = *(m_StrSegVec[segNum]);
	return true;
}
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
	return m_impl->getSegment(segNum, seg);
}
