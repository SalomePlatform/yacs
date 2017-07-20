#include "AbstractResourceModel.hxx"
#include <sstream>
#include <iostream>

AbstractResourceModel::AbstractResourceModel()
{
}

AbstractResourceModel::~AbstractResourceModel()
{
}

std::string AbstractResourceModel::getMaxDuration()const
{
  int hours = getMaxDurationHours();
  int minutes = getMaxDurationMinutes();
  if(hours<0)
    hours = 0;
  if(hours>99)
    hours = 99;
  if(minutes<0)
    minutes = 5;
  if(minutes > 59)
    minutes = 59;
  std::stringstream ss;
  // hours=0 && minutes=0 => use default values
  if(hours > 0 || minutes > 0)
  {
    if(hours<10)
      ss << "0";
    ss << hours << ":";
    if(minutes<10)
      ss << "0";
    ss << minutes;
  }
  return ss.str();
}
