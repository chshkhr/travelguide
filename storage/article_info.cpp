#include "article_info.hpp"
#include "distance.hpp"

#include "../env/strings.hpp"
#include "../env/writer.hpp"
#include "../env/reader.hpp"

#include "../std/iterator.hpp"
#include "../std/algorithm.hpp"
#include "../std/cmath.hpp"


void ArticleInfo::GenerateKey()
{
  m_key = str::MakeNormalizeAndLowerUtf8(m_title);
}

void ArticleInfo::SetAsRedirect(ArticleInfo const & src)
{
  m_url = src.m_url;
  m_thumbnailUrl = src.m_thumbnailUrl;
  m_length = src.m_length;
  m_lat = src.m_lat;
  m_lon = src.m_lon;

  m_redirect = true;
}

namespace
{

void WriteCoord(wr::Writer & w, double d)
{
  w.Write(static_cast<int32_t>(d * 100000));
}

void ReadCoord(rd::Reader & r, double & d)
{
  int32_t i;
  r.Read(i);
  d = i / 100000.0;
}

}

void ArticleInfo::Write(wr::Writer & w) const
{
  w.Write(m_title);
  w.Write(m_url);
  w.Write(m_thumbnailUrl);
  w.Write(m_length);
  w.Write(m_parentIndex);

  WriteCoord(w, m_lat);
  WriteCoord(w, m_lon);

  w.Write(m_redirect);
}

void ArticleInfo::Read(rd::Reader & r)
{
  r.Read(m_title);
  r.Read(m_url);
  r.Read(m_thumbnailUrl);
  r.Read(m_length);
  r.Read(m_parentIndex);

  ReadCoord(r, m_lat);
  ReadCoord(r, m_lon);

  r.Read(m_redirect);

  GenerateKey();
}

double ArticleInfo::Score(double currLat, double currLon) const
{
  if (m_lat != EMPTY_COORD && m_lon != EMPTY_COORD)
    return earth::Distance(m_lat, m_lon, currLat, currLon);
  else
    return 0.0;
}

void ArticleInfo::Swap(ArticleInfo & i)
{
  m_key.swap(i.m_key);
  m_title.swap(i.m_title);
  m_url.swap(i.m_url);
  m_thumbnailUrl.swap(i.m_thumbnailUrl);
  std::swap(m_length, i.m_length);
  std::swap(m_parentIndex, i.m_parentIndex);
  std::swap(m_lat, i.m_lat);
  std::swap(m_lon, i.m_lon);
  std::swap(m_redirect, i.m_redirect);
}

namespace
{
  bool EqualCoord(double d1, double d2)
  {
    return fabs(d1 - d2) < 1.0E-7;
  }
}

bool ArticleInfo::operator == (ArticleInfo const & r) const
{
  return (m_key == r.m_key &&
          m_title == r.m_title &&
          m_url == r.m_url &&
          m_thumbnailUrl == r.m_thumbnailUrl &&
          m_parentIndex == r.m_parentIndex &&
          EqualCoord(m_lat, r.m_lat) &&
          EqualCoord(m_lon, r.m_lon));
}
