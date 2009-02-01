/* Gobby - GTK-based collaborative text editor
 * Copyright (C) 2008, 2009 Armin Burgmeier <armin@arbur.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "commands/file-commands.hpp"

#include "commands/file-tasks/task-new.hpp"
#include "commands/file-tasks/task-open-file.hpp"
#include "commands/file-tasks/task-open-location.hpp"
#include "commands/file-tasks/task-save.hpp"
#include "commands/file-tasks/task-save-all.hpp"

Gobby::FileCommands::Task::Task(FileCommands& file_commands):
	m_file_commands(file_commands)
{
}

Gobby::FileCommands::Task::~Task()
{
}

void Gobby::FileCommands::Task::finish()
{
	// Note this could delete this:
	m_signal_finished.emit();
}

Gtk::Window& Gobby::FileCommands::Task::get_parent()
{
	return m_file_commands.m_parent;
}

Gobby::Folder& Gobby::FileCommands::Task::get_folder()
{
	return m_file_commands.m_folder;
}

Gobby::StatusBar& Gobby::FileCommands::Task::get_status_bar()
{
	return m_file_commands.m_status_bar;
}

Gobby::FileChooser& Gobby::FileCommands::Task::get_file_chooser()
{
	return m_file_commands.m_file_chooser;
}

Gobby::Operations& Gobby::FileCommands::Task::get_operations()
{
	return m_file_commands.m_operations;
}

const Gobby::DocumentInfoStorage&
Gobby::FileCommands::Task::get_document_info_storage()
{
	return m_file_commands.m_document_info_storage;
}

Gobby::Preferences& Gobby::FileCommands::Task::get_preferences()
{
	return m_file_commands.m_preferences;
}

Gobby::DocumentLocationDialog&
Gobby::FileCommands::Task::get_document_location_dialog()
{
	if(m_file_commands.m_location_dialog.get() == NULL)
	{
		m_file_commands.m_location_dialog.reset(
			new DocumentLocationDialog(
				m_file_commands.m_parent,
				INF_GTK_BROWSER_MODEL(
					m_file_commands.m_browser.
						get_store())));
	}

	return *m_file_commands.m_location_dialog;
}

Gobby::FileCommands::FileCommands(Gtk::Window& parent, Header& header,
                                  Browser& browser, Folder& folder,
				  StatusBar& status_bar,
                                  FileChooser& file_chooser,
                                  Operations& operations,
				  const DocumentInfoStorage& info_storage,
                                  Preferences& preferences):
	m_parent(parent), m_header(header), m_browser(browser),
	m_folder(folder), m_status_bar(status_bar),
	m_file_chooser(file_chooser), m_operations(operations),
	m_document_info_storage(info_storage), m_preferences(preferences)
{
	header.action_file_new->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_new));
	header.action_file_open->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_open));
	header.action_file_open_location->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_open_location));
	header.action_file_save->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_save));
	header.action_file_save_as->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_save_as));
	header.action_file_save_all->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_save_all));
	header.action_file_close->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_close));
	header.action_file_quit->signal_activate().connect(
		sigc::mem_fun(*this, &FileCommands::on_quit));

	folder.signal_document_changed().connect(
		sigc::mem_fun(*this, &FileCommands::on_document_changed));

	InfGtkBrowserStore* store = browser.get_store();
	m_row_inserted_handler =
		g_signal_connect(G_OBJECT(store), "row-inserted",
		                 G_CALLBACK(on_row_inserted_static), this);
	m_row_deleted_handler =
		g_signal_connect(G_OBJECT(store), "row-deleted",
		                 G_CALLBACK(on_row_deleted_static), this);

	update_sensitivity();	
}

Gobby::FileCommands::~FileCommands()
{
	InfGtkBrowserStore* store = m_browser.get_store();
	g_signal_handler_disconnect(G_OBJECT(store), m_row_inserted_handler);
	g_signal_handler_disconnect(G_OBJECT(store), m_row_deleted_handler);
}

void Gobby::FileCommands::set_task(Task* task)
{
	task->signal_finished().connect(sigc::mem_fun(
		*this, &FileCommands::on_task_finished));
	m_task.reset(task);
}

void Gobby::FileCommands::on_document_changed(DocWindow* document)
{
	update_sensitivity();
}

void Gobby::FileCommands::on_row_inserted()
{
	update_sensitivity();
}

void Gobby::FileCommands::on_row_deleted()
{
	update_sensitivity();
}

void Gobby::FileCommands::on_task_finished()
{
	m_task.reset(NULL);
}

void Gobby::FileCommands::on_new()
{
	set_task(new TaskNew(*this));
}

void Gobby::FileCommands::on_open()
{
	set_task(new TaskOpenFile(*this));
}

void Gobby::FileCommands::on_open_location()
{
	set_task(new TaskOpenLocation(*this));
}

void Gobby::FileCommands::on_save()
{
	// TODO: Encoding selection in file chooser
	DocWindow* document = m_folder.get_current_document();
	g_assert(document != NULL);

	const DocumentInfoStorage::Info* info =
		m_document_info_storage.get_info(
			document->get_info_storage_key());

	if(info != NULL && !info->uri.empty())
	{
		m_operations.save_document(
			*document, m_folder, info->uri, info->encoding,
			info->eol_style);
	}
	else
	{
		on_save_as();
	}
}

void Gobby::FileCommands::on_save_as()
{
	DocWindow* document = m_folder.get_current_document();
	g_assert(document != NULL);

	set_task(new TaskSave(*this, *document));
}

void Gobby::FileCommands::on_save_all()
{
	set_task(new TaskSaveAll(*this));
}

void Gobby::FileCommands::on_close()
{
	DocWindow* document = m_folder.get_current_document();
	g_assert(document != NULL);

	m_folder.remove_document(*document);
}

void Gobby::FileCommands::on_quit()
{
	m_parent.hide();
}

void Gobby::FileCommands::update_sensitivity()
{
	GtkTreeIter dummy_iter;
	bool create_sensitivity = gtk_tree_model_get_iter_first(
		GTK_TREE_MODEL(m_browser.get_store()), &dummy_iter);
	gboolean active_sensitivity = m_folder.get_current_document() != NULL;

	m_header.action_file_new->set_sensitive(create_sensitivity);
	m_header.action_file_open->set_sensitive(create_sensitivity);
	m_header.action_file_open_location->set_sensitive(create_sensitivity);

	m_header.action_file_save->set_sensitive(active_sensitivity);
	m_header.action_file_save_as->set_sensitive(active_sensitivity);
	m_header.action_file_save_all->set_sensitive(active_sensitivity);
	m_header.action_file_close->set_sensitive(active_sensitivity);
}
