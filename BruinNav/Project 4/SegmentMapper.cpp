#include "provided.h"
#include <vector>
#include "MyMap.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<GeoCoord, vector<StreetSegment>> m_segmentMap;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

//bool SegmentMapperImpl::hasAttractionThere(const GeoCoord a, const StreetSegment& b)
//{
//	for (int k = 0; k < b.attractions.size(); k++)
//	{
//		if (b.attractions[k].geocoordinates.latitude == a.latitude||b.attractions[k].geocoordinates.longitude==a.longitude)
//			return true;
//	}
//	return false;
//}
void SegmentMapperImpl::init(const MapLoader& ml)//mapping coordinates to a vector of segments with that coord association
{
	for (int k = 0; k < ml.getNumSegments(); k++)
	{
		StreetSegment temp;
		ml.getSegment(k, temp);
		if (m_segmentMap.find(temp.segment.start) == nullptr)//if there isn't that geocoord already there
		{
			vector<StreetSegment> segTemp;//do the association with a brand new vector
			segTemp.push_back(temp);
			m_segmentMap.associate(temp.segment.start, segTemp);
		}
		else if (m_segmentMap.find(temp.segment.start) != nullptr)//otherwise it is there
		{
			vector<StreetSegment>* ptr = m_segmentMap.find(temp.segment.start);//just push this segment onto the existing vector
			ptr->push_back(temp);
		}
		if (m_segmentMap.find(temp.segment.end) == nullptr)//do the same as above, but for the end coordinates of the segments
		{
			vector<StreetSegment> segTemp;
			segTemp.push_back(temp);
			m_segmentMap.associate(temp.segment.end, segTemp);
		}
		else if (m_segmentMap.find(temp.segment.end) != nullptr)
		{
			vector<StreetSegment>*ptr = m_segmentMap.find(temp.segment.end);
			ptr->push_back(temp);
		}
		for (int k = 0; k < temp.attractions.size(); k++)//do the same as above, but for each attraction on the streets
		{
			if (m_segmentMap.find(temp.attractions[k].geocoordinates) == nullptr)
			{
				vector<StreetSegment> segTemp;
				segTemp.push_back(temp);
				m_segmentMap.associate(temp.attractions[k].geocoordinates, segTemp);
			}
			//check to make sure that the segment's start or end coordinate isnt the same as the attraction coordinate, otherwise we'd add the same segment twice which is bad
			else if (m_segmentMap.find(temp.attractions[k].geocoordinates)!= nullptr&&
				temp.attractions[k].geocoordinates.latitude!=temp.segment.start.latitude&&temp.attractions[k].geocoordinates.longitude!= temp.segment.start.longitude&&
				temp.attractions[k].geocoordinates.latitude!= temp.segment.end.latitude&&temp.attractions[k].geocoordinates.longitude!= temp.segment.end.longitude)
			{
				vector<StreetSegment>* ptr = m_segmentMap.find(temp.attractions[k].geocoordinates);
				ptr->push_back(temp);
			}
		}
	}
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	if(m_segmentMap.find(gc)!=nullptr)
	return *(m_segmentMap.find(gc));
	else
	{
		vector<StreetSegment> segments;
		return segments;
	}
	//return segments;  // This compiles, but may not be correct
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
