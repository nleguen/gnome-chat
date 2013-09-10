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


#ifndef CHAT_CONVERSATION_VIEW_H
#define CHAT_CONVERSATION_VIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CHAT_TYPE_CONVERSATION_VIEW (chat_conversation_view_get_type ())

#define CHAT_CONVERSATION_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   CHAT_TYPE_CONVERSATION_VIEW, ChatConversationView))

#define CHAT_CONVERSATION_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   CHAT_TYPE_CONVERSATION_VIEW, ChatConversationViewClass))

#define CHAT_IS_CONVERSATION_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   CHAT_TYPE_CONVERSATION_VIEW))

#define CHAT_IS_CONVERSATION_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   CHAT_TYPE_CONVERSATION_VIEW))

#define CHAT_CONVERSATION_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   CHAT_TYPE_CONVERSATION_VIEW, ChatConversationViewClass))

typedef struct _ChatConversationView        ChatConversationView;
typedef struct _ChatConversationViewClass   ChatConversationViewClass;
typedef struct _ChatConversationViewPrivate ChatConversationViewPrivate;

struct _ChatConversationView
{
  GtkTextView parent_instance;
  ChatConversationViewPrivate *priv;
};

struct _ChatConversationViewClass
{
  GtkTextViewClass parent_class;
};

GType                  chat_conversation_view_get_type               (void) G_GNUC_CONST;

GtkWidget             *chat_conversation_view_new                    (void);

G_END_DECLS

#endif /* CHAT_CONVERSATION_VIEW_H */
