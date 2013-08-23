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


#include "config.h"

#include "chat-application.h"
#include "chat-embed.h"
#include "chat-main-toolbar.h"
#include "polari-fixed-size-frame.h"


struct _ChatEmbedPrivate
{
  GtkSizeGroup *size_group_left;
  GtkWidget *sidebar_frame;
  GtkWidget *toolbar;
};

static void chat_embed_buildable_init (GtkBuildableIface *iface);


G_DEFINE_TYPE_WITH_CODE (ChatEmbed, chat_embed, GTK_TYPE_BOX,
                         G_ADD_PRIVATE (ChatEmbed)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                                                chat_embed_buildable_init));


static void
chat_embed_window_added (ChatEmbed *self, GtkWindow *window)
{
  gtk_window_set_titlebar (window, self->priv->toolbar);
}


static void
chat_embed_dispose (GObject *object)
{
  ChatEmbed *self = CHAT_EMBED (object);
  ChatEmbedPrivate *priv = self->priv;

  g_clear_object (&priv->size_group_left);

  G_OBJECT_CLASS (chat_embed_parent_class)->dispose (object);
}


static void
chat_embed_init (ChatEmbed *self)
{
  ChatEmbedPrivate *priv;
  GtkApplication *app;
  GtkWidget *toolbar_left;

  self->priv = chat_embed_get_instance_private (self);
  priv = self->priv;

  gtk_widget_init_template (GTK_WIDGET (self));

  app = chat_application_new ();
  g_signal_connect_swapped (app, "window-added", G_CALLBACK (chat_embed_window_added), self);
  g_object_unref (app);

  priv->toolbar = chat_main_toolbar_new ();
  toolbar_left = chat_main_toolbar_get_toolbar_left (CHAT_MAIN_TOOLBAR (priv->toolbar));

  priv->size_group_left = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
  gtk_size_group_add_widget (priv->size_group_left, priv->sidebar_frame);
  gtk_size_group_add_widget (priv->size_group_left, toolbar_left);
}


static void
chat_embed_class_init (ChatEmbedClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  object_class->dispose = chat_embed_dispose;

  g_type_ensure (POLARI_TYPE_FIXED_SIZE_FRAME);
  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/chat/embed.ui");
  gtk_widget_class_bind_template_child_private (widget_class, ChatEmbed, sidebar_frame);
}


static void
chat_embed_buildable_init (GtkBuildableIface *iface)
{
}


GtkWidget *
chat_embed_new (void)
{
  return g_object_new (CHAT_TYPE_EMBED, NULL);
}
