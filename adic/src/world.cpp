#include "world.h"
#include "wall.h"
#include "polygon.h"

World::EdgeIterator::EdgeIterator
(const World &world, FWEdge::RoomID room) 
: m_w(world)
{
  assert(room>=0);
  assert(room<m_w.m_rooms.size());
  const Room &r=m_w.m_rooms[room];
  m_cw=r.m_cw;
  m_c=m_b=r.m_e;
  m_end=false;
}

World::EdgeIterator &
World::EdgeIterator::inc() 
{
  if (m_end)
    return *this;
  // last end vertice
  FWEdge::VID le;
  if (m_cw) {
    le=m_w.m_edges[m_c].m_ev;
    m_c=m_w.m_edges[m_c].m_ncw;
  }else{
    le=m_w.m_edges[m_c].m_sv;
    m_c=m_w.m_edges[m_c].m_nccw;
  }
  // update clockwise
  m_cw=(m_w.m_edges[m_c].m_sv==le);
  m_end=(m_c==m_b);
  return *this;
}

World::World(const Mesh &mesh)
{
  init();
  setFromMesh(mesh);
}

void
World::setFromMesh(const Mesh &mesh)
{
  //  DOPE_WARN("called");
  // this does not have to be fast since it is called only once or twice ;-)

  // copy all vertices
  m_vertices=mesh.vertices;
  m_startPoints=mesh.startPoints;
  m_startObjects=mesh.startObjects;
  // constuct our new edge-list

  typedef std::pair<Edge::VID,Edge::VID> Key;
  typedef FWEdge::EID Value;
  // first edge ID of the current room
  FWEdge::EID fe;
  // the first edge was clockwise
  bool feCW;
  
  // the ID of the last edge we visisted
  FWEdge::EID lastID=FWEdge::noEdge;
  // the last edge was clockwise
  bool lastCW;
  
  // half edges already stored
  std::map<Key,Value> stored;
  for (Mesh::EID ce=0;ce<mesh.edgelist.size();++ce)
    {
      if (mesh.edgelist[ce].vid==-1)
	continue;
      // first edge of room
      bool firstEdge=(lastID==FWEdge::noEdge);

      // next edge in this room
      Mesh::EID ne=mesh.getNCW(ce);
      // is this the "last" edge of this room ?
      bool leor=mesh.lastEdgeOfRoom(ce);
      // if it is the first edge remember this id
      
      // we assume that all rooms are in clockwise or counter clockwise order
      // each edge is visited in both directions or only once (borders)
      std::map<Key,Value>::const_iterator it(stored.find(Key(mesh.edgelist[ne].vid,mesh.edgelist[ce].vid)));

      // \todo clean it up this is nearly twice
      if (it!=stored.end())
	{
	  // we visited this edge already - set current full winged edge
	  Value cfweID=it->second;
	  if (firstEdge) {
	    fe=cfweID; // todo here is a bug? (if the first edge is not CW there is a bug)
	    feCW=false;
	  }
	  FWEdge &cfwe=m_edges[cfweID];
	  // cfwe.m_nccw=;
	  if (!firstEdge) {
	    cfwe.m_pccw=lastID;
	    if (lastCW)
	      m_edges[lastID].m_ncw=cfweID;
	    else
	      m_edges[lastID].m_nccw=cfweID;
	  }
	  cfwe.m_rccw=m_rooms.size();
	  cfwe.m_etccw=mesh.edgelist[ce].type;
	  if (leor) {
	    // we finished this room
	    if (feCW)
	      m_edges[fe].m_pcw=cfweID;
	    else
	      m_edges[fe].m_pccw=cfweID;
	    cfwe.m_nccw=fe;
	    
	    Room newRoom;
	    newRoom.m_e=fe;
	    newRoom.m_cw=feCW;
	    m_rooms.push_back(newRoom);
	    fe=lastID=FWEdge::noEdge;
	  }else{
	    lastID=cfweID;
	    lastCW=false;
	  }
	}
      else
	{
	  // we did not visit this edge yet

	  // the id of the new edge
	  FWEdge::EID newID=m_edges.size();
	  if (firstEdge) {
	    fe=newID;
	    feCW=true;
	  }
	  // fill in the things we know already
	  FWEdge newEdge;
	  newEdge.m_sv=mesh.edgelist[ce].vid;
	  newEdge.m_ev=mesh.edgelist[ne].vid;
	  // if it is the first edge we have to fill this in later
	  if (!firstEdge) {
	    newEdge.m_pcw=lastID;
	    if (lastCW)
	      m_edges[lastID].m_ncw=newID;
	    else
	      m_edges[lastID].m_nccw=newID;
	  }
	  // we don't know yet but the constructor initialized to noEdge/noRoom
	  // 	  newEdge.m_ncw=FWEdge::noEdge;
	  //	  newEdge.m_nccw=FWEdge::noEdge;
	  //	  newEdge.m_pccw=FWEdge::noEdge;
	  newEdge.m_rcw=m_rooms.size();
	  newEdge.m_etcw=mesh.edgelist[ce].type;
	  
	  //	  newEdge.m_rccw=FWEdge::noRoom;
	  
	  if (leor) {
	    // we finished this room
	    if (feCW)
	      m_edges[fe].m_pcw=newID;
	    else
	      m_edges[fe].m_pccw=newID;
	    newEdge.m_ncw=fe;

	    Room newRoom;
	    newRoom.m_e=fe;
	    newRoom.m_cw=feCW;
	    m_rooms.push_back(newRoom);
	    fe=lastID=FWEdge::noEdge;
	  }else{
	    lastID=newID;
	    lastCW=true;
	  }

	  // calculate corresponding wall
	  DOPE_CHECK(newEdge.m_sv<m_vertices.size()&&newEdge.m_ev<m_vertices.size());
	  newEdge.m_wall=Wall(Line(m_vertices[newEdge.m_sv],m_vertices[newEdge.m_ev]));

	  m_edges.push_back(newEdge);
	  stored[Key(mesh.edgelist[ce].vid,mesh.edgelist[ne].vid)]=newID;
	  //	  std::cout << "Stored "<<mesh.edgelist[ce].vid<<","<<mesh.edgelist[ne].vid<<std::endl;
	}
    }
  for (unsigned r=0;r<m_rooms.size();++r) {
    m_rooms[r].m_poly=getLineLoop(r);

    // loop through edges of this room
    EdgeIterator end(*this);
    for (EdgeIterator it(*this,r);it!=end;++it)
      {
	if (it.getEdge().isDoor())
	  continue;
	m_rooms[r].m_walls.push_back(it.getEdge().m_wall);
      }
  }
}

bool
World::collide
(const Circle &c, FWEdge::RoomID room, V2D &cv) const
{
  // loop through walls
  const Room &r(m_rooms[room]);
  unsigned nw=r.m_walls.size();
  bool res=false;
  V2D subc;
  for (unsigned w=0;w<nw;++w) {
    if (r.m_walls[w].collide(c,subc)) {
      res=true;
      cv+=subc;
    }
  }
  return res;
}

FWEdge::RoomID
World::inRoom(const V2D &p)
{
  for (FWEdge::RoomID r=0;r<m_rooms.size();++r)
    {
      if (isInRoom(p,r))
	return r;
    }
  return FWEdge::noRoom;
}

std::vector<V2D> 
World::getLineLoop(FWEdge::RoomID room) const
{
  std::vector<V2D> lineloop;
  EdgeIterator it(*this,room);
  // not needed because the last end point produces this point
  // lineloop.push_back(it.getStartPoint());  
  for (;it!=EdgeIterator(*this);++it)
    lineloop.push_back(it.getEndPoint());
  return lineloop;
}

bool 
World::isInRoom(const V2D &p, FWEdge::RoomID r)
{
  assert(r<m_rooms.size());
  return m_rooms[r].m_poly.inside(p);
  /*
  const std::vector<V2D> &ll(getLineLoop(r));
  bool res=Polygon(ll).inside(p);
  if (res) {
    std::cerr << p << " in "; 
    for (unsigned i=0;i<ll.size();++i)
      std::cerr << ll[i] << ":";
  }else{
    std::cerr << p << " not in ";
    for (unsigned i=0;i<ll.size();++i)
      std::cerr << ll[i] << ":";
  }
  std::cerr << std::endl;
  return res;*/
}

std::vector<FWEdge::EID>
World::getAllDoors() const
{
  std::vector<FWEdge::EID> r;
  for (unsigned e=0;e<m_edges.size();++e)
    {
      if (m_edges[e].isDoor())
	r.push_back(e);
    }
  return r;
}


