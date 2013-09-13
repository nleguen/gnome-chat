/*
 * Chat - instant messaging client for GNOME
 * Copyright © 2013 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#ifndef CHAT_UTILS_H
#define CHAT_UTILS_H

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <telepathy-glib/telepathy-glib.h>

G_BEGIN_DECLS

void            chat_utils_get_contact_avatar            (TpContact *contact,
                                                          GCancellable *cancellable,
                                                          GAsyncReadyCallback callback,
                                                          gpointer user_data);

GtkImage       *chat_utils_get_contact_avatar_finish     (TpContact *contact, GAsyncResult *res, GError **error);

GtkImage       *chat_utils_get_contact_avatar_default    (void);

G_END_DECLS

#endif /* CHAT_UTILS_H */
