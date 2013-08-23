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

#include "chat-main-toolbar.h"


struct _ChatMainToolbarPrivate
{
  GtkWidget *toolbar_left;
};


G_DEFINE_TYPE_WITH_PRIVATE (ChatMainToolbar, chat_main_toolbar, GTK_TYPE_BOX);


static void
chat_main_toolbar_init (ChatMainToolbar *self)
{
  self->priv = chat_main_toolbar_get_instance_private (self);
  gtk_widget_init_template (GTK_WIDGET (self));
}


static void
chat_main_toolbar_class_init (ChatMainToolbarClass *class)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/chat/main-toolbar.ui");
  gtk_widget_class_bind_template_child_private (widget_class, ChatMainToolbar, toolbar_left);
}


GtkWidget *
chat_main_toolbar_new (void)
{
  return g_object_new (CHAT_TYPE_MAIN_TOOLBAR, NULL);
}


GtkWidget *
chat_main_toolbar_get_toolbar_left (ChatMainToolbar *self)
{
  return self->priv->toolbar_left;
}
