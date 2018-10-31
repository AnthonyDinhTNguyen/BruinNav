#include "provided.h"
#include <string>
#include <vector>
#include <queue>
#include"MyMap.h"
#include <list>
#include<float.h>
#include<iostream>
#include <algorithm>
using namespace std;
class Noder
{
public:
	Noder* parent;
	GeoCoord m_coord;
	float H;//distance current node to the end node
	float G;//distance from start to prev node + prev node to current node
	float F;//sum of G and H
};
class Noder_Compare
{
public:
	bool operator()(const Noder& lhs, const Noder& rhs)
	{
		if (lhs.F < rhs.F)
			return true;
		return false;
	}
};
class NavigatorImpl
{
public:
	NavigatorImpl();
	~NavigatorImpl();
	bool loadMapData(string mapFile);
	NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	SegmentMapper m_segmentMapper;
	AttractionMapper m_attractionMapper;
	void makeNavSegments(vector<GeoCoord>& points, vector<NavSegment>&dir)const;
	void calculateDirection(const GeoSegment& a,string& directionStr)const;
	double calculateDistance(const GeoSegment& a)const;
	void addPoints(Noder temp, vector<Noder>& m_Openvec, vector<Noder>& m_Closedvec)const;
};

NavigatorImpl::NavigatorImpl()
{

}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	MapLoader m;
	m.load(mapFile);
	m_segmentMapper.init(m);
	m_attractionMapper.init(m);
	return true; 
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	bool done = false;
	vector<Noder>m_Openvec;//keep track of where we still have to go
	list<Noder> m_total;//keeps a storage of the total nodes that does not change
	vector<Noder> m_Closedvec;//keeps track of where we are no longer going to visit
	GeoCoord gc;
	if (m_attractionMapper.getGeoCoord(start, gc) == false)//checks for validity of the attractions
		return NAV_BAD_SOURCE;
	else if (m_attractionMapper.getGeoCoord(end, gc) == false)
		return NAV_BAD_DESTINATION;
	
	GeoCoord StartCoord;//get the coordinate of starting location
	m_attractionMapper.getGeoCoord(start, StartCoord);
	Noder one;//create a node
	one.m_coord = StartCoord;
	one.F = 0;//starting f g and h scores for A* are always just zero
	one.G = 0;
	one.H = 0;
	one.parent = nullptr;
	m_Openvec.push_back(one);//add it to where we have to visit
	GeoCoord DestCoord;//find the destination coord so we know when we've reached the goal
	m_attractionMapper.getGeoCoord(end, DestCoord);
	Noder dest;
	dest.m_coord = DestCoord;
	while (m_Openvec.size() > 0&&done==false)
	{	
		int pos = 0;
		float minF = FLT_MAX;
		for (int k = 0; k < m_Openvec.size(); k++)//find the node with the lowest F score in the open list 
		{
			if (m_Openvec[k].F < minF)
			{
				minF = m_Openvec[k].F;
				pos = k;
			}
		}
		Noder current = m_Openvec[pos];//save it to the current node and...
		m_total.push_back(current);
		vector<Noder>::iterator it = m_Openvec.begin();//"pop" it off the list
		it += pos;
		m_Openvec.erase(it);
		vector<StreetSegment> a;
		a = m_segmentMapper.getSegments(current.m_coord);
		for (int k = 0; k < a.size(); k++)//for each of the associated segments, find if they have the end attraction on them
		{
			for (int i = 0; i < a[k].attractions.size(); i++)
			{
				if (a[k].attractions[i].geocoordinates == DestCoord)
				{
					done = true;
					list<Noder>::iterator it = m_total.begin();
					dest.parent = &(*(find(m_total.begin(), m_total.end(),current)));
					break;
				}
			}
			if (done == true)
				break;
			Noder temp;//if you havent found the end attraction
			if (!(a[k].segment.start == current.m_coord))//check that the start of the segment isn't the same as the current node we are on
			{//if it isn't then create the new node to push on (don't push it on yet)
				temp.m_coord = a[k].segment.start;
				temp.G = current.G + distanceEarthMiles(temp.m_coord, current.m_coord);
				temp.parent = &(*(find(m_total.begin(), m_total.end(), current)));//setting the pointer to the current pointer that was stored in the total list
				temp.H = distanceEarthMiles(temp.m_coord, DestCoord);
				temp.F = temp.G + temp.H;
			}
			else
			{	//otherwise we are invalidating this node
				temp.F = FLT_MAX;
				temp.parent = nullptr;
			}
			Noder temp2;
			if (!(a[k].segment.end == current.m_coord))
			{//check the same as above for the end coordinate
				temp2.m_coord = a[k].segment.end;
				temp2.G = current.G + distanceEarthMiles(temp2.m_coord, current.m_coord);
				temp2.parent = &(*(find(m_total.begin(), m_total.end(), current)));
				temp2.H = distanceEarthMiles(temp2.m_coord, DestCoord);
				temp2.F = temp2.G + temp2.H;
			}
			else {
				temp2.F = FLT_MAX;
				temp2.parent = nullptr;
			}

				if (temp2.F < temp.F)//want to add on the one with the lower F score first
				{
					if (temp2.parent != nullptr)//checks for invalidation
						addPoints(temp2, m_Openvec, m_Closedvec);//use the helper function to add the coord
					if (temp.parent != nullptr)
						addPoints(temp, m_Openvec, m_Closedvec);
				}
				else if (temp.F < temp2.F)
				{
					if (temp.parent != nullptr)
						addPoints(temp, m_Openvec, m_Closedvec);
					if (temp2.parent != nullptr)
						addPoints(temp2, m_Openvec, m_Closedvec);
				}
		}
		m_Closedvec.push_back(current);//when we've finished analyzing the node, push it onto the closed list
	}
	if(done == false)
		return NAV_NO_ROUTE; 
	vector<GeoCoord> route;
	Noder* ptr = &dest;
	while (ptr != nullptr)//trace back the parent pointers to find the route
	{
		route.push_back(ptr->m_coord);
		ptr = ptr->parent;
	}
	if (done == true)
	{
		makeNavSegments(route, directions);//use helper function to create the NavSegments
		return NAV_SUCCESS;
	}
	return NAV_NO_ROUTE;
	
}
void NavigatorImpl::addPoints(Noder temp, vector<Noder>&m_Openvec, vector<Noder>& m_Closedvec)const
{
	bool add = true;
	for (int k = 0; k < m_Openvec.size(); k++)//only add node to the open list if it isnt already in the closed or open list, or if it is, if the F score is lower than the ones for the existing coords
	{
		if (m_Openvec[k].m_coord == temp.m_coord&&m_Openvec[k].F <= temp.F)//allowed to refisit if the new temp node has lower f score than the one already in the closed list
		{
			add = false;
		}
	}
	if (add == true)
	{
		for (int k = 0; k < m_Closedvec.size(); k++)
		{
			if (m_Closedvec[k].m_coord == temp.m_coord&&m_Closedvec[k].F <= temp.F)
				add = false;
		}
	}
	if (add == true)
	{
		m_Openvec.push_back(temp);
	}

}
void NavigatorImpl::makeNavSegments(vector<GeoCoord>& points, vector<NavSegment>&dir)const
{
	reverse(points.begin(), points.end());//reverses coordinate vector because it used to be from ending coord to start coord
	string directionStr;
	GeoSegment a(points[0], points[1]);//first segment always a proceed segment
	calculateDirection(a,directionStr);
	double dist = distanceEarthMiles(points[0], points[1]);
	vector<StreetSegment> b = m_segmentMapper.getSegments(points[0]);
	string name;
	if (points.size() == 2)//if there is only one segment then we have to find it's name in the attractions
	{
		for (int k = 0; k < b.size(); k++)
			for (int i = 0; i < b[k].attractions.size(); i++)
				if (b[k].attractions[i].geocoordinates == points[1])//finding the right name for the street
					name = b[k].streetName;
		NavSegment seg(directionStr, name, dist, a);
		dir.push_back(seg);
		return;
	}
	else//otherwise, the ending coord will be a streetsegment start/end
	{
		for (int k = 0; k < b.size(); k++)
			if (b[k].segment.start == points[1] || b[k].segment.end == points[1])
				name = b[k].streetName;
	}
	NavSegment seg(directionStr, name, dist, a);
	dir.push_back(seg);
	int pos = 1;
	while (pos < points.size()-2)//doing the middle segments 
	{
		GeoSegment temp(points[pos], points[pos + 1]);
		b=m_segmentMapper.getSegments(temp.start);
		for (int k=0;k<b.size();k++)
			if (b[k].segment.start == points[pos + 1] || b[k].segment.end == points[pos + 1])//finding right segment associated with the coord
			{
				if (b[k].streetName != name)//if the name of the segment is not the same as the existing name then we are at a turn 
				{
					name = b[k].streetName;//set the new street name and make the turn segment
					string turn;
					GeoSegment prev(points[pos - 1], points[pos]);
					if (angleBetween2Lines(prev, temp) < 180)//figure out what direction to turn;
						turn = "left";
					else
						turn = "right";
					NavSegment tempSeg(turn, name);
					dir.push_back(tempSeg);
					//break;
				}
				//else
				//{
					calculateDirection(temp, directionStr);//then do the proceed segment 
					NavSegment tempSeg(directionStr, name, calculateDistance(temp), temp);
					dir.push_back(tempSeg);
					pos++;
					break;
				//}
			}
	}
	GeoSegment finalSeg(points[points.size() - 2], points[points.size() - 1]);//for the final segment it must be a proceed segment
	b = m_segmentMapper.getSegments(finalSeg.end);
	for (int k=0;k<b.size();k++)
		for (int i = 0; i < b[k].attractions.size(); i++)
		{
			if (b[k].attractions[i].geocoordinates == points[points.size() - 1])
			{
				if (b[k].streetName != name)//check if you need to turn
				{
					name = b[k].streetName;
					string turn;
					GeoSegment prev(points[points.size() - 3], points[points.size() - 2]);
					if (angleBetween2Lines(prev, finalSeg) < 180)//same as above
						turn = "left";
					else
						turn = "right";
					NavSegment tempSeg(turn, name);
					dir.push_back(tempSeg);
				}
				calculateDirection(finalSeg, directionStr);
				NavSegment finish(directionStr, name, calculateDistance(finalSeg), finalSeg);//the name will have to be the same as the last segment
				dir.push_back(finish);
				return;
			}
		}
	
}
void NavigatorImpl::calculateDirection(const GeoSegment& a,string& directionStr)const 
{
	if (angleOfLine(a) >= 0 && angleOfLine(a) <= 22.5)
		directionStr = "east";
	else if (angleOfLine(a) > 22.5&&angleOfLine(a) <= 67.5)
		directionStr = "northeast";
	else if (angleOfLine(a) > 67.5&&angleOfLine(a) <= 112.5)
		directionStr = "north";
	else if (angleOfLine(a) > 112.5&&angleOfLine(a) <= 157.5)
		directionStr = "northwest";
	else if (angleOfLine(a) > 157.5&&angleOfLine(a) <= 202.5)
		directionStr = "west";
	else if (angleOfLine(a) > 202.5&&angleOfLine(a) <= 247.5)
		directionStr = "southwest";
	else if (angleOfLine(a) > 247.5&&angleOfLine(a) <= 292.5)
		directionStr = "south";
	else if (angleOfLine(a) > 292.5&&angleOfLine(a) <= 337.5)
		directionStr = "southeast";
	else if (angleOfLine(a) > 337.5&&angleOfLine(a) < 360)
		directionStr = "east";
}
double NavigatorImpl::calculateDistance(const GeoSegment& a) const
{
	return distanceEarthMiles(a.start, a.end);
}
bool operator==(const Noder& lhs, const Noder& rhs)
{
	return lhs.m_coord == rhs.m_coord;
}
bool operator <(const Noder&lhs, const Noder& rhs)
{
	return lhs.m_coord < rhs.m_coord;
}
bool operator >(const Noder& lhs, const Noder&rhs)
{
	return lhs.m_coord > rhs.m_coord;
}
//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
	m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
	delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
	return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
	return m_impl->navigate(start, end, directions);
}





//#include "provided.h"
//#include <string>
//#include <vector>
//#include <queue>
//#include"MyMap.h"
//#include <list>
//using namespace std;
//class Noder
//{
//public:
//	GeoCoord m_coord;
//	int H;
//	int G;
//	int F;
//};
//class NavigatorImpl
//{
//public:
//	NavigatorImpl();
//	~NavigatorImpl();
//	bool loadMapData(string mapFile);
//	NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
//private:
//	SegmentMapper m_segmentMapper;
//	AttractionMapper m_attractionMapper;
//
//};
//
//NavigatorImpl::NavigatorImpl()
//{
//
//}
//
//NavigatorImpl::~NavigatorImpl()
//{
//}
//
//bool NavigatorImpl::loadMapData(string mapFile)
//{
//	MapLoader m;
//	m.load(mapFile);
//	m_segmentMapper.init(m);
//	m_attractionMapper.init(m);
//	return true;  // when would this ever be false?//////////////////////////////////////////////
//}
//
//NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
//{
//	bool done = false;
//	queue<Noder> m_Openqueue;
//	MyMap<Noder, Noder> m_pathMap;
//	GeoCoord gc;
//	if (m_attractionMapper.getGeoCoord(start, gc) == false)
//		return NAV_BAD_SOURCE;
//	else if (m_attractionMapper.getGeoCoord(end, gc) == false)
//		return NAV_BAD_DESTINATION;
//	GeoCoord StartCoord;
//	m_attractionMapper.getGeoCoord(start, StartCoord);
//	Noder one;
//	one.m_coord = StartCoord;
//	m_Openqueue.push(one);
//	GeoCoord DestCoord;
//	m_attractionMapper.getGeoCoord(end, DestCoord);
//	Noder dest;
//	dest.m_coord = DestCoord;
//	while (m_Openqueue.size() > 0 && done == false)
//	{
//		Noder current = m_Openqueue.front();
//		m_Openqueue.pop();
//		vector<StreetSegment> a;
//		a = m_segmentMapper.getSegments(current.m_coord);
//		for (int k = 0; k < a.size(); k++)
//		{
//			for (int i = 0; i < a[k].attractions.size(); i++)
//			{
//				if (a[k].attractions[i].geocoordinates == DestCoord)
//				{
//					done = true;
//					m_Openqueue.push(dest);
//					m_pathMap.associate(dest, current);
//					//return NAV_SUCCESS;
//					break;
//				}
//			}
//			Noder temp;
//			temp.m_coord = a[k].segment.start;
//			if (m_pathMap.find(temp) == nullptr)
//			{
//				m_Openqueue.push(temp);
//				m_pathMap.associate(temp, current);
//			}
//			Noder temp2;
//			temp2.m_coord = a[k].segment.end;
//			if (m_pathMap.find(temp2) == nullptr)
//			{
//				m_Openqueue.push(temp2);
//				m_pathMap.associate(temp2, current);
//			}
//		}
//	}
//	if (done == false)
//		return NAV_NO_ROUTE;  // This compiles, but may not be correct
//	Noder *ptr = m_pathMap.find(dest);
//	//GeoSegment(ptr->m_coord, dest.m_coord);
//	if (done == true)
//	return NAV_SUCCESS;
//}
//bool operator==(const Noder& lhs, const Noder& rhs)
//{
//	return lhs.m_coord == rhs.m_coord;
//}
//bool operator <(const Noder&lhs, const Noder& rhs)
//{
//	return lhs.m_coord < rhs.m_coord;
//}
//bool operator >(const Noder& lhs, const Noder&rhs)
//{
//	return lhs.m_coord > rhs.m_coord;
//}
////******************** Navigator functions ************************************
//
//// These functions simply delegate to NavigatorImpl's functions.
//// You probably don't want to change any of this code.
//
//Navigator::Navigator()
//{
//	m_impl = new NavigatorImpl;
//}
//
//Navigator::~Navigator()
//{
//	delete m_impl;
//}
//
//bool Navigator::loadMapData(string mapFile)
//{
//	return m_impl->loadMapData(mapFile);
//}
//
//NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
//{
//	return m_impl->navigate(start, end, directions);
//}

