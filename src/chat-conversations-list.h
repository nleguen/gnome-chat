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


#ifndef CHAT_CONVERSATIONS_LIST_H
#define CHAT_CONVERSATIONS_LIST_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CHAT_TYPE_CONVERSATIONS_LIST (chat_conversations_list_get_type ())

#define CHAT_CONVERSATIONS_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   CHAT_TYPE_CONVERSATIONS_LIST, ChatConversationsList))

#define CHAT_CONVERSATIONS_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   CHAT_TYPE_CONVERSATIONS_LIST, ChatConversationsListClass))

#define CHAT_IS_CONVERSATIONS_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   CHAT_TYPE_CONVERSATIONS_LIST))

#define CHAT_IS_CONVERSATIONS_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   CHAT_TYPE_CONVERSATIONS_LIST))

#define CHAT_CONVERSATIONS_LIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   CHAT_TYPE_CONVERSATIONS_LIST, ChatConversationsListClass))

typedef struct _ChatConversationsList        ChatConversationsList;
typedef struct _ChatConversationsListClass   ChatConversationsListClass;
typedef struct _ChatConversationsListPrivate ChatConversationsListPrivate;

struct _ChatConversationsList
{
  GtkListBox parent_instance;
  ChatConversationsListPrivate *priv;
};

struct _ChatConversationsListClass
{
  GtkListBoxClass parent_class;
};

GType                  chat_conversations_list_get_type               (void) G_GNUC_CONST;

GtkWidget             *chat_conversations_list_new                    (void);

G_END_DECLS

#endif /* CHAT_CONVERSATIONS_LIST_H */
