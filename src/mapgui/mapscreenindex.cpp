/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "mapgui/mapscreenindex.h"

#include "common/maptypes.h"

#include "route/routecontroller.h"
#include "mapgui/mapscale.h"
#include "mapgui/mapwidget.h"
#include "mapgui/mappaintlayer.h"
#include "mapgui/maplayer.h"
#include "common/maptypes.h"
#include "common/maptools.h"
#include "mapgui/mapquery.h"
#include "common/coordinateconverter.h"
#include "common/constants.h"
#include "settings/settings.h"

MapScreenIndex::MapScreenIndex(MapWidget *parentWidget, MapQuery *mapQueryParam, MapPaintLayer *mapPaintLayer)
  : mapWidget(parentWidget), mapQuery(mapQueryParam), paintLayer(mapPaintLayer)
{
}

MapScreenIndex::~MapScreenIndex()
{

}

void MapScreenIndex::updateAirwayScreenLines(const Marble::GeoDataLatLonAltBox& curBox)
{
  using atools::geo::Pos;
  using maptypes::MapAirway;

  airwayLines.clear();

  CoordinateConverter conv(mapWidget->viewport());
  const MapScale *scale = paintLayer->getMapScale();
  bool showJet = paintLayer->getShownMapFeatures().testFlag(maptypes::AIRWAYJ);
  bool showVictor = paintLayer->getShownMapFeatures().testFlag(maptypes::AIRWAYV);

  if(scale->isValid() && paintLayer->getMapLayer()->isAirway() && (showJet || showVictor))
  {
    const QList<MapAirway> *airways = mapQuery->getAirways(curBox, paintLayer->getMapLayer(), false);

    for(int i = 0; i < airways->size(); i++)
    {
      const MapAirway& airway = airways->at(i);
      if((airway.type == maptypes::VICTOR && !showVictor) || (airway.type == maptypes::JET && !showJet))
        continue;

      Marble::GeoDataLatLonBox airwaybox(airway.bounding.getNorth(), airway.bounding.getSouth(),
                                         airway.bounding.getEast(), airway.bounding.getWest(),
                                         Marble::GeoDataCoordinates::Degree);

      if(airwaybox.intersects(curBox))
      {
        float distanceMeter = airway.from.distanceMeterTo(airway.to);
        // Approximate the needed number of line segments
        float numSegments = std::min(std::max(scale->getPixelIntForMeter(distanceMeter) / 20.f, 4.f), 72.f);
        float step = 1.f / numSegments;

        for(int j = 0; j < numSegments; j++)
        {
          float cur = step * static_cast<float>(j);
          int xs1, ys1, xs2, ys2;
          bool va = conv.wToS(airway.from.interpolate(airway.to, distanceMeter, cur), xs1, ys1);
          bool vb = conv.wToS(airway.from.interpolate(airway.to, distanceMeter, cur + step), xs2, ys2);

          if(va || vb)
            airwayLines.append(std::make_pair(airway.id, QLine(xs1, ys1, xs2, ys2)));
        }
      }
    }
  }
}

void MapScreenIndex::saveState()
{
  atools::settings::Settings& s = atools::settings::Settings::instance();
  s.setValueVar(lnm::MAP_DISTANCEMARKERS, QVariant::fromValue<QList<maptypes::DistanceMarker> >(distanceMarks));
  s.setValueVar(lnm::MAP_RANGEMARKERS, QVariant::fromValue<QList<maptypes::RangeMarker> >(rangeMarks));
}

void MapScreenIndex::restoreState()
{
  atools::settings::Settings& s = atools::settings::Settings::instance();
  distanceMarks = s.valueVar(lnm::MAP_DISTANCEMARKERS).value<QList<maptypes::DistanceMarker> >();
  rangeMarks = s.valueVar(lnm::MAP_RANGEMARKERS).value<QList<maptypes::RangeMarker> >();
}

RouteMapObjectList& MapScreenIndex::getRouteHighlights()
{
  return routeHighlights;
}

const RouteMapObjectList& MapScreenIndex::getRouteHighlights() const
{
  return routeHighlights;
}

maptypes::MapSearchResult& MapScreenIndex::getHighlights()
{
  return highlights;
}

const maptypes::MapSearchResult& MapScreenIndex::getHighlights() const
{
  return highlights;
}

QList<maptypes::RangeMarker>& MapScreenIndex::getRangeMarks()
{
  return rangeMarks;
}

const QList<maptypes::RangeMarker>& MapScreenIndex::getRangeMarks() const
{
  return rangeMarks;
}

QList<maptypes::DistanceMarker>& MapScreenIndex::getDistanceMarks()
{
  return distanceMarks;
}

const QList<maptypes::DistanceMarker>& MapScreenIndex::getDistanceMarks() const
{
  return distanceMarks;
}

void MapScreenIndex::updateRouteScreenGeometry()
{
  using atools::geo::Pos;

  const RouteMapObjectList& routeMapObjects = mapWidget->getRouteController()->getRouteMapObjects();

  routeLines.clear();
  routePoints.clear();

  QList<std::pair<int, QPoint> > airportPoints;
  QList<std::pair<int, QPoint> > otherPoints;

  CoordinateConverter conv(mapWidget->viewport());
  const MapScale *scale = paintLayer->getMapScale();
  if(scale->isValid())
  {
    Pos p1;

    for(int i = 0; i < routeMapObjects.size(); i++)
    {
      const Pos& p2 = routeMapObjects.at(i).getPosition();
      maptypes::MapObjectTypes type = routeMapObjects.at(i).getMapObjectType();
      int x2, y2;
      conv.wToS(p2, x2, y2);

      if(type == maptypes::AIRPORT && (i == 0 || i == routeMapObjects.size() - 1))
        airportPoints.append(std::make_pair(i, QPoint(x2, y2)));
      else
        otherPoints.append(std::make_pair(i, QPoint(x2, y2)));

      if(p1.isValid())
      {
        float distanceMeter = p2.distanceMeterTo(p1);
        // Approximate the needed number of line segments
        float numSegments = std::min(std::max(scale->getPixelIntForMeter(distanceMeter) / 20.f, 4.f), 72.f);
        float step = 1.f / numSegments;

        for(int j = 0; j < numSegments; j++)
        {
          float cur = step * static_cast<float>(j);
          int xs1, ys1, xs2, ys2;
          bool visible1 = conv.wToS(p1.interpolate(p2, distanceMeter, cur), xs1, ys1);
          bool visible2 = conv.wToS(p1.interpolate(p2, distanceMeter, cur + step), xs2, ys2);

          if(visible1 || visible2)
            routeLines.append(std::make_pair(i - 1, QLine(xs1, ys1, xs2, ys2)));
        }
      }
      p1 = p2;
    }

    routePoints.append(airportPoints);
    routePoints.append(otherPoints);
  }
}

void MapScreenIndex::getAllNearest(int xs, int ys, int screenDistance,
                                   maptypes::MapSearchResult& mapSearchResult)
{
  CoordinateConverter conv(mapWidget->viewport());
  const MapLayer *mapLayer = paintLayer->getMapLayer();
  const MapLayer *mapLayerEffective = paintLayer->getMapLayerEffective();

  // Airways use a screen coordinate buffer
  getNearestAirways(xs, ys, screenDistance, mapSearchResult);

  if(paintLayer->getShownMapFeatures().testFlag(maptypes::ROUTE))
    // Get copies from route
    mapWidget->getRouteController()->getRouteMapObjects().getNearest(conv, xs, ys, screenDistance,
                                                                     mapSearchResult);

  // Get copies from highlightMapObjects
  getNearestHighlights(xs, ys, screenDistance, mapSearchResult);

  // Get objects from cache - alread present objects will be skipped
  mapQuery->getNearestObjects(conv, mapLayer, mapLayerEffective->isAirportDiagram(),
                              paintLayer->getShownMapFeatures() &
                              (maptypes::AIRPORT_ALL | maptypes::VOR | maptypes::NDB | maptypes::WAYPOINT |
                               maptypes::MARKER | maptypes::AIRWAYJ | maptypes::AIRWAYV),
                              xs, ys, screenDistance, mapSearchResult);

  // Update all incomplete objects, especially from search
  for(maptypes::MapAirport& obj : mapSearchResult.airports)
    if(!obj.complete())
      mapQuery->getAirportById(obj, obj.getId());
}

void MapScreenIndex::getNearestHighlights(int xs, int ys, int screenDistance,
                                          maptypes::MapSearchResult& mapobjects)
{
  using namespace maptypes;

  CoordinateConverter conv(mapWidget->viewport());
  int x, y;

  using maptools::insertSortedByDistance;

  for(const maptypes::MapAirport& obj : highlights.airports)
    if(conv.wToS(obj.position, x, y))
      if((atools::geo::manhattanDistance(x, y, xs, ys)) < screenDistance)
        insertSortedByDistance(conv, mapobjects.airports, &mapobjects.airportIds, xs, ys, obj);

  for(const maptypes::MapVor& obj : highlights.vors)
    if(conv.wToS(obj.position, x, y))
      if((atools::geo::manhattanDistance(x, y, xs, ys)) < screenDistance)
        insertSortedByDistance(conv, mapobjects.vors, &mapobjects.vorIds, xs, ys, obj);

  for(const maptypes::MapNdb& obj : highlights.ndbs)
    if(conv.wToS(obj.position, x, y))
      if((atools::geo::manhattanDistance(x, y, xs, ys)) < screenDistance)
        insertSortedByDistance(conv, mapobjects.ndbs, &mapobjects.ndbIds, xs, ys, obj);

  for(const maptypes::MapWaypoint& obj : highlights.waypoints)
    if(conv.wToS(obj.position, x, y))
      if((atools::geo::manhattanDistance(x, y, xs, ys)) < screenDistance)
        insertSortedByDistance(conv, mapobjects.waypoints, &mapobjects.waypointIds, xs, ys, obj);
}

int MapScreenIndex::getNearestDistanceMarksIndex(int xs, int ys, int screenDistance)
{
  CoordinateConverter conv(mapWidget->viewport());
  int index = 0;
  int x, y;
  for(const maptypes::DistanceMarker& marker : distanceMarks)
  {
    if(conv.wToS(marker.to, x, y))
      if((atools::geo::manhattanDistance(x, y, xs, ys)) < screenDistance)
        return index;

    index++;
  }
  return -1;
}

int MapScreenIndex::getNearestRoutePointIndex(int xs, int ys, int screenDistance)
{
  if(!paintLayer->getShownMapFeatures().testFlag(maptypes::ROUTE))
    return -1;

  int minIndex = -1;
  int minDist = std::numeric_limits<int>::max();

  for(const std::pair<int, QPoint>& rsp : routePoints)
  {
    const QPoint& point = rsp.second;
    int dist = atools::geo::manhattanDistance(point.x(), point.y(), xs, ys);
    if(dist < minDist && dist < screenDistance)
    {
      minDist = dist;
      minIndex = rsp.first;
    }
  }
  return minIndex;
}

void MapScreenIndex::getNearestAirways(int xs, int ys, int screenDistance, maptypes::MapSearchResult& result)
{
  if(!paintLayer->getShownMapFeatures().testFlag(maptypes::AIRWAYJ) &&
     !paintLayer->getShownMapFeatures().testFlag(maptypes::AIRWAYV))
    return;

  for(int i = 0; i < airwayLines.size(); i++)
  {
    const std::pair<int, QLine>& line = airwayLines.at(i);

    QLine l = line.second;

    if(atools::geo::distanceToLine(xs, ys, l.x1(), l.y1(), l.x2(), l.y2(), true) < screenDistance)
    {
      maptypes::MapAirway airway;
      mapQuery->getAirwayById(airway, line.first);
      result.airways.append(airway);
    }
  }
}

int MapScreenIndex::getNearestRouteLegIndex(int xs, int ys, int screenDistance)
{
  if(!paintLayer->getShownMapFeatures().testFlag(maptypes::ROUTE))
    return -1;

  int minIndex = -1;
  float minDist = std::numeric_limits<float>::max();

  for(int i = 0; i < routeLines.size(); i++)
  {
    const std::pair<int, QLine>& line = routeLines.at(i);

    QLine l = line.second;

    float dist = atools::geo::distanceToLine(xs, ys, l.x1(), l.y1(), l.x2(), l.y2(), true);

    if(dist < minDist && dist < screenDistance)
    {
      minDist = dist;
      minIndex = line.first;
    }
  }
  return minIndex;
}

int MapScreenIndex::getNearestRangeMarkIndex(int xs, int ys, int screenDistance)
{
  CoordinateConverter conv(mapWidget->viewport());
  int index = 0;
  int x, y;
  for(const maptypes::RangeMarker& marker : rangeMarks)
  {
    if(conv.wToS(marker.center, x, y))
      if((atools::geo::manhattanDistance(x, y, xs, ys)) < screenDistance)
        return index;

    index++;
  }
  return -1;
}
