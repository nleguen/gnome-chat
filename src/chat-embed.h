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


#ifndef CHAT_EMBED_H
#define CHAT_EMBED_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CHAT_TYPE_EMBED (chat_embed_get_type ())

#define CHAT_EMBED(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   CHAT_TYPE_EMBED, ChatEmbed))

#define CHAT_EMBED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   CHAT_TYPE_EMBED, ChatEmbedClass))

#define CHAT_IS_EMBED(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   CHAT_TYPE_EMBED))

#define CHAT_IS_EMBED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   CHAT_TYPE_EMBED))

#define CHAT_EMBED_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   CHAT_TYPE_EMBED, ChatEmbedClass))

typedef struct _ChatEmbed        ChatEmbed;
typedef struct _ChatEmbedClass   ChatEmbedClass;
typedef struct _ChatEmbedPrivate ChatEmbedPrivate;

struct _ChatEmbed
{
  GtkBox parent_instance;
  ChatEmbedPrivate *priv;
};

struct _ChatEmbedClass
{
  GtkBoxClass parent_class;
};

GType                  chat_embed_get_type               (void) G_GNUC_CONST;

GtkWidget             *chat_embed_new                    (void);

G_END_DECLS

#endif /* CHAT_EMBED_H */
