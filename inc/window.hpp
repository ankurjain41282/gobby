/* gobby - A GTKmm driven libobby client
 * Copyright (C) 2005 0x539 dev group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _GOBBY_WINDOW_HPP_
#define _GOBBY_WINDOW_HPP_

#include <gtkmm/window.h>
#include <gtkmm/paned.h>
#include <gtkmm/frame.h>
#include "header.hpp"
#include "folder.hpp"
#include "userlist.hpp"
#include "chat.hpp"

namespace Gobby
{

class Window : public Gtk::Window
{
public:
	Window();
	~Window();

protected:
	void on_session_create();
	void on_session_join();
	void on_session_quit();
	void on_quit();

	Gtk::VBox m_mainbox;
	Header m_header;

	Gtk::VPaned m_mainpaned;
	Gtk::HPaned m_subpaned;

	Gtk::Frame m_frame_chat;
	Gtk::Frame m_frame_list;
	Gtk::Frame m_frame_text;

	Folder m_folder;
	UserList m_userlist;
	Chat m_chat;
};

}

#endif // _GOBBY_WINDOW_HPP_
