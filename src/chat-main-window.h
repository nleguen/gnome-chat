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


#ifndef CHAT_MAIN_WINDOW_H
#define CHAT_MAIN_WINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CHAT_TYPE_MAIN_WINDOW (chat_main_window_get_type ())

#define CHAT_MAIN_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   CHAT_TYPE_MAIN_WINDOW, ChatMainWindow))

#define CHAT_MAIN_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   CHAT_TYPE_MAIN_WINDOW, ChatMainWindowClass))

#define CHAT_IS_MAIN_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   CHAT_TYPE_MAIN_WINDOW))

#define CHAT_IS_MAIN_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   CHAT_TYPE_MAIN_WINDOW))

#define CHAT_MAIN_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   CHAT_TYPE_MAIN_WINDOW, ChatMainWindowClass))

typedef struct _ChatMainWindow        ChatMainWindow;
typedef struct _ChatMainWindowClass   ChatMainWindowClass;
typedef struct _ChatMainWindowPrivate ChatMainWindowPrivate;

struct _ChatMainWindow
{
  GtkApplicationWindow parent_instance;
  ChatMainWindowPrivate *priv;
};

struct _ChatMainWindowClass
{
  GtkApplicationWindowClass parent_class;
};

GType                  chat_main_window_get_type               (void) G_GNUC_CONST;

GtkWidget             *chat_main_window_new                    (GtkApplication *application);

void                   chat_main_window_show_about             (ChatMainWindow *self);

void                   chat_main_window_add_conversation_cb    (ChatMainWindow *self);

G_END_DECLS

#endif /* CHAT_MAIN_WINDOW_H */
