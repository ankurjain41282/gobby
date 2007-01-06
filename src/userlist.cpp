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

#include "userlist.hpp"

Gobby::UserList::Columns::Columns()
{
	add(name);
	add(color);
}

Gobby::UserList::Columns::~Columns()
{
}

Gobby::UserList::UserList()
{
	m_list_data = Gtk::ListStore::create(m_list_cols);
	m_list_view.set_model(m_list_data);

	m_list_view.append_column("Name", m_list_cols.name);
	m_list_view.append_column("Color", m_list_cols.color);

	set_shadow_type(Gtk::SHADOW_IN);
	add(m_list_view);

	set_sensitive(false);
}

Gobby::UserList::~UserList()
{
}
