/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file messages.h
   \brief Message classes
   \author Jens Thiele
*/

#ifndef MESSAGES_H
#define MESSAGES_H

#include "typedefs.h"
#include "version.h"
#include "team.h"
#include <dope/timestamp.h>

#define USE_RAW_PROTOCOL 1

#if USE_RAW_PROTOCOL == 1

// layer 1 network byte order raw stream
#include <dope/nbostream.h>
typedef NBOOutStream<std::streambuf> OutProto;
typedef NBOInStream<std::streambuf> InProto;

#elif USE_XML_PROTOCOL == 1

// layer 2 mini xml output stream
#include <dope/minixml.h>
// layer 2 xml sax input
#include <dope/xmlsaxinstream.h>
typedef XMLOutStream<std::streambuf> OutProto;
typedef XMLSAXInStream<std::streambuf> InProto;

#endif


struct Greeting
{
  Greeting()
    : m_dopeVersion(dope_major_version,dope_minor_version,dope_micro_version),
      m_adicVersion(PACKAGE_MAJOR_VERSION,PACKAGE_MINOR_VERSION,PACKAGE_MICRO_VERSION)
  {}
  ~Greeting(){}
  
  Version m_dopeVersion;
  Version m_adicVersion;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(m_dopeVersion).SIMPLE(m_adicVersion);
  }
};
DOPE_CLASS(Greeting);
template <typename Layer2>
inline void composite(Layer2 &layer2, Greeting &g)
{
  g.composite(layer2);
}

struct ServerGreeting : public Greeting
{
  //! the player ID's the client got
  std::vector<PlayerID> m_players;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    Greeting::composite(l2);
    l2.SIMPLE(m_players);
  }
};

DOPE_CLASS(ServerGreeting);
template <typename Layer2>
inline void composite(Layer2 &layer2, ServerGreeting &g)
{
  g.composite(layer2);
}


struct User
{
  User()
  {}
  
  User(const char *uname, const char *tname=NULL) 
  {
    m_uname=uname ? uname : "";
    m_tname=tname ? tname : "";
  }
  
  std::string m_uname;
  std::string m_tname;
};
DOPE_CLASS(User);
template <typename Layer2>
inline void composite(Layer2 &layer2, User &c)
{
  layer2.simple(c.m_uname,"uname").simple(c.m_tname,"tname");
}

struct UserSetting
{
  //! the users (players)
  std::vector<User> users;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(users);
  }
};
DOPE_CLASS(UserSetting);
template <typename Layer2>
inline void composite(Layer2 &layer2, UserSetting &c)
{
  c.composite(layer2);
}

struct ClientGreeting : public Greeting
{
  UserSetting m_userSetting;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    Greeting::composite(l2);
    l2.SIMPLE(m_userSetting);
  }
};
DOPE_CLASS(ClientGreeting);
template <typename Layer2>
inline void composite(Layer2 &layer2, ClientGreeting &c)
{
  c.composite(layer2);
}

//! a chat message
struct ChatMessage
{
  //! username or teamname or empty
  /*! 
    server must check this to prevent faked names 
    => client should leave this field empty and server will fill it
  */
  std::string sender;
  //! the message content
  std::string message;
  //! global message ?
  /*!
    global messages are sent to every client
    non-global messages are sent to team mates
  */
  bool global;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(sender).SIMPLE(message).SIMPLE(global);
  }
};
DOPE_CLASS(ChatMessage);
template <typename Layer2>
inline void composite(Layer2 &layer2, ChatMessage &c)
{
  c.composite(layer2);
}

//! new client connected
struct NewClient
{
  //! all player names - in the game
  std::vector<std::string> playerNames;
  //! all teams - in the game
  std::vector<Team> teams;
};
DOPE_CLASS(NewClient);
template <typename Layer2>
inline void composite(Layer2 &layer2, NewClient &o)
{
  layer2.simple(o.playerNames,"playerNames").simple(o.teams,"teams");
}

//! end of game message
/*!
  this message is sent by the server to all clients on the end of one game/level
*/
struct EndGame
{
  //! 0=server quit 1=winner 2=draw
  uint8_t reason;
  //! only set if reason==1
  TeamID winner;

  template <typename Layer2>
  void composite(Layer2 &l2)
  {
    l2.SIMPLE(reason).SIMPLE(winner);
  }
};
DOPE_CLASS(EndGame);
template <typename Layer2>
inline void composite(Layer2 &layer2, EndGame &o)
{
  o.composite(layer2);
}

//! ping message
struct Ping
{
  TimeStamp m_ctime;
  TimeStamp m_stime;
};
DOPE_CLASS(Ping);
template <typename Layer2>
inline void composite(Layer2 &layer2, Ping &o)
{
  layer2.simple(o.m_ctime,"ctime").simple(o.m_stime,"stime");
}

#endif
