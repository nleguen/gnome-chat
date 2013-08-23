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


#ifndef CHAT_MAIN_TOOLBAR_H
#define CHAT_MAIN_TOOLBAR_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CHAT_TYPE_MAIN_TOOLBAR (chat_main_toolbar_get_type ())

#define CHAT_MAIN_TOOLBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   CHAT_TYPE_MAIN_TOOLBAR, ChatMainToolbar))

#define CHAT_MAIN_TOOLBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   CHAT_TYPE_MAIN_TOOLBAR, ChatMainToolbarClass))

#define CHAT_IS_MAIN_TOOLBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   CHAT_TYPE_MAIN_TOOLBAR))

#define CHAT_IS_MAIN_TOOLBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   CHAT_TYPE_MAIN_TOOLBAR))

#define CHAT_MAIN_TOOLBAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   CHAT_TYPE_MAIN_TOOLBAR, ChatMainToolbarClass))

typedef struct _ChatMainToolbar        ChatMainToolbar;
typedef struct _ChatMainToolbarClass   ChatMainToolbarClass;
typedef struct _ChatMainToolbarPrivate ChatMainToolbarPrivate;

struct _ChatMainToolbar
{
  GtkBox parent_instance;
  ChatMainToolbarPrivate *priv;
};

struct _ChatMainToolbarClass
{
  GtkBoxClass parent_class;
};

GType                  chat_main_toolbar_get_type               (void) G_GNUC_CONST;

GtkWidget             *chat_main_toolbar_new                    (void);

GtkWidget             *chat_main_toolbar_get_toolbar_left       (ChatMainToolbar *self);

G_END_DECLS

#endif /* CHAT_MAIN_TOOLBAR_H */
