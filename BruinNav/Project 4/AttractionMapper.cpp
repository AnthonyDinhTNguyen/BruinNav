#include "provided.h"
#include <string>
#include"MyMap.h"
#include <cctype>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<std::string, GeoCoord> m_attractionMap;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	for (int i = 0; i < ml.getNumSegments(); i++)
	{
		StreetSegment temp;
		ml.getSegment(i, temp);
		for (int k = 0; k < temp.attractions.size(); k++)
		{
			string a= temp.attractions[k].name;
			string c="";
			for (int z = 0; z < a.size(); z++)//to make this case insensitive we have to lowercase names
			{
				c += tolower(a[z]);
			}
			GeoCoord b = temp.attractions[k].geocoordinates;
			m_attractionMap.associate(c, b);//use mymap to do the association between the attraction and their geocoordinates for easy lookup
		}
	}
}
bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	string tempString = "";
	for (int k = 0; k < attraction.size(); k++)//for case insensitivity 
		tempString += tolower(attraction[k]);
	const GeoCoord* tempCoord = m_attractionMap.find(tempString);
	if(tempCoord==nullptr)
	return false; 
	else
	{
		gc = *(tempCoord);
		return true;
	}
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
