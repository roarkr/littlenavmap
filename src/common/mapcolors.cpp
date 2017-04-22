/*****************************************************************************
* Copyright 2015-2017 Alexander Barthel albar965@mailbox.org
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

#include "common/mapcolors.h"

#include "mapgui/mapquery.h"
#include "options/optiondata.h"
#include "settings/settings.h"

#include <QPen>
#include <QString>
#include <QApplication>
#include <QPalette>
#include <QSettings>

namespace mapcolors {

// Colors that are updated from confguration file
QColor airportDetailBackColor(255, 255, 255);
QColor airportEmptyColor(110, 110, 110);
QColor toweredAirportColor(15, 70, 130);
QColor unToweredAirportColor(126, 58, 91);
QColor vorSymbolColor(Qt::darkBlue);
QColor ndbSymbolColor(Qt::darkRed);
QColor markerSymbolColor(Qt::darkMagenta);
QColor ilsSymbolColor(Qt::darkGreen);

QColor ilsTextColor(0, 30, 0);
QColor waypointSymbolColor(200, 0, 200);
QPen airwayVictorPen(QColor(150, 150, 150), 1.5);
QPen airwayJetPen(QColor(100, 100, 100), 1.5);
QPen airwayBothPen(QColor(100, 100, 100), 1.5);
QColor airwayTextColor(80, 80, 80);

QColor distanceRhumbColor(80, 80, 80);
QColor rangeRingColor(Qt::red);
QColor rangeRingTextColor(Qt::black);
QColor distanceColor(Qt::black);

/* Elevation profile colors and pens */
QColor profileSkyColor(QColor(204, 204, 255));
QColor profileSkyDarkColor(QColor(20, 20, 90));
QColor profileBackgroundColor(Qt::white);

QColor profileBackgroundDarkColor(QColor(20, 20, 20));
QColor profileLandColor(Qt::darkGreen);
QColor profileLandOutlineColor(Qt::black);

QPen profileWaypointLinePen(Qt::gray, 1, Qt::SolidLine);
QPen profileElevationScalePen(Qt::gray, 1, Qt::SolidLine);
QPen profileSafeAltLinePen(Qt::red, 4, Qt::SolidLine);
QPen profileSafeAltLegLinePen(QColor(255, 100, 0), 3, Qt::SolidLine);

const QColor& colorForAirport(const map::MapAirport& ap)
{
  if(ap.empty() && !ap.waterOnly() && OptionData::instance().getFlags() & opts::MAP_EMPTY_AIRPORTS)
    return airportEmptyColor;
  else if(ap.tower())
    return toweredAirportColor;
  else
    return unToweredAirportColor;
}

const QColor& alternatingRowColor(int row, bool isSort)
{
  /* Alternating colors */
  static const QColor rowBgColor = QApplication::palette().color(QPalette::Active, QPalette::Base);
  static const QColor rowAltBgColor = QApplication::palette().color(QPalette::Active, QPalette::AlternateBase);

  /* Slightly darker background for sort column */
  static const QColor rowSortBgColor = rowBgColor.darker(106);
  static const QColor rowSortAltBgColor = rowAltBgColor.darker(106);

  if((row % 2) == 0)
    return isSort ? rowSortBgColor : rowBgColor;
  else
    return isSort ? rowSortAltBgColor : rowAltBgColor;
}

const QColor& colorForParkingType(const QString& type)
{
  static const QColor rampMilCargo(180, 0, 0);
  static const QColor rampMilCombat(Qt::red);
  static const QColor gate(100, 100, 255);
  static const QColor rampGa(0, 200, 0);
  static const QColor rampCargo(Qt::darkGreen);
  static const QColor fuel(Qt::yellow);
  static const QColor unknown;

  if(type == "RMCB")
    return rampMilCombat;
  else if(type == "RMC")
    return rampMilCargo;
  else if(type.startsWith("G"))
    return gate;
  else if(type.startsWith("RGA") || type.startsWith("DGA"))
    return rampGa;
  else if(type.startsWith("RC"))
    return rampCargo;
  else if(type.startsWith("FUEL"))
    return fuel;
  else
    return unknown;
}

const QIcon& iconForStartType(const QString& type)
{
  static const QIcon runway(":/littlenavmap/resources/icons/startrunway.svg");
  static const QIcon helipad(":/littlenavmap/resources/icons/starthelipad.svg");
  static const QIcon water(":/littlenavmap/resources/icons/startwater.svg");

  static const QIcon empty;
  if(type == "R")
    return runway;
  else if(type == "H")
    return helipad;
  else if(type == "R")
    return water;
  else
    return empty;
}

const QIcon& iconForParkingType(const QString& type)
{
  static const QIcon cargo(":/littlenavmap/resources/icons/parkingrampcargo.svg");
  static const QIcon ga(":/littlenavmap/resources/icons/parkingrampga.svg");
  static const QIcon mil(":/littlenavmap/resources/icons/parkingrampmil.svg");
  static const QIcon gate(":/littlenavmap/resources/icons/parkinggate.svg");
  static const QIcon fuel(":/littlenavmap/resources/icons/parkingfuel.svg");
  static const QIcon empty;

  if(type.startsWith("RM"))
    return mil;
  else if(type.startsWith("G"))
    return gate;
  else if(type.startsWith("RGA") || type.startsWith("DGA"))
    return ga;
  else if(type.startsWith("RC"))
    return cargo;
  else if(type.startsWith("FUEL"))
    return fuel;

  return empty;
}

const QColor& colorForSurface(const QString& surface)
{
  static const QColor concrete("#888888");
  static const QColor grass("#00a000");
  static const QColor water("#808585ff");
  static const QColor asphalt("#707070");
  static const QColor cement("#d0d0d0");
  static const QColor clay("#DEB887");
  static const QColor snow("#dbdbdb");
  static const QColor ice("#d0d0ff");
  static const QColor dirt("#CD853F");
  static const QColor coral("#FFE4C4");
  static const QColor gravel("#c0c0c0");
  static const QColor oilTreated("#2F4F4F");
  static const QColor steelMats("#a0f0ff");
  static const QColor bituminous("#808080");
  static const QColor brick("#A0522D");
  static const QColor macadam("#c8c8c8");
  static const QColor planks("#8B4513");
  static const QColor sand("#F4A460");
  static const QColor shale("#F5DEB3");
  static const QColor tarmac("#909090");
  static const QColor unknown("#ffffff");

  if(surface == "C")
    return concrete;
  else if(surface == "G")
    return grass;
  else if(surface == "W")
    return water;
  else if(surface == "A")
    return asphalt;
  else if(surface == "CE")
    return cement;
  else if(surface == "CL")
    return clay;
  else if(surface == "SN")
    return snow;
  else if(surface == "I")
    return ice;
  else if(surface == "D")
    return dirt;
  else if(surface == "CR")
    return coral;
  else if(surface == "GR")
    return gravel;
  else if(surface == "OT")
    return oilTreated;
  else if(surface == "SM")
    return steelMats;
  else if(surface == "B")
    return bituminous;
  else if(surface == "BR")
    return brick;
  else if(surface == "M")
    return macadam;
  else if(surface == "PL")
    return planks;
  else if(surface == "S")
    return sand;
  else if(surface == "SH")
    return shale;
  else if(surface == "T")
    return tarmac;

  // else if(surface == "NONE" || surface == "UNKNOWN" || surface == "INVALID")
  return unknown;
}

const QPen aircraftTrailPen(float size)
{
  opts::DisplayTrailType type = OptionData::instance().getDisplayTrailType();

  switch(type)
  {
    case opts::DASHED:
      return QPen(OptionData::instance().getTrailColor(), size, Qt::DashLine, Qt::FlatCap, Qt::BevelJoin);

    case opts::DOTTED:
      return QPen(OptionData::instance().getTrailColor(), size, Qt::DotLine, Qt::FlatCap, Qt::BevelJoin);

    case opts::SOLID:
      return QPen(OptionData::instance().getTrailColor(), size, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin);
  }
  return QPen();
}

static QHash<map::MapAirspaceTypes, QColor> airspaceFillColors(
  {
    {map::AIRSPACE_NONE, QColor("#00000000")},
    {map::CENTER, QColor("#00808080")},
    {map::CLASS_A, QColor("#308d0200")},
    {map::CLASS_B, QColor("#30902ece")},
    {map::CLASS_C, QColor("#308594ec")},
    {map::CLASS_D, QColor("#306c5bce")},
    {map::CLASS_E, QColor("#30cc5060")},
    {map::CLASS_F, QColor("#307d8000")},
    {map::CLASS_G, QColor("#30cc8040")},
    {map::TOWER, QColor("#3060808a")},
    {map::CLEARANCE, QColor("#3060808a")},
    {map::GROUND, QColor("#3060808a")},
    {map::DEPARTURE, QColor("#3060808a")},
    {map::APPROACH, QColor("#3060808a")},
    {map::MOA, QColor("#304485b7")},
    {map::RESTRICTED, QColor("#30fd8c00")},
    {map::PROHIBITED, QColor("#30f00909")},
    {map::WARNING, QColor("#30fd8c00")},
    {map::ALERT, QColor("#30fd8c00")},
    {map::DANGER, QColor("#30dd103d")},
    {map::NATIONAL_PARK, QColor("#30509090")},
    {map::MODEC, QColor("#30509090")},
    {map::RADAR, QColor("#30509090")},
    {map::TRAINING, QColor("#30509090")}
  }
  );

static QHash<map::MapAirspaceTypes, QPen> airspacePens(
  {
    {map::AIRSPACE_NONE, QPen(QColor("#00000000"))},
    {map::CENTER, QPen(QColor("#808080"), 1.5)},
    {map::CLASS_A, QPen(QColor("#8d0200"), 2)},
    {map::CLASS_B, QPen(QColor("#902ece"), 2)},
    {map::CLASS_C, QPen(QColor("#8594ec"), 2)},
    {map::CLASS_D, QPen(QColor("#6c5bce"), 2)},
    {map::CLASS_E, QPen(QColor("#cc5060"), 2)},
    {map::CLASS_F, QPen(QColor("#7d8000"), 2)},
    {map::CLASS_G, QPen(QColor("#cc8040"), 2)},
    {map::TOWER, QPen(QColor("#60808a"), 2)},
    {map::CLEARANCE, QPen(QColor("#60808a"), 2)},
    {map::GROUND, QPen(QColor("#60808a"), 2)},
    {map::DEPARTURE, QPen(QColor("#60808a"), 2)},
    {map::APPROACH, QPen(QColor("#60808a"), 2)},
    {map::MOA, QPen(QColor("#4485b7"), 2)},
    {map::RESTRICTED, QPen(QColor("#fd8c00"), 2)},
    {map::PROHIBITED, QPen(QColor("#f00909"), 3)},
    {map::WARNING, QPen(QColor("#fd8c00"), 2)},
    {map::ALERT, QPen(QColor("#fd8c00"), 2)},
    {map::DANGER, QPen(QColor("#dd103d"), 2)},
    {map::NATIONAL_PARK, QPen(QColor("#509090"), 2)},
    {map::MODEC, QPen(QColor("#509090"), 2)},
    {map::RADAR, QPen(QColor("#509090"), 2)},
    {map::TRAINING, QPen(QColor("#509090"), 2)}
  }
  );

static QHash<QString, map::MapAirspaceTypes> airspaceConfigNames(
  {
    {"Center", map::CENTER},
    {"ClassA", map::CLASS_A},
    {"ClassB", map::CLASS_B},
    {"ClassC", map::CLASS_C},
    {"ClassD", map::CLASS_D},
    {"ClassE", map::CLASS_E},
    {"ClassF", map::CLASS_F},
    {"ClassG", map::CLASS_G},
    {"Tower", map::TOWER},
    {"Clearance", map::CLEARANCE},
    {"Ground", map::GROUND},
    {"Departure", map::DEPARTURE},
    {"Approach", map::APPROACH},
    {"Moa", map::MOA},
    {"Restricted", map::RESTRICTED},
    {"Prohibited", map::PROHIBITED},
    {"Warning", map::WARNING},
    {"Alert", map::ALERT},
    {"Danger", map::DANGER},
    {"NationalPark", map::NATIONAL_PARK},
    {"Modec", map::MODEC},
    {"Radar", map::RADAR},
    {"Training", map::TRAINING}
  }
  );

const QColor& colorForAirspaceFill(const map::MapAirspace& airspace)
{
  return airspaceFillColors[airspace.type];
}

const QPen& penForAirspace(const map::MapAirspace& airspace)
{
  return airspacePens[airspace.type];
}

/* Read ARGB color if value exists in settings or update in settings with given value */
void syncColorArgb(QSettings& settings, const QString& key, QColor& color)
{
  if(settings.contains(key))
    color.setNamedColor(settings.value(key).toString());
  else
    settings.setValue(key, color.name(QColor::HexArgb));
}

/* Read color if value exists in settings or update in settings with given value */
void syncColor(QSettings& settings, const QString& key, QColor& color)
{
  if(settings.contains(key))
    color.setNamedColor(settings.value(key).toString());
  else
    settings.setValue(key, color.name());
}

/* Read color and pen width if value exists in settings or update in settings with values of given pen */
void syncPen(QSettings& settings, const QString& key, QPen& pen)
{
  static QHash<QString, Qt::PenStyle> penToStyle(
    {
      {"Solid", Qt::SolidLine},
      {"Dash", Qt::DashLine},
      {"Dot", Qt::DotLine},
      {"DashDot", Qt::DashDotLine},
      {"DashDotDot", Qt::DashDotDotLine},
    });
  static QHash<Qt::PenStyle, QString> styleToPen(
    {
      {Qt::SolidLine, "Solid"},
      {Qt::DashLine, "Dash"},
      {Qt::DotLine, "Dot"},
      {Qt::DashDotLine, "DashDot"},
      {Qt::DashDotDotLine, "DashDotDot"},
    });

  if(settings.contains(key))
  {
    QStringList list = settings.value(key).toStringList();
    if(list.size() >= 1)
    {
      pen.setColor(QColor(list.at(0)));

      if(list.size() >= 2)
        pen.setWidthF(list.at(1).toFloat());

      if(list.size() >= 2)
        pen.setStyle(penToStyle.value(list.at(2), Qt::SolidLine));
    }
  }
  else
    settings.setValue(key, QStringList({pen.color().name(),
                                        QString::number(pen.widthF()),
                                        styleToPen.value(pen.style(), "Solid")}));
}

void syncColors()
{
  QString filename = atools::settings::Settings::instance().getConfigFilename("_mapstyle.ini");

  QSettings colorSettings(filename, QSettings::IniFormat);
  colorSettings.beginGroup("Airport");
  syncColor(colorSettings, "DiagramBackgroundColor", airportDetailBackColor);
  syncColor(colorSettings, "EmptyColor", airportEmptyColor);
  syncColor(colorSettings, "ToweredColor", toweredAirportColor);
  syncColor(colorSettings, "UnToweredColor", unToweredAirportColor);
  colorSettings.endGroup();

  colorSettings.beginGroup("Navaid");
  syncColor(colorSettings, "VorColor", vorSymbolColor);
  syncColor(colorSettings, "NdbColor", ndbSymbolColor);
  syncColor(colorSettings, "MarkerColor", markerSymbolColor);
  syncColor(colorSettings, "IlsColor", ilsSymbolColor);
  syncColor(colorSettings, "IlsTextColor", ilsTextColor);
  syncColor(colorSettings, "WaypointColor", waypointSymbolColor);
  colorSettings.endGroup();

  colorSettings.beginGroup("Airway");
  syncPen(colorSettings, "VictorPen", airwayVictorPen);
  syncPen(colorSettings, "JetPen", airwayJetPen);
  syncPen(colorSettings, "BothPen", airwayBothPen);
  syncColor(colorSettings, "TextColor", airwayTextColor);
  colorSettings.endGroup();

  colorSettings.beginGroup("Marker");
  syncColor(colorSettings, "DistanceRhumbColor", distanceRhumbColor);
  syncColor(colorSettings, "DistanceGreatCircleColor", distanceColor);
  syncColor(colorSettings, "RangeRingColor", rangeRingColor);
  syncColor(colorSettings, "RangeRingTextColor", rangeRingTextColor);
  colorSettings.endGroup();

  colorSettings.beginGroup("Profile");
  syncColor(colorSettings, "SkyColor", profileSkyColor);
  syncColor(colorSettings, "SkyDarkColor", profileSkyDarkColor);
  syncColor(colorSettings, "BackgroundColor", profileBackgroundColor);
  syncColor(colorSettings, "BackgroundDarkColor", profileBackgroundDarkColor);
  syncColor(colorSettings, "LandColor", profileLandColor);
  syncColor(colorSettings, "LandOutlineColor", profileLandOutlineColor);
  syncPen(colorSettings, "WaypointLinePen", profileWaypointLinePen);
  syncPen(colorSettings, "ElevationScalePen", profileElevationScalePen);
  syncPen(colorSettings, "SafeAltLinePen", profileSafeAltLinePen);
  syncPen(colorSettings, "SafeAltLegLinePen", profileSafeAltLegLinePen);
  colorSettings.endGroup();

  // Sync airspace colors ============================================
  colorSettings.beginGroup("Airspace");
  for(const QString& name : airspaceConfigNames.keys())
  {
    map::MapAirspaceTypes type = airspaceConfigNames.value(name);
    syncPen(colorSettings, name + "Pen", airspacePens[type]);
    syncColorArgb(colorSettings, name + "FillColor", airspaceFillColors[type]);
  }
  colorSettings.endGroup();

  colorSettings.sync();
}

} // namespace mapcolors
